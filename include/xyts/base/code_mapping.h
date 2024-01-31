#pragma once

#include <string>

namespace xyts::fut {

std::string ExtractSymbol(const std::string& instrument);

std::string ExtractXySymbol(const std::string& instrument);

std::string GetXySymbol(const std::string& symbol, const std::string& exchange);

std::string GetXyInstrument(const std::string& instrument, const std::string& exchange,
                            int year = 0);

// return {instrument, exchange}
std::pair<std::string, std::string> SplitXyInstrument(const std::string& instrument);

}  // namespace xyts::fut
