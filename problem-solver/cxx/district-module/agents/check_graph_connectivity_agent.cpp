#include "check_graph_connectivity_agent.hpp"
#include "../keynodes/graph_district_keynodes.hpp"
#include "../utils/utils.hpp"
#include "../settings/settings.hpp"
#include <string>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <utility>
#include <tuple>

CheckConnectivityAgent::CheckConnectivityAgent()
{
  m_logger =
      utils::ScLogger(utils::ScLogger::ScLogType::File, "logs/CheckConnectivityhAgent.log", utils::ScLogLevel::Debug);
}

ScAddr CheckConnectivityAgent::GetEventSubscriptionElement() const
{
  return GraphKeynodes::concept_ready_to_analyze_city_routes;
}

ScTemplate CheckConnectivityAgent::GetInitiationConditionTemplate(ConnectivityEvent const & event) const
{
  ScTemplate templ;
  templ.Triple(GraphKeynodes::concept_ready_to_analyze_city_routes, ScType::VarPermPosArc, ScType::VarNode);
  return templ;
}

ScAddr CheckConnectivityAgent::GetActionClass() const
{
  return GraphKeynodes::action_check_connectivity;
}

int CheckConnectivityAgent::GetCountOfEdges(ScAddr const & el)
{
  if (!el.IsValid())
  {
    return 0;
  }

  int count = 0;

  ScIterator3Ptr it1 = m_context.CreateIterator3(el, ScType::ConstCommonEdge, ScType::ConstNode);
  while (it1->Next())
  {
    count++;
  }

  // ScIterator3Ptr it2 = m_context.CreateIterator3(ScType::ConstNode, ScType::ConstCommonEdge, el);
  // while (it2->Next())
  // {
  //   count++;
  // }

  return count;
}

ScAddr CheckConnectivityAgent::GetElementByIterator(ScAddr const & el, int index)
{
  if (!el.IsValid())
  {
    return ScAddr();
  }

  int count = 0;

  ScIterator3Ptr it1 = m_context.CreateIterator3(el, ScType::ConstCommonEdge, ScType::ConstNode);
  while (it1->Next())
  {
    if (count == index)
    {
      ScAddr result = it1->Get(2);
      return result.IsValid() ? result : ScAddr();
    }
    count++;
  }

  // ScIterator3Ptr it2 = m_context.CreateIterator3(ScType::ConstNode, ScType::ConstCommonEdge, el);
  // while (it2->Next())
  // {
  //   if (count == index)
  //   {
  //     ScAddr result = it2->Get(0);
  //     return result.IsValid() ? result : ScAddr();
  //   }
  //   count++;
  // }

  return ScAddr();
}

ScAddrUnorderedSet CheckConnectivityAgent::GetDistricts(ScAddr const & city)
{
  m_logger.Info("Try to create iterator");
  ScIterator3Ptr const it3 = m_context.CreateIterator3(city, ScType::ConstPermPosArc, ScType::ConstNodeStructure);
  m_logger.Info("Success create iterator");

  ScAddrVector routes;
  m_logger.Info("Try to add all routes route to elements");
  while (it3->Next())
  {
    ScAddr const & elementAddr = it3->Get(2);
    std::string const & nameRoute = m_context.GetElementSystemIdentifier(elementAddr);
    m_logger.Debug(nameRoute);
    routes.push_back(elementAddr);
  }
  m_logger.Info("All routes were successfully added to elements");

  m_logger.Info("Try to get all unique districts from all routes");
  ScAddrUnorderedSet districts;
  for (auto const & route : routes)
  {
    ScIterator3Ptr const itRoutes = m_context.CreateIterator3(route, ScType::ConstPermPosArc, ScType::ConstNode);
    while (itRoutes->Next())
    {
      ScAddr const & district = itRoutes->Get(2);
      std::string const & nameDistrict = m_context.GetElementSystemIdentifier(district);
      if (nameDistrict.find(BASE_NAME_OF_NODES) != std::string::npos)
      {
        districts.insert(district);
      }
    }
  }
  m_logger.Info("Successful adding all unique districts from all routes");

  return districts;
}

