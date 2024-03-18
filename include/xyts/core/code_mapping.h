#pragma once

#include <string>

namespace xyts::fut {

std::string ExtractSymbol(const std::string_view instrument);

std::string ExtractXySymbol(const std::string_view instrument);

std::string GetXySymbol(const std::string_view symbol, const std::string_view exchange);

std::string GetXyInstrument(const std::string_view instrument, const std::string_view exchange,
                            int year = 0);

// return {instrument, exchange}
std::pair<std::string, std::string> SplitXyInstrument(const std::string_view instrument);

}  // namespace xyts::fut

namespace xyts::opt {

std::string GetXyInstrument(const std::string_view instrument, const std::string_view exchange,
                            int year = 0);

}  // namespace xyts::opt
