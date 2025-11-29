#pragma once

#include <sc-memory/sc_memory.hpp>
#include <map>
#include <string>

class OperationsWithUnorderedSet
{
public:
  static ScAddrUnorderedSet FindElemNotInIntersection(
      ScAddrUnorderedSet const & set_1,
      ScAddrUnorderedSet const & set_2);
};

class OperationsWithMap
{
public:
  static int GetNumberMin(std::map<std::string, int> const & map_1, std::string const & str);
  static int GetNumberMax(std::map<std::string, int> const & map_1, std::string const & str);
};

// class ContentGraph
// {
// public:
//   static std::string GetResultAnswer(std::vector<std::vector<std::string>> const & resComponents);
// };