std::string CheckConnectivityAgent::GetMainIndentifier(std::string const & district)
{
  auto const & dist = m_context.SearchElementBySystemIdentifier(district);
  std::string resMainIdtfDistrict;
  ScIterator5Ptr const it = m_context.CreateIterator5(
      dist, ScType::ConstCommonArc, ScType::NodeLink, ScType::ConstPermPosArc, ScKeynodes::nrel_main_idtf);
  it->Next();
  auto const & linkMainIdtf = it->Get(2);
  if (linkMainIdtf.IsValid())
  {
    m_context.GetLinkContent(linkMainIdtf, resMainIdtfDistrict);
  }
  return resMainIdtfDistrict;
}

ScAddrUnorderedSet CheckConnectivityAgent::DFSConnection(
    ScAddrUnorderedSet & districts,
    ScAddr const & startDistrict,
    ScAddrUnorderedSet & visitedDistricts)
{
  ScAddrUnorderedSet elComponents;
  ScAddrStack stack;
  stack.push(startDistrict);
  visitedDistricts.insert(startDistrict);
  elComponents.insert(startDistrict);
  m_logger.Info("We start dfs and  try check connectivity");
  while (!stack.empty())
  {
    auto const & currDistrict = stack.top();
    m_logger.Info("Current vertex - " + std::string(m_context.GetElementSystemIdentifier(currDistrict)));
    stack.pop();
    ScIterator3Ptr const itNeighbourDistrict =
        m_context.CreateIterator3(currDistrict, ScType::ConstCommonEdge, ScType::ConstNode);
    while (itNeighbourDistrict->Next())
    {
      ScAddr const & neighDistrict = itNeighbourDistrict->Get(2);
      m_logger.Debug("Neighboor vertex - " + std::string(m_context.GetElementSystemIdentifier(neighDistrict)));
      auto const & it = visitedDistricts.find(neighDistrict);
      if (it == visitedDistricts.end())
      {
        stack.push(neighDistrict);
        visitedDistricts.insert(neighDistrict);
        elComponents.insert(neighDistrict);
      }
    }
  }
  m_logger.Info("We finish dfs and check connectivity");
  return elComponents;
}

void CheckConnectivityAgent::DFSBridges(
    ScAddrUnorderedSet & districts,
    std::vector<std::pair<std::string, std::string>> & bridges)
{
  m_logger.Debug("DFSBridges start");
  ScAddrUnorderedSet visitedDistricts;
  std::map<std::string, int> tin;
  std::map<std::string, int> low;
  std::map<std::string, int> nextIndex;
  std::map<std::string, std::string> parent;
  int currentVal = 0;

  for (auto const & startDistrict : districts)
  {
    if (!startDistrict.IsValid() || visitedDistricts.find(startDistrict) != visitedDistricts.end())
    {
      continue;
    }

    std::string startName;
    startName = m_context.GetElementSystemIdentifier(startDistrict);

    ScAddrStack stack;
    stack.push(startDistrict);
    visitedDistricts.insert(startDistrict);

    tin[startName] = currentVal;
    low[startName] = currentVal;
    nextIndex[startName] = 0;
    parent[startName] = "";
    currentVal++;

    while (!stack.empty())
    {
      ScAddr currDistrict = stack.top();
      if (!currDistrict.IsValid())
      {
        stack.pop();
        continue;
      }
      std::string currName;
      currName = m_context.GetElementSystemIdentifier(currDistrict);

      if (nextIndex[currName] < GetCountOfEdges(currDistrict))
      {
        int index = nextIndex[currName];
        ScAddr neighbor = GetElementByIterator(currDistrict, index);
        nextIndex[currName]++;

        if (!neighbor.IsValid())
        {
          continue;
        }

        std::string neighborName;
        neighborName = m_context.GetElementSystemIdentifier(neighbor);

        if (parent[currName] == neighborName)
        {
          continue;
        }

        if (visitedDistricts.find(neighbor) == visitedDistricts.end())
        {
          visitedDistricts.insert(neighbor);
          tin[neighborName] = currentVal;
          low[neighborName] = currentVal;
          nextIndex[neighborName] = 0;
          parent[neighborName] = currName;
          currentVal++;
          stack.push(neighbor);
        }
        else
        {
          low[currName] = std::min(low[currName], tin[neighborName]);
        }
      }
      else
      {
        stack.pop();
        std::string parentName = parent[currName];
        if (!parentName.empty())
        {
          low[parentName] = std::min(low[parentName], low[currName]);
          if (low[currName] > tin[parentName])
          {
            bridges.push_back({parentName, currName});
            m_logger.Debug("Found bridge: " + parentName + " - " + currName);
          }
        }
      }
    }
  }
  m_logger.Debug("DFSBridges finish. Found " + std::to_string(bridges.size()) + " bridges");
}

