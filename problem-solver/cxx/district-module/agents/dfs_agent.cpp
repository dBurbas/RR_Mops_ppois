#include "dfs_agent.hpp"
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

TransportNetDFSAgent::TransportNetDFSAgent()
{
  m_logger = utils::ScLogger(utils::ScLogger::ScLogType::File, "logs/DFSAgent.log", utils::ScLogLevel::Debug);
}

ScAddr TransportNetDFSAgent::GetEventSubscriptionElement() const
{
  return GraphKeynodes::action_ready_to_analyze_city_routes;
}

ScTemplate TransportNetDFSAgent::GetInitiationConditionTemplate(ConnectivityEvent const & event) const
{
  ScTemplate templ;
  templ.Triple(GraphKeynodes::action_ready_to_analyze_city_routes, ScType::VarPermPosArc, ScType::VarNode);
  return templ;
}

ScAddr TransportNetDFSAgent::GetActionClass() const
{
  return GraphKeynodes::action_check_connectivity;
}

int TransportNetDFSAgent::GetCountOfEdges(ScAddr const & el)
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

  return count;
}

ScAddr TransportNetDFSAgent::GetElementByIterator(ScAddr const & el, int index)
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

  return ScAddr();
}

ScAddrUnorderedSet TransportNetDFSAgent::GetDistricts(ScAddr const & city)
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

std::string TransportNetDFSAgent::GetMainIndentifier(std::string const & district)
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

ScAddrUnorderedSet TransportNetDFSAgent::FindConnection(
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

void TransportNetDFSAgent::FindBridges(
    ScAddrUnorderedSet & districts,
    std::vector<std::pair<std::string, std::string>> & bridges,
    std::vector<std::pair<ScAddr, ScAddr>> & bridgesAddr)
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
            auto parentNode = m_context.SearchElementBySystemIdentifier(parentName);
            auto currentNode = m_context.SearchElementBySystemIdentifier(currName);
            bridgesAddr.push_back({parentNode, currentNode});
            m_logger.Debug("Found bridge: " + parentName + " - " + currName);
          }
        }
      }
    }
  }
  m_logger.Debug("DFSBridges finish. Found " + std::to_string(bridges.size()) + " bridges");
}

void TransportNetDFSAgent::GetResultOfConnectivity(
    std::vector<ScAddrUnorderedSet> & resComponents,
    ScAddrUnorderedSet & districts,
    ScAddrUnorderedSet & visitedDistricts)
{
  for (auto const & districtIt : districts)
  {
    auto checkDistrict = visitedDistricts.find(districtIt);
    if (checkDistrict == visitedDistricts.end())
    {
      auto resultOfDfs = FindConnection(districts, districtIt, visitedDistricts);
      resComponents.push_back(resultOfDfs);
    }
  }
}

void TransportNetDFSAgent::DefineTypeOfGraph(std::vector<ScAddrUnorderedSet> & resComponents, ScAddr & city)
{
  if (resComponents.size() == 1)
  {
    m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::concept_connect_graph, city);
  }
  else
  {
    m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::concept_noconnect_graph, city);
  }
}

void TransportNetDFSAgent::GetComponents(
    std::vector<ScAddrUnorderedSet> & resComponents,
    ScAddr & city,
    ScAddr & nodeTuple)
{
  for (auto const & listDistr : resComponents)
  {
    ScStructure component = m_context.GenerateStructure();
    ScAddrUnorderedSet listRoads;
    for (auto const & distr : listDistr)
    {
      ScIterator3Ptr const it = m_context.CreateIterator3(distr, ScType::ConstCommonEdge, ScType::ConstNode);
      while (it->Next())
      {
        auto road = it->Get(1);
        auto checkEdge = listRoads.find(road);
        if (checkEdge == listRoads.end())
        {
          component << road << distr;
          listRoads.insert(road);
        }
      }
    }
    auto componentConnector = m_context.GenerateConnector(ScType::ConstCommonArc, city, component);
    auto nrelConnectivityComponent = m_context.GenerateConnector(
        ScType::ConstPermPosArc, GraphKeynodes::nrel_connectivity_component, componentConnector);
    m_logger.Info("Try to create graph");

    m_logger.Info("Try to make component rrel subgraph");
    auto connectorComponentGraph = m_context.GenerateConnector(ScType::ConstPermPosArc, nodeTuple, component);
    auto rrelSubgraphComponent =
        m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::rrel_subgraph, connectorComponentGraph);
    m_logger.Info("Finish make component rrel subgraph");
  }
}

