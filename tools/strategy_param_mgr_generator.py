import sys
import json
import re
import os


h_head_fmt = '''// This file is auto-generated from {json_file}. DO NOT EDIT!
#pragma once

#include <string>
#include <vector>

#include "xyts/strategy/strategy_param_manager.h"

class {cls_name} final : public ::xyts::strategy::StrategyParamManager {{'''

get_func_fmt = '''  const {func_type} get_{name}() const {{ return {name}; }}'''

set_func_fmt = '''  void set_{name}(const {func_type} _{name}) {{
    {name} = _{name};
    json_params_["{name}"]["value"] = _{name};
  }}'''

nameof_func_fmt = '''  const char* nameof_{name}() const {{ return "{name}"; }}'''

update_member_fmt = '''    if (ContainsParam(update_json, "{name}")) {{
      auto value = GetParamValue<{type}>(update_json, "{name}");
      set_{name}(value);
    }}'''

init_func_head = '''  explicit {cls_name}(const std::filesystem::path& param_path) : StrategyParamManager(param_path) {{'''
init_func_tail = '''  }
'''

update_func_head = '''  void Update(const nlohmann::json& update_json) final {'''
update_func_tail = '''  }
'''

check_func = '''  template <class T>
    T CheckAndGetParamValue(const std::string& param_name) {
    if (!ContainsParam(json_params_, param_name)) {
      throw std::runtime_error("Parameter '" + param_name + "' does not exist");
    }
    return GetParamValue<T>(json_params_, param_name);
  }

  static bool ContainsParam(const nlohmann::json& json, const std::string& param_name) {
    return json.contains(param_name) && json[param_name].contains("value");
  }

  template <class T>
  static T GetParamValue(const nlohmann::json& json, const std::string& param_name) {
    return json[param_name]["value"].get<T>();
  }
'''

def gen_conf_struct_file(conf, output_dir):
    obj = json.load(open(conf))
    basename = os.path.basename(conf)
    tmp = basename.replace('.json', '')
    its = [ i[0].upper() + i[1:] for i in re.split("[\.|_]", tmp) ] + ["ParamManager"]
    cls_name = "".join(its)
    filename = basename.replace('.json', '').replace('.', '_') + "_param_manager.h"
    for name,attr in obj.items():
        if attr["type"] in ["uint", "vector<uint>"]:
            attr["type"] = attr["type"].replace('uint', 'uint32_t')
        elif attr["type"] in ["float", "vector<float>"]:
            attr["type"] = attr["type"].replace('float', 'double')
        elif attr["type"] in ["uint64", "vector<uint64>"]:
            attr["type"] = attr["type"].replace('uint64', 'uint64_t')
        attr["type"] = attr["type"].replace('string', 'std::string').replace('vector', 'std::vector')
        assert attr["type"] in ["bool", "int", "uint32_t", 'uint64_t', "double", "std::string", "std::vector<bool>", "std::vector<int>", "std::vector<uint32_t>", "std::vector<double>", "std::vector<std::string>"]
    class_members = []
    class_funcs = [' public:']
    getter_setter_funcs = []
    init_func = [init_func_head.format(cls_name=cls_name)]
    update_func = [update_func_head]
    for name,attr in obj.items():
        is_number = attr["type"] in ["bool", "int", "uint32_t", "uint64_t", "double"]
        args = {
            'type': attr["type"],
            'name': name,
            'func_type': attr["type"] + ('' if is_number else "&" )
        }
        init_func.append('    {name} = CheckAndGetParamValue<{type}>("{name}");'.format_map(args))
        update_func.append(update_member_fmt.format_map(args))
        if is_number:
            class_members.append("  {type} {name} = 0;".format_map(args))
        else:
            class_members.append("  {type} {name};".format_map(args))
        getter_setter_funcs.append(nameof_func_fmt.format_map(args))
        getter_setter_funcs.append(get_func_fmt.format_map(args))
        getter_setter_funcs.append(set_func_fmt.format_map(args) + '\n')
    init_func.append(init_func_tail)
    update_func.append(update_func_tail)
    class_funcs += init_func
    class_funcs.extend(getter_setter_funcs)
    class_funcs += update_func
    class_funcs.append(' private:')
    class_funcs.append(check_func)
    class_defs = [h_head_fmt.format(cls_name=cls_name, json_file=os.path.basename(conf))] +class_funcs + class_members + ["};"]
    class_defs.append('\n')
    print("gen file", filename)
    new_file_data = "\n".join(class_defs)
    old_file_data = ""
    path = os.path.join(output_dir, filename)
    if os.path.exists(path):
        old_file_data = open(path).read()
    if new_file_data == old_file_data:
        print("file not changed. done")
        return 
    print("new file done")
    with open(path, "w") as fp:
        fp.write(new_file_data)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("%s <json_conf_file> [output_dir]" % sys.argv[0])
        exit(0)
    input_file = sys.argv[1]
    output_dir = '.'
    if len(sys.argv) >= 3:
        output_dir = sys.argv[2]
    gen_conf_struct_file(input_file, output_dir)
