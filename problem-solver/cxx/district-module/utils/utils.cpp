#include "utils.hpp"

ScAddrUnorderedSet OperationsWithUnorderedSet::FindElemNotInIntersection(
    ScAddrUnorderedSet const & set_1,
    ScAddrUnorderedSet const & set_2)
{
  ScAddrUnorderedSet result;
  for (auto const & el : set_1)
  {
    auto const & it = set_2.find(el);
    if (it == set_2.end())
    {
      result.insert(el);
    }
  }
  return result;
}