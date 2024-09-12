#pragma once

#include <string>

namespace xyts::fut {

std::string ExtractSymbol(std::string_view instrument);

std::string ExtractXySymbol(std::string_view instrument);

std::string GetXySymbol(std::string_view symbol, std::string_view exchange);

std::string GetXyInstrument(std::string_view instrument, std::string_view exchange, int year = 0);

// return {instrument, exchange}
std::pair<std::string, std::string> SplitXyInstrument(std::string_view instrument);

}  // namespace xyts::fut

namespace xyts::opt {

std::string GetXyInstrument(std::string_view instrument, std::string_view exchange, int year = 0);

}  // namespace xyts::opt
