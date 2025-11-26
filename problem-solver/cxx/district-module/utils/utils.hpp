#pragma once

#include <sc-memory/sc_memory.hpp>

class OperationsWithUnorderedSet
{
public:
  static ScAddrUnorderedSet FindElemNotInIntersection(
      ScAddrUnorderedSet const & set_1,
      ScAddrUnorderedSet const & set_2);
};