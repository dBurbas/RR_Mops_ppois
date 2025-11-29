#include "utils.hpp"
#include "../settings/settings.hpp"

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

// std::string ContentGraph::GetResultAnswer(std::vector<std::vector<std::string>> const & resComponents)
// {
//   std::string resAnswer;
//   if (resComponents.size() == 1)
//   {
//     resAnswer = POSITIVE_ANSWER;
//   }
//   else
//   {
//     resAnswer = NEGATIVE_ANSWER;
//     for (int i = 0; i < resComponents.size(); i++)
//     {
//       auto header = "Компонента " + std::to_string(i + 1) + ":\n";
//       resAnswer += header;
//       for (auto const & distr : resComponents[i])
//       {
//         auto el = distr + "\n";
//         resAnswer += el;
//       }
//     }
//   }
//   return resAnswer;
// }