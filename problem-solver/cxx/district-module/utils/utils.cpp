#include "utils.hpp"
#include "../settings/settings.hpp"

int OperationsWithMap::GetNumberMin(std::map<std::string, int> const & map_1, std::string const & str)
{
  auto it = map_1.find(str);
  if (it == map_1.end())
  {
    return -1;
  }
  return it->second;
}

int OperationsWithMap::GetNumberMax(std::map<std::string, int> const & map_1, std::string const & str)
{
  auto it = map_1.find(str);
  if (it == map_1.end())
  {
    return INF;
  }
  return it->second;
}