#pragma once

#include <sc-memory/sc_memory.hpp>
#include <map>
#include <string>

class OperationsWithMap
{
public:
  static int GetNumberMin(std::map<std::string, int> const & map_1, std::string const & str);
  static int GetNumberMax(std::map<std::string, int> const & map_1, std::string const & str);
};