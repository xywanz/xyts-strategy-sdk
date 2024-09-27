import socket
import struct
import json
import argparse


kNetCmdNotifyStrategy = 10002
kNetCmdDataFeedSubscribe = 10016
kNetCmdDataFeedUnsubscribe = 10018

kNetCmdNewOrder = 20002
kNetCmdCancelOrder = 20004
kNetCmdCancelAll = 20016
kNetCmdMakeInternalTrade = 20010
kNetCmdRemoveRiskRule = 20012
kNetCmdModifyRiskRule = 20014

g_trader_ip = '127.0.0.1'
g_trader_port = 10086

g_data_center_ip = '127.0.0.1'
g_data_center_port = 10087


class PackageCodec:
    '''include/xyts/core/network.h'''
    kPkgHeaderSize = 12

    @staticmethod
    def pack_req(req, cmd):
        data_bytes = json.dumps(req).encode('utf-8')
        magic = 8888  # uint16_t
        cmd = cmd  # uint16_t
        seq = 0  # uint32_t
        body_size = len(data_bytes)  # uint32_t
        return struct.pack('!HHII', magic, cmd, seq, body_size) + data_bytes

    @staticmethod
    def unpack_rsp(data_bytes) -> dict:
        head = struct.unpack('!HHII', data_bytes[:PackageCodec.kPkgHeaderSize])
        # print(head)
        size = head[-1]
        s = PackageCodec.kPkgHeaderSize
        e = s + size
        # app_log.info('unpack_rsp.data_bytes=%s', data_bytes)
        return json.loads(data_bytes[s:e])


tcp = socket.socket()


def parse_direction(s):
    map = {'Buy': 1, 'Sell': 2, 'b': 1, 's': 2}
    return map[s] if s in map.keys() else 0


def parse_position_effect(s):
    map = {'Open': 1, 'Close': 2, 'CloseToday': 4,
           'CloseYesterday': 8, 'Auto': 16, 'Smart': 32}
    return map[s] if s in map.keys() else 0


def parse_order_type(s):
    map = {'Limit': 0, 'Market': 1, 'FAK': 2,
           'FOK': 3}
    return map[s] if s in map.keys() else 0


def send_and_recv(req, cmd):
    tcp.send(PackageCodec.pack_req(req, cmd))
    buff = tcp.recv(1024 * 1024)
    rsp = PackageCodec.unpack_rsp(buff)
    print(rsp)


def send_cmd(address, cmd, req):
    tcp.connect(address)
    send_and_recv(req, cmd)


def send_cmd_to_trader(cmd, req):
    send_cmd((g_trader_ip, g_trader_port), cmd, req)


def send_cmd_to_data_center(cmd, req):
    send_cmd((g_data_center_ip, g_data_center_port), cmd, req)


def internal_trade(args):
    req = {
        'src': args.src_strategy,
        'dst': args.dst_strategy,
        'instr': args.instr,
        'volume': args.volume,
        'price': args.price
    }
    send_cmd_to_trader(kNetCmdMakeInternalTrade, req)


def order(args):
    req = {
        'owner': args.strategy,
        'direction': parse_direction(args.direction),
        'position_effect': parse_position_effect(args.position_effect),
        'order_type': parse_order_type(args.type),
        'instr': args.instr,
        'volume': args.volume,
        'price': args.price
    }
    send_cmd_to_trader(kNetCmdNewOrder, req)


def send_market_order(args):
    req = {
        'owner': args.strategy,
        'direction': parse_direction(args.direction),
        'position_effect': parse_position_effect(args.position_effect),
        'order_type': 'Market',
        'instr': args.instr,
        'volume': args.volume,
        'price': 0.0
    }
    send_cmd_to_trader(kNetCmdNewOrder, req)


def cancel(args):
    req = {
        'order_id': args.order_id
    }
    send_cmd_to_trader(kNetCmdCancelOrder, req)


def cancel_all(args):
    req = {
        'account': args.account
    }
    send_cmd_to_trader(kNetCmdCancelAll, req)


def remove_risk_rule(args):
    req = {
        'account_name': args.account_name,
        'risk_rule': args.risk_rule,
    }
    send_cmd_to_trader(kNetCmdRemoveRiskRule, req)


def modify_risk_rule(args):
    req = {
        'account_name': args.account_name,
        'risk_rule': args.risk_rule,
        'cmd': args.cmd
    }
    send_cmd_to_trader(kNetCmdModifyRiskRule, req)


def query_strategy_params(args):
    req = {
        'strategy': args.strategy,
        'cmd': 'query_strategy_params'
    }
    send_cmd_to_data_center(kNetCmdNotifyStrategy, req)


def update_strategy_params(args):
    req = {
        'strategy': args.strategy,
        'cmd': 'update_strategy_params',
        'params_update': json.loads(args.conf)
    }
    send_cmd_to_data_center(kNetCmdNotifyStrategy, req)


def send_cmd_to_strategy(args):
    req = {
        'strategy': args.strategy,
        'cmd': 'send_cmd_to_strategy',
        'data': json.loads(args.data)
    }
    send_cmd_to_data_center(kNetCmdNotifyStrategy, req)


def subscribe_market_data(args):
    req = {
        'subscription_list': args.pattern,
    }
    send_cmd_to_data_center(kNetCmdDataFeedSubscribe, req)


