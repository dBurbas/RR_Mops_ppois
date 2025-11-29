#include "bfs_agent.hpp"
#include "../keynodes/graph_district_keynodes.hpp"
#include "../utils/utils.hpp"
#include "../settings/settings.hpp"
#include <vector>
#include <utility>
#include <map>
#include <string>
#include <unordered_set>

TransportNetBFSEvent::TransportNetBFSEvent()
{
  m_logger = utils::ScLogger(utils::ScLogger::ScLogType::File, "logs/BFSAgent.log", utils::ScLogLevel::Debug);
}

ScAddr TransportNetBFSEvent::GetEventSubscriptionElement() const
{
  return GraphKeynodes::concept_ready_to_analyze_city_routes;
}

ScTemplate TransportNetBFSEvent::GetInitiationConditionTemplate(WaysEvent const & event) const
{
  ScTemplate templ;
  templ.Triple(GraphKeynodes::concept_ready_to_analyze_city_routes, ScType::VarPermPosArc, ScType::VarNode);
  return templ;
}

ScAddr TransportNetBFSEvent::GetActionClass() const
{
  return GraphKeynodes::action_transport_net_bfs;
}

ScAddrUnorderedSet TransportNetBFSEvent::GetDistricts(ScAddr const & city)
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

void TransportNetBFSEvent::FindShortestWays(
    ScAddrUnorderedSet const & districts,
    ScAddr const & startVertex,
    std::map<std::string, int> & resDists)
{
  ScAddrQueue queue;
  ScAddrUnorderedSet visited;
  std::string nameStartVertex = m_context.GetElementSystemIdentifier(startVertex);
  resDists[nameStartVertex] = 0;
  queue.push(startVertex);

  while (!queue.empty())
  {
    ScAddr vertex = queue.front();
    std::string nameVertex = m_context.GetElementSystemIdentifier(vertex);
    queue.pop();

    ScIterator3Ptr const it = m_context.CreateIterator3(vertex, ScType::ConstCommonEdge, ScType::ConstNode);
    while (it->Next())
    {
      ScAddr toVertex = it->Get(2);
      std::string nameToVertex = m_context.GetElementSystemIdentifier(toVertex);
      int const vDist = OperationsWithMap::GetNumberMax(resDists, nameVertex);
      int const toDist = OperationsWithMap::GetNumberMax(resDists, nameToVertex);
      if (toDist > vDist + 1)
      {
        visited.insert(toVertex);
        resDists[nameToVertex] = vDist + 1;
        queue.push(toVertex);
      }
    }
  }
  for (auto const & district : districts)
  {
    if (visited.find(district) == visited.end())
    {
      std::string nameUnreachDistr = m_context.GetElementSystemIdentifier(district);
      resDists[nameUnreachDistr] = INF;
    }
  }
}

void TransportNetBFSEvent::SearchShortestWaysInCity(ScAddrUnorderedSet const & districts, ScAddr & city)
{
  ScStructure shortestDists = m_context.GenerateStructure();
  std::vector<std::pair<std::string, std::string>> createdPairs;
  for (auto const & district : districts)
  {
    std::string nameDist = m_context.GetElementSystemIdentifier(district);
    std::map<std::string, int> resDists;
    FindShortestWays(districts, district, resDists);
    for (auto const & [nameToDistr, value] : resDists)
    {
      ScAddr toDistr = m_context.SearchElementBySystemIdentifier(nameToDistr);
      auto pair_1 = std::make_pair(nameDist, nameToDistr);
      auto pair_2 = std::make_pair(nameDist, nameToDistr);
      if (find(createdPairs.begin(), createdPairs.end(), pair_1) != createdPairs.end()
          or find(createdPairs.begin(), createdPairs.end(), pair_2) != createdPairs.end())
      {
        continue;
      }

      if (value == INF)
      {
        continue;
      }

      ScStructure way = m_context.GenerateStructure();
      way << district << toDistr;

      createdPairs.push_back(std::make_pair(nameDist, nameToDistr));

      auto wayLink = m_context.GenerateLink(ScType::ConstNodeLink);
      m_context.SetLinkContent(wayLink, std::to_string(value));

      auto connectorLinkStruct = m_context.GenerateConnector(ScType::ConstCommonArc, way, wayLink);
      auto connectorLinkValue =
          m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::nrel_length_way, connectorLinkStruct);
      auto connectorClassShortestWay =
          m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::concept_shortest_way, way);
      auto connectorCity = m_context.GenerateConnector(ScType::ConstPermPosArc, city, way);
      auto connectorRrelShortWay =
          m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::rrel_shortest_way, connectorCity);
    }
  }
}

ScResult TransportNetBFSEvent::DoProgram(WaysEvent const & event, ScAction & action)
{
  m_logger.Info("Agent start finding shortest ways");
  ScAddr city = event.GetArcTargetElement();
  ScAddrUnorderedSet districts = GetDistricts(city);
  SearchShortestWaysInCity(districts, city);
  m_logger.Info("Agent finish finding shortest ways");
  return action.FinishSuccessfully();
}