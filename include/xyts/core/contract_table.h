#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "xydata/sessions.h"
#include "xyts/core/trade_msg.h"
#include "xyu/datetime.h"
#include "xyu/utility.h"

namespace xyts {

// 合约类
// 所有可交易的东西我们统称为合约，包括期货、期权、股票、外汇等
// 该合约类中存储了某只期货合约、某只期权合约、某只股票或是某种可交易物品的基本信息
// 这些信息主要是当日的静态信息，可在盘前通过数据库或是交易API查询到
struct Contract {
  // 内部使用的整型合约ID，从0开始连续递增
  ContractId contract_id;
  // 内部使用的整型交易所ID
  ExchangeId exchange_id;
  // 内部使用的合约命名，如FUT_CCFX_IF:202112, OPT_XSHG_510300:202112:C:3.2
  // 命名规则
  // 对于期货：FUT_{交易所代码}_{品种}-{YYYYMM格式的到期日}
  // 对于期权： OPT_{交易所代码}_{品种}-{YYYYMM格式的到期日}-{看涨或看跌,C/P}-{行权价}
  // 对于A股：{6位代码}.{交易所}，如600001.XSHG, 300001.XSHE
  std::string instr;
  // 该合约在交易所的命名，如IF2112, 10003325, 600001
  std::string code;
  // 交易所命名，和交易所的规范一致，如CFFEX, SHFE等
  std::string exchange;
  // FUT_{交易所代码}_{品种}
  std::string symbol;
  // 该合约的类型，期货/期权/股票/...
  ProductType product_type;
  // 合约乘数
  double contract_unit;
  // 最小价格变动单位
  double price_tick;
  // 当日涨停价
  double upper_limit_price;
  // 当日跌停价
  double lower_limit_price;
  // 多头保证金率，对于股票这类没有杠杆的应该设为1.0
  double long_margin_rate;
  // 空头保证金率
  double short_margin_rate;
  // 单次限价单的最大数量
  Volume max_limit_order_volume;
  // 单次限价单的最小数量
  Volume min_limit_order_volume;
  // 单次市价单的最大数量
  Volume max_market_order_volume;
  // 单次市价单的最小数量
  Volume min_market_order_volume;
  // 上市日
  std::string list_date;
  // 到期日
  std::string expire_date;

  xydata::SessionsPtr sessions = nullptr;

  // ----- 衍生品独有属性 -----
  // 标的的合约类型
  ProductType underlying_type;
  // 标的的内部命名
  std::string underlying_symbol;

  // ----- 期权独有属性 -----
  // 行权日
  std::string exercise_date;
  // 行权价
  double exercise_price;
};

using ContractPtr = const Contract*;

class ContractTable : public xyu::NonCopyableNonMoveable {
 public:
  static void LoadFromArray(std::vector<Contract>&& vec);

  static void LoadFromFile(const std::string& file);

  static void LoadFromFiles(const std::vector<std::string>& files);

  static std::size_t size() { return Instance().contracts_.size(); }

  static ContractPtr GetByInstrument(const std::string& instr) {
    auto& instr2contract = Instance().instr2contract_;
    if (auto it = instr2contract.find(instr); it != instr2contract.end()) {
      return it->second;
    } else {
      return nullptr;
    }
  }

  // 对于多个交易所中存在同一个code的情况，应该转成instr后再通过GetByInstrument获取。
  // 这里提供这个接口主要是为了方便国内的交易，且该接口应该只用于trade_api和data_feed
  // 的实现中
  static ContractPtr GetByCode(const std::string& code) {
    auto& code2contract = Instance().code2contract_;
    if (auto it = code2contract.find(code); it != code2contract.end()) {
      return it->second;
    } else {
      return nullptr;
    }
  }

  // 该接口效率不高，一般只在初识化时使用
  // pattern: 用于匹配instr的正则表达式(C++ regex)，如OPT_XSHG_510050.+?
  static std::vector<ContractPtr> GetByPattern(const std::string& pattern);

  static std::vector<ContractPtr> GetByPatterns(const std::vector<std::string>& patterns);

  static ContractPtr GetById(ContractId contract_id) {
    auto& contracts = Instance().contracts_;
    if (contract_id >= contracts.size()) {
      return nullptr;
    }
    return &contracts[contract_id];
  }

  static ContractPtr At(ContractId contract_id) {
    auto& contracts = Instance().contracts_;
    return &contracts.at(contract_id);
  }

  static const std::vector<Contract>& GetAllContracts() { return Instance().contracts_; }

 private:
  static ContractTable& Instance() {
    static ContractTable ct;
    return ct;
  }

  std::vector<Contract> contracts_;
  std::unordered_map<std::string, Contract*> instr2contract_;
  std::unordered_map<std::string, Contract*> code2contract_;
};

void LoadContractsFromCsv(const std::filesystem::path& file, std::vector<Contract>* contracts);

void StoreContractsToCsv(const std::filesystem::path& file, const std::vector<Contract>& contracts);

}  // namespace xyts