void CheckConnectivityAgent::GetResultOfConnectivity(
    std::vector<std::vector<std::string>> & resComponents,
    ScAddrUnorderedSet & districts,
    ScAddrUnorderedSet & visitedDistricts)
{
  for (auto const & districtIt : districts)
  {
    auto checkDistrict = visitedDistricts.find(districtIt);
    if (checkDistrict == visitedDistricts.end())
    {
      auto resultOfDfs = DFSConnection(districts, districtIt, visitedDistricts);
      std::vector<std::string> nameDistricts;
      for (auto const & distr : resultOfDfs)
      {
        std::string const & nameDistr = m_context.GetElementSystemIdentifier(distr);
        nameDistricts.push_back(nameDistr);
      }
      resComponents.push_back(nameDistricts);
    }
  }
}

ScResult CheckConnectivityAgent::DoProgram(ConnectivityEvent const & event, ScAction & action)
{
  m_logger.Info("Agent start to check connectivity");
  ScAddr const & city = event.GetArcTargetElement();
  ScAddrUnorderedSet districts = GetDistricts(city);
  ScAddrUnorderedSet visitedDistricts;
  ScAddrUnorderedSet resDistricts;
  std::vector<std::vector<std::string>> resComponents;

  m_logger.Info("Check what districts are stay alone");
  GetResultOfConnectivity(resComponents, districts, visitedDistricts);
  m_logger.Info("Finish checking what districts are stay alone");

  m_logger.Info("Generate link and set answer");
  ScAddr const & linkResultConnectivity = m_context.GenerateLink(ScType::ConstNodeLink);
  auto const & resAnswer = ContentGraph::GetResultAnswer(resComponents);
  m_context.SetLinkContent(linkResultConnectivity, resAnswer);
  m_logger.Info("Successfull generate link and set answer");

  m_logger.Info("Finish checking components");
  ScAddr const & resArcOfConnectivity =
      m_context.GenerateConnector(ScType::ConstCommonArc, city, linkResultConnectivity);
  m_logger.Info("Link was successful create and connect to city");

  m_logger.Info("Try to create nrel result of connectivity");
  ScAddr const & nameCityArcNrelConnectivity = m_context.GenerateConnector(
      ScType::ConstPermPosArc, GraphKeynodes::nrel_result_connectivity, resArcOfConnectivity);
  m_logger.Info("Succesfully create nrel result of connectivity");

  std::vector<std::pair<std::string, std::string>> bridges;
  std::string resultAnswerBridge;
  DFSBridges(districts, bridges);
  ScAddr const & linkResultBridges = m_context.GenerateLink(ScType::ConstNodeLink);
  if (bridges.empty())
  {
    resultAnswerBridge = NEGATIVE_BRIDGE_ANSWER;
  }
  else
  {
    m_logger.Debug(std::to_string(bridges.size()));
    for (int i = 0; i < bridges.size(); i++)
    {
      auto it = bridges[i];
      auto const & firstDistrict = GetMainIndentifier(it.first);
      auto const & secondDistrict = GetMainIndentifier(it.second);
      resultAnswerBridge += firstDistrict;
      resultAnswerBridge += " ======= ";
      resultAnswerBridge += secondDistrict;
      resultAnswerBridge += "\n";
    }
  }
  m_context.SetLinkContent(linkResultBridges, resultAnswerBridge);

  ScAddr const & resArcOfBridges = m_context.GenerateConnector(ScType::ConstCommonArc, city, linkResultBridges);

  ScAddr const & nameCityArcNrelBridge =
      m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::nrel_result_bridges, resArcOfBridges);
  m_logger.Info("All is correct");

  return action.FinishSuccessfully();
}