def unsubscribe_market_data(args):
    req = {
        'unsubscription_list': args.pattern,
    }
    send_cmd_to_data_center(kNetCmdDataFeedUnsubscribe, req)


def parse_args_and_run():
    parser = argparse.ArgumentParser(description='xyTS cmdline interface')
    parser.add_argument('--ip', type=str, required=False, default='127.0.0.1')
    parser.add_argument('--port', type=int, required=False, default=10086)

    subparsers = parser.add_subparsers(help='help', dest='subcmd', title='sub commands')

    parser_order = subparsers.add_parser('order', help='send order')
    parser_order.set_defaults(func=order)
    parser_order.add_argument('-s', '--strategy', type=str, required=True)
    parser_order.add_argument('-d', '--direction', type=str, required=True)
    parser_order.add_argument('-e', '--position_effect', type=str, required=False, default='Auto')
    parser_order.add_argument('-t', '--type', type=str, required=False, default='Limit')
    parser_order.add_argument('-i', '--instr', type=str, required=True)
    parser_order.add_argument('-v', '--volume', type=int, required=True)
    parser_order.add_argument('-p', '--price', type=float, required=False, default=0.0)

    parser_mo = subparsers.add_parser('mo', help='send market order')
    parser_mo.set_defaults(func=send_market_order)
    parser_mo.add_argument('-s', '--strategy', type=str, required=True)
    parser_mo.add_argument('-d', '--direction', type=str, required=True)
    parser_mo.add_argument('-e', '--position_effect', type=str, required=False, default='Auto')
    parser_mo.add_argument('-i', '--instr', type=str, required=True)
    parser_mo.add_argument('-v', '--volume', type=int, required=True)

    parser_cancel = subparsers.add_parser('cancel', help='cancel order')
    parser_cancel.set_defaults(func=cancel)
    parser_cancel.add_argument('-i', '--order_id', type=int, required=True)

    parser_cancel_all = subparsers.add_parser('cancel_all', help='cancel all orders')
    parser_cancel_all.set_defaults(func=cancel_all)
    parser_cancel_all.add_argument('-a', '--account', type=str, required=False, default='')

    parser_itrade = subparsers.add_parser('internal_trade', help='internal trade')
    parser_itrade.set_defaults(func=internal_trade)
    parser_itrade.add_argument('-s', '--src_strategy', type=str, required=True)
    parser_itrade.add_argument('-d', '--dst_strategy', type=str, required=True)
    parser_itrade.add_argument('-i', '--instr', type=str, required=True)
    parser_itrade.add_argument('-v', '--volume', type=int, required=True)
    parser_itrade.add_argument('-p', '--price', type=float, required=True)

    parser_rm_risk_rule = subparsers.add_parser('remove_risk_rule', help='remove risk rule')
    parser_rm_risk_rule.set_defaults(func=remove_risk_rule)
    parser_rm_risk_rule.add_argument('-a', '--account_name', type=str, required=True)
    parser_rm_risk_rule.add_argument('-r', '--risk_rule', type=str, required=True)

    parser_mod_risk_rule = subparsers.add_parser('modify_risk_rule', help='modify risk rule')
    parser_mod_risk_rule.set_defaults(func=modify_risk_rule)
    parser_mod_risk_rule.add_argument('-a', '--account_name', type=str, required=True)
    parser_mod_risk_rule.add_argument('-r', '--risk_rule', type=str, required=True)
    parser_mod_risk_rule.add_argument('-c', '--cmd', type=str, required=True)

    parser_query_strategy_params = subparsers.add_parser('query_strategy_params', help='query strategy conf')
    parser_query_strategy_params.set_defaults(func=query_strategy_params)
    parser_query_strategy_params.add_argument('-s', '--strategy', type=str, required=True, help='strategy name')

    parser_update_strategy_params = subparsers.add_parser('update_strategy_params', help='update strategy conf')
    parser_update_strategy_params.set_defaults(func=update_strategy_params)
    parser_update_strategy_params.add_argument('-s', '--strategy', type=str, required=True, help='strategy name')
    parser_update_strategy_params.add_argument('-c', '--conf', type=str, required=True, help='new conf')
    
    parser_send_cmd = subparsers.add_parser('send_cmd', help='send cmd to strategy')
    parser_send_cmd.set_defaults(func=send_cmd_to_strategy)
    parser_send_cmd.add_argument('-s', '--strategy', type=str, required=True, help='strategy name')
    parser_send_cmd.add_argument('-d', '--data', type=str, required=True, help='commmand data')

    parser_subscribe = subparsers.add_parser('subscribe_md', help='subscribe market data')
    parser_subscribe.set_defaults(func=subscribe_market_data)
    parser_subscribe.add_argument('-p', '--pattern', type=str, action='append', required=True, help='pattern')

    parser_subscribe = subparsers.add_parser('unsubscribe_md', help='unsubscribe market data')
    parser_subscribe.set_defaults(func=unsubscribe_market_data)
    parser_subscribe.add_argument('-p', '--pattern', type=str, action='append', required=True, help='pattern')

    args = parser.parse_args()
    global g_trader_port, g_trader_ip
    g_trader_port = args.port
    g_trader_ip = args.ip

    if hasattr(args, 'func'):
        args.func(args)
    else:
        parser.print_help()


def main():
    parse_args_and_run()


if __name__ == '__main__':
    main()