void TransportNetDFSAgent::GetBridges(
    std::vector<std::pair<ScAddr, ScAddr>> & bridgesAddr,
    ScAddr & nodeTuple,
    ScAddr & city)
{
  for (int i = 0; i < bridgesAddr.size(); i++)
  {
    auto const & route = bridgesAddr[i];
    ScIterator3Ptr const it = m_context.CreateIterator3(route.first, ScType::ConstCommonEdge, route.second);
    it->Next();

    ScIterator3Ptr const it2 =
        m_context.CreateIterator3(ScType::ConstNodeStructure, ScType::ConstPermPosArc, it->Get(1));
    ScAddr bridge;
    while (it2->Next())
    {
      auto stIsTown = it2->Get(0);
      ScIterator3Ptr const it3 = m_context.CreateIterator3(city, ScType::ConstPermPosArc, stIsTown);
      if (it3->Next())
      {
        bridge = stIsTown;
      }
    }
    ScAddr const & arcBridge = m_context.GenerateConnector(ScType::ConstPermPosArc, nodeTuple, bridge);
    ScAddr const & connectorRrelBridge =
        m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::rrel_bridge, arcBridge);
  }
}

ScResult TransportNetDFSAgent::DoProgram(ConnectivityEvent const & event, ScAction & action)
{
  m_logger.Info("Agent start to check connectivity");
  ScAddr city = event.GetArcTargetElement();
  ScAddrUnorderedSet districts = GetDistricts(city);
  ScAddrUnorderedSet visitedDistricts;
  ScAddrUnorderedSet resDistricts;
  std::vector<ScAddrUnorderedSet> resComponents;
  ScAddr nodeTuple = m_context.GenerateNode(ScType::ConstNodeTuple);

  m_logger.Info("Try to connect nrel_bridge to node_tuple");
  ScAddr const & arcNrelBridge =
      m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::nrel_bridge, nodeTuple);
  m_logger.Info("Finish connect nrel_bridge to node_tuple");

  m_logger.Info("Try to make city rrel graph");
  auto connectorCityGraph = m_context.GenerateConnector(ScType::ConstPermPosArc, nodeTuple, city);
  auto rrelGraphCity =
      m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::rrel_graph, connectorCityGraph);
  m_logger.Info("Finish make city rrel graph");

  m_logger.Info("Check what districts are stay alone");
  GetResultOfConnectivity(resComponents, districts, visitedDistricts);
  m_logger.Info("Finish checking what districts are stay alone");

  m_logger.Info("Try to define type of graph");
  DefineTypeOfGraph(resComponents, city);
  m_logger.Info("Type of graph is successfully defined");

  m_logger.Info("Start finding connectivity result");
  GetComponents(resComponents, city, nodeTuple);
  m_logger.Info("Finish finding connectivity result");

  m_logger.Info("Try to find bridge");
  std::vector<std::pair<std::string, std::string>> bridges;
  std::vector<std::pair<ScAddr, ScAddr>> bridgesAddr;
  std::string resultAnswerBridge;

  m_logger.Info("DFSBridges start working");
  FindBridges(districts, bridges, bridgesAddr);
  m_logger.Info("DFSBridges finish working");

  m_logger.Info("DFSBridges start searching bridges");
  GetBridges(bridgesAddr, nodeTuple, city);
  m_logger.Info("DFSBridges finish searching bridges");
  return action.FinishSuccessfully();
}