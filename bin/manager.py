#!/usr/bin/python3

import sys
import json
import time
import yaml
import signal
import subprocess
import os
import fcntl

# 如果有并发，需等待前面的完成
__lock_fp = open(__file__)
fcntl.flock(__lock_fp.fileno(), fcntl.LOCK_EX)

# 切换到文件目录，支持全路径运行
g_work_path = os.path.dirname(os.path.abspath(__file__))
os.chdir(g_work_path)


if not os.path.exists('../data'):
    os.mkdir('../data')
if not os.path.exists('../log'):
    os.mkdir('../log')


def get_app_log():
    import logging
    import logging.handlers
    app_log = logging.getLogger("app_log")
    app_log.setLevel(logging.NOTSET)
    channel = logging.handlers.RotatingFileHandler(
        filename="../log/manager.log",
        maxBytes=256*1024*1024,
        backupCount=4)
    formatter = logging.Formatter(
        "%(asctime)s|%(filename)s:%(lineno)s|pid:%(process)d|%(levelname)s|%(message)s")
    channel.setFormatter(formatter)
    app_log.addHandler(channel)
    return app_log


g_app_log = get_app_log()

# 启动配置
g_loglevel_options = ('trace', 'debug', 'info', 'warning', 'error')
g_process_loglevel = 'info'
g_strategy_data_file = '../data/manager_strategy.data'
g_data_feed_dir = '../conf/data_feed'
g_data_feed_conf = []
if os.path.exists(g_data_feed_dir):
    g_data_feed_conf = [yaml.safe_load(open(os.path.join(g_data_feed_dir, entry))) for entry in os.listdir(g_data_feed_dir)]
g_process_priority_lists = []
# 启动顺序1
g_process_priority_lists.append(['data_center'])
g_process_priority_lists.append(['trader'])
# 启动顺序2
_1 = ['shared_memory_data_dispatcher ' + gateway["name"] for gateway in g_data_feed_conf if gateway["enable"]]
g_process_priority_lists.append(_1)
# 进程汇总
g_process_list = []
for _2 in g_process_priority_lists:
    g_process_list += _2
del _1, _2

os.environ['LD_LIBRARY_PATH'] = os.path.join(g_work_path, "../lib")


def run_shell(shell_cmd, cwd=None):
    g_app_log.debug("run_shell|%s", shell_cmd)
    proc = subprocess.Popen(shell_cmd, stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE, shell=True, cwd=cwd)
    outs, errs = proc.communicate()
    outs = outs.decode('utf-8').strip()
    errs = errs.decode('utf-8').strip()
    if proc.returncode != 0:
        g_app_log.error(f"shell_cmd={shell_cmd}|outs={outs}|errs={errs}")
        raise RuntimeError(f"Fail to execute cmd: {shell_cmd}")
    return outs, errs


class StrategyHelp:
    def get_loader_list(self):
        return ['strategy_loader', 'pystrategy_loader.py']

    def get_process(self, strategy):
        if strategy.startswith("py"):
            return "pystrategy_loader.py " + strategy
        else:
            return "strategy_loader " + strategy

    def remove_local_data(self):
        if os.path.exists(g_strategy_data_file):
            os.remove(g_strategy_data_file)

    def get_process_list(self, strategy_list):
        return [self.get_process(strategy) for strategy in strategy_list]

    def get_local_strategy_process_list(self):
        data = self._load_data()
        return self.get_process_list(data.keys())

    def _load_data(self):
        if not os.path.exists(g_strategy_data_file):
            return {}
        with open(g_strategy_data_file) as fp:
            data = json.load(fp)
        return data

    def _dump_data(self, data):
        with open(g_strategy_data_file, "w") as fp:
            json.dump(data, fp)

    def remove_strategy(self, strategy_list):
        data = self._load_data()
        for strategy in strategy_list:
            data.pop(strategy, None)
        self._dump_data(data)

    def add_strategy(self, strategy_list):
        data = self._load_data()
        for strategy in strategy_list:
            data[strategy] = 1
        self._dump_data(data)


class ProcessHelp:
    def __init__(self):
        self.process_dict = None

    def fresh(self):
        #out = run_shell("ps -au%s -o pid,cmd" % getpass.getuser())[0]
        out = run_shell("ps -eo pid,cmd")[0]
        self.process_dict = {}
        for line in out.split('\n')[1:]:  # remove first line  PID CMD
            pid, cmd = line.strip().split(' ', 1)
            self.process_dict[int(pid)] = cmd

    def _is_running(self, process):
        start_cmd_prefix = self._get_start_cmd_prefix(process)
        for cmd in self.process_dict.values():
            if start_cmd_prefix in cmd:
                return True
        return False

    def get_not_running_list(self, process_list):
        assert isinstance(process_list, list)
        self.fresh()
        ret = [
            process for process in process_list if not self._is_running(process)]
        g_app_log.debug("get_not_running_list %s ===> %s", process_list, ret)
        return ret

    def get_running_list(self, process_list):
        assert isinstance(process_list, list)
        self.fresh()
        ret = [
            process for process in process_list if self._is_running(process)]
        g_app_log.debug("get_running_list %s ===> %s", process_list, ret)
        return ret

    def _stop(self, process, sig):
        start_cmd_prefix = self._get_start_cmd_prefix(process)
        for pid, cmd in self.process_dict.items():
            if start_cmd_prefix in cmd:
                g_app_log.info("kill %d %s", pid, start_cmd_prefix)
                try:
                    os.kill(pid, sig)
                except OSError as e:
                    if e.errno != 3:
                        raise

    def stop_all(self, process_list, kill9=False):
        if not isinstance(process_list, (list, tuple)):
            process_list = [process_list]
        g_app_log.debug("stop_all process_list = %s kill9=%s",
                        process_list, kill9)
        self.fresh()
        sig = signal.SIGTERM
        if kill9:
            sig = signal.SIGKILL
        for process in process_list:
            self._stop(process, sig)

    def start_all(self, process_list, cwd, loglevel):
        assert isinstance(process_list, list)
        g_app_log.debug("start_all process_list=%s cwd=%s", process_list, cwd)
        for process in process_list:
            nohup_start_cmd = self._get_nohub_start_cmd(process, loglevel)
            run_shell(nohup_start_cmd, cwd)
            time.sleep(1)

    def _get_start_cmd_prefix(self, process):
        return './{process} '.format(process=process)  # 最后1个有个空格

    def _get_nohub_start_cmd(self, process, loglevel):
        logprefix = "../log/{process}".format(process=process)
        logprefix = '-'.join(logprefix.split())  # 去掉空白字符
        args = {
            'conf_dir': "../conf",
            'process': process,
            'loglevel': loglevel,
            'logfile':      logprefix + ".log",  # ../log/xxx.log
            'nohup_output': logprefix + ".nohup"  # ../log/xxx.nohup
        }
        start_cmd = self._get_start_cmd_prefix(
            process) + ' {logfile} {conf_dir} {loglevel}'.format(**args)
        args['start_cmd'] = start_cmd
        nohup_start_cmd = 'nohup {start_cmd} >"../log/{nohup_output}" 2>&1 &'.format(
            **args)
        return nohup_start_cmd


def stop(process_list, stop_all_strategy):
    strategy_help = StrategyHelp()
    if stop_all_strategy:
        process_list = strategy_help.get_loader_list() + process_list
    assert isinstance(process_list, list) and len(process_list) > 0
    phelp = ProcessHelp()
    begin_time = time.time()
    g_app_log.info("stop process_list = %s", process_list)
    # 先等待5秒 kill
    while begin_time + 5 > time.time():
        phelp.stop_all(process_list)
        time.sleep(0.2)
        if not phelp.get_running_list(process_list):
            if stop_all_strategy:
                strategy_help.remove_local_data()
            return
    # 还没停掉，kill9
    phelp.stop_all(process_list, kill9=True)
    time.sleep(1)
    running_list = phelp.get_running_list(process_list)
    if running_list:
        raise RuntimeError(f"Failed to stop {running_list}")
    if stop_all_strategy:
        strategy_help.remove_local_data()


def start(process_list_list):
    assert isinstance(process_list_list, list) and len(process_list_list) > 0
    g_app_log.info("start|%s", process_list_list)
    phelp = ProcessHelp()
    for process_list in process_list_list:
        if not isinstance(process_list, (list, tuple)):
            process_list = [process_list]
        running_list = phelp.get_running_list(process_list)
        if running_list:
            print(f"WARNING: Some programs are running, please stop first: {running_list}")
            exit(-1)
    for process_list in process_list_list:
        if not isinstance(process_list, (list, tuple)):
            process_list = [process_list]
        g_app_log.info("do start|%s", process_list)
        phelp.start_all(process_list, g_work_path, g_process_loglevel)
        time.sleep(1)
        not_running_list = phelp.get_not_running_list(process_list)
        if not_running_list:
            raise RuntimeError(f"Failed to start {not_running_list}")
        print("start %s done" % " ".join(process_list))


def _get_strategy_list_from_py(pymodule: str) -> list:
    pwd = os.getcwd()
    lib_path = os.path.join(pwd, "../lib")
    conf_path = os.path.join(pwd, "../conf")
    module = __import__(pymodule)

    def _gen_strategy(strategy: str, name: str):
        strategy_so = f"lib{strategy}.so"
        instance_json = f"{name}.json"
        instance_so = f"lib{name}.so"
        os.chdir(lib_path)
        if os.path.exists(instance_so):
            os.remove(instance_so)
        os.symlink(strategy_so, instance_so)
        os.chdir(conf_path)
        if not os.path.exists(instance_json):
            raise Exception(f"instance param file {instance_json} does not exist")

    def _gen_pystrategy(strategy: str, name: str):
        strategy_py = f"{strategy}.py"
        instance_json = f"{name}.json"
        instance_py = f"{name}.py"
        os.chdir(pwd)
        if os.path.exists(instance_py):
            os.remove(instance_py)
        os.symlink(strategy_py, instance_py)
        os.chdir(conf_path)
        if not os.path.exists(instance_json):
            raise Exception(f"instance param file {instance_json} does not exist")

    strategy_names = []
    strategy_names_set = set()
    for strategy_info in module.conf["strategy_list"]:
        strategy = strategy_info["strategy"]
        strategy_names_set.add(strategy)
        gen_strategy_fn = _gen_pystrategy if strategy.startswith("py") else _gen_strategy
        for strategy_instance in strategy_info["instance_list"]:
            if strategy_instance == strategy:
                raise Exception(f"strategy name is the same with strategy template: {strategy}")
            if strategy_instance in strategy_names_set:
                raise Exception(f"duplicate strategy name {strategy_instance}")
            strategy_names_set.add(strategy_instance)
            gen_strategy_fn(strategy, strategy_instance)
            strategy_names.append(strategy_instance)
    os.chdir(pwd)
    return strategy_names


def _check_arg_strategy_list(strategy_list: list):
    '''This function may modify strategy_list'''
    assert isinstance(strategy_list, list) and len(strategy_list) > 0
    new_strategy_list = []
    need_modify = False
    for strategy in strategy_list:
        if strategy.endswith(".py") and os.path.exists(strategy):
            need_modify = True
            # remove .py
            new_strategy_list += _get_strategy_list_from_py(
                os.path.basename(strategy)[:-3])
        else:
            new_strategy_list.append(strategy)
    # 修改strategy_list
    if need_modify:
        strategy_list.clear()
        strategy_list.extend(new_strategy_list)
    if len(set(strategy_list)) != len(strategy_list):
        raise Exception(f"duplicate strategy: {strategy_list}")
    for strategy in strategy_list:
        if ' ' in strategy or '\t' in strategy:
            raise RuntimeError(f"illegal strategy name {strategy}")


def start_strategy(strategy_list):
    _check_arg_strategy_list(strategy_list)
    strategy_help = StrategyHelp()
    process_list = strategy_help.get_process_list(strategy_list)
    # 先加入到本地策略
    strategy_help.add_strategy(strategy_list)
    start(process_list)


def stop_strategy(strategy_list):
    _check_arg_strategy_list(strategy_list)
    strategy_help = StrategyHelp()
    process_list = strategy_help.get_process_list(strategy_list)
    g_app_log.info("stop_strategy process_list = %s", process_list)
    stop(process_list, False)
    strategy_help.remove_strategy(strategy_list)
    g_app_log.info("stop_strategy done")


def _status():
    strategy_help = StrategyHelp()
    phelp = ProcessHelp()
    phelp.fresh()
    run_process = []
    stop_process = []

    def fill_run_and_stop(process_list, is_strategy):
        start_cmd_prefix_list = [phelp._get_start_cmd_prefix(
            process) for process in process_list]
        g_app_log.debug("start_cmd_prefix_list=%s", start_cmd_prefix_list)
        for start_cmd_prefix in start_cmd_prefix_list:
            is_run = False
            for cmd in phelp.process_dict.values():
                if start_cmd_prefix in cmd:
                    is_run = True
                    items = cmd.split()
                    if is_strategy:
                        if 'pystrategy_loader.py' in start_cmd_prefix:
                            run_process.append(" ".join(items[1:3]))
                        else:
                            run_process.append(" ".join(items[:2]))
                    elif 'shared_memory_data_dispatcher' in start_cmd_prefix:
                        run_process.append(" ".join(items[:2]))
                    else:
                        run_process.append(items[0])
            if is_strategy:
                continue
            if not is_run:
                stop_process.append(start_cmd_prefix.strip())
    fill_run_and_stop(g_process_list, False)
    fill_run_and_stop(strategy_help.get_loader_list(), True)
    # 本地策略没启动的
    for process in strategy_help.get_local_strategy_process_list():
        start_cmd_prefix = phelp._get_start_cmd_prefix(process)
        if start_cmd_prefix.strip() not in run_process:
            stop_process.append(start_cmd_prefix.strip())
    return run_process, stop_process


def status():
    run_process, stop_process = _status()
    for process in run_process:
        print("%-30s     RUNNING" % process)
    for process in stop_process:
        print("%-30s NOT RUNNING" % process)


def usage():
    def help(cmd, desc):
        print("python3 " + sys.argv[0], "%-44s" % cmd,  desc)
    help("stop_all", "停掉所有程序（含策略）")
    help("restart_all", "停掉所有程序（含策略），启动所有程序（不含策略）")
    help("stop <svr1> <svr2> ...", "停掉程序（不包含策略）")
    help("start <svr1> <svr2> ...", "启动程序（不包含策略）")
    help("start_strategy <strategy1> <strategy2> ...", "启动策略1 策略2 策略3 ...")
    help("stop_strategy <strategy1> <strategy2> ...", "停掉策略1 策略2 策略3 ...")
    help("status",  "状态")


def parse_loglevel():
    global g_process_loglevel
    if len(sys.argv) > 2:
        if sys.argv[-1] in g_loglevel_options:
            g_process_loglevel = sys.argv[-1]
            sys.argv = sys.argv[0:-1]


def main():
    g_app_log.info("%s", sys.argv)
    if len(sys.argv) < 2:
        return usage()
    parse_loglevel()
    cmd = sys.argv[1]
    if cmd == "stop_all":
        stop(g_process_list, True)
    elif cmd == "restart_all":
        stop(g_process_list, True)
        start(g_process_priority_lists)
    elif cmd == "stop":
        stop(sys.argv[2:], False)
    elif cmd == "start":
        start(sys.argv[2:])
    elif cmd == "start_strategy":
        start_strategy(sys.argv[2:])
    elif cmd == "stop_strategy":
        stop_strategy(sys.argv[2:])
    elif cmd == "status":
        status()
    else:
        usage()
    g_app_log.info("%s: done", cmd)


if __name__ == "__main__":
    try:
        main()
    except:
        g_app_log.exception("exception from main")
        raise
