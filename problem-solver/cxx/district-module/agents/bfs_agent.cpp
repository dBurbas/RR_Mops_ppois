#include <queue>
#include <algorithm>
#include <vector>
#include <utility>
#include <map>
#include <string>
#include <unordered_set>
#include "bfs_agent.hpp"
#include "../keynodes/graph_district_keynodes.hpp"
#include "../settings/settings.hpp"
#include "../utils/string_formatter.hpp"
#include "../utils/utils.hpp"

TransportNetBFSAgent::TransportNetBFSAgent()
{
  m_logger = utils::ScLogger(utils::ScLogger::ScLogType::File, "logs/BFSAgent.log", utils::ScLogLevel::Debug);
}

ScAddr TransportNetBFSAgent::GetEventSubscriptionElement() const
{
  return GraphKeynodes::action_ready_to_analyze_city_routes;
}

ScTemplate TransportNetBFSAgent::GetInitiationConditionTemplate(TransportNetBFSEvent const & event) const
{
  ScTemplate templ;
  templ.Triple(GraphKeynodes::action_ready_to_analyze_city_routes, ScType::VarPermPosArc, ScType::VarNode);
  return templ;
}

ScAddr TransportNetBFSAgent::GetActionClass() const
{
  return GraphKeynodes::action_transport_net_bfs;
}

ScResult TransportNetBFSAgent::DoProgram(TransportNetBFSEvent const & event, ScAction & action)
{
  m_logger.Info("Agent start do program");
  m_logger.Info("Agent start to calculate route web diameter and central districts");
  ScIterator3Ptr const it3 =
      m_context.CreateIterator3(GraphKeynodes::concept_city, ScType::ConstPosArc, ScType::ConstNodeStructure);
  if (it3->Next())
  {
    ScAddr city = it3->Get(2);
    try
    {
      CalculateCentralRegionAndDiameterBFS(city);
    }
    catch (std::runtime_error const & e)
    {
      m_logger.Warning("Agent can't calculate diameter and central districts in non connect");
    }
  }
  else
  {
    m_logger.Error("No city");
    action.FinishUnsuccessfully();
  }
  m_logger.Info("Agent calculated diameter and central districts good");
  m_logger.Info("Agent start finding shortest ways");
  ScAddr city = event.GetArcTargetElement();
  ScAddrUnorderedSet districts = GetDistricts(city);
  SearchShortestWaysInCity(districts, city);
  m_logger.Info("Agent finish finding shortest ways");
  return action.FinishSuccessfully();
}

ScAddrUnorderedSet TransportNetBFSAgent::GetDistricts(ScAddr const & city)
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

void TransportNetBFSAgent::FindShortestWays(
    ScAddrUnorderedSet const & districts,
    ScAddr const & startVertex,
    std::map<std::string, int> & resDists)
{
  m_logger.Info("Start finding shortest ways for start vertex");
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
  m_logger.Info("Agent find all ways for districts, where we can appear from start vertex");
  for (auto const & district : districts)
  {
    if (visited.find(district) == visited.end())
    {
      std::string nameUnreachDistr = m_context.GetElementSystemIdentifier(district);
      resDists[nameUnreachDistr] = INF;
    }
  }
  m_logger.Info("Agent finish findig all ways for all districts");
}

void TransportNetBFSAgent::SearchShortestWaysInCity(ScAddrUnorderedSet const & districts, ScAddr & city)
{
  m_logger.Info("Start finding shortest ways in city");
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
      auto connectorCity = m_context.GenerateConnector(ScType::ConstCommonArc, city, way);
      auto connectorRrelShortWay =
          m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::nrel_shortest_way, connectorCity);
    }
  }
  m_logger.Info("Finish finding shortest ways in city");
}

void TransportNetBFSAgent::CalculateCentralRegionAndDiameterBFS(ScAddr & city)
{
  m_logger.Info("Start calculating diameter and central regions");
  int numberOfDistricts = GetDistricts(city).size();
  m_logger.Debug("Districts number: " + std::to_string(numberOfDistricts));
  std::vector<int> eccentricities(numberOfDistricts);
  int diameter = 0;
  int radius = numberOfDistricts;
  for (size_t i = 0; i < numberOfDistricts; i++)
  {
    std::vector<int> distances(numberOfDistricts, numberOfDistricts);
    m_logger.Debug("Start BFS for district_" + std::to_string(i));
    BFSShortestPathsSingleSource(i, distances, numberOfDistricts);
    m_logger.Info("Success BFS for district_" + std::to_string(i));
    m_logger.Debug("Start find maximum of shortest distances for district_" + std::to_string(i));
    int i_max = 0;
    for (int current = 0; current < numberOfDistricts; current++)
    {
      if (distances[current] == numberOfDistricts)
      {
        throw std::runtime_error("Graph is not connected");
      }
      i_max = std::max(i_max, distances[current]);
    }
    m_logger.Info("Success find maximum of shortest distances for district_" + std::to_string(i));
    eccentricities[i] = i_max;
    diameter = std::max(diameter, i_max);
    radius = std::min(radius, i_max);
  }
  m_logger.Debug("Start find and connect central districts to class of central districts");
  FindCentralDistricts(city, eccentricities, radius);
  m_logger.Debug("Success find and connect central districts to class of central districts");

  m_logger.Debug("Try create link diameter of transport net");
  ScAddr const & linkDiameterTransportNet = m_context.GenerateLink(ScType::ConstNodeLink);
  m_context.SetLinkContent(linkDiameterTransportNet, std::to_string(diameter));
  m_logger.Info("Success create link of diameter of transport net");
  m_logger.Debug("Try create connection between city and link");
  ScAddr const & arcCommonAddr = m_context.GenerateConnector(ScType::ConstCommonArc, city, linkDiameterTransportNet);
  ScAddr const & nrelTransportNetDiameter =
      m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::nrel_transport_net_diameter, arcCommonAddr);
  m_logger.Info("Success create connection between city and link");
  m_logger.Info("Success calculating diameter and central regions");
}

void TransportNetBFSAgent::FindCentralDistricts(ScAddr & city, std::vector<int> const & eccentrics, int const radius)
{
  int const n = eccentrics.size();
  for (int i = 0; i < n; i++)
  {
    if (eccentrics[i] == radius)
    {
      ScAddr center = m_context.SearchElementBySystemIdentifier(BASE_NAME_OF_NODES + std::to_string(i));
      if (center.IsValid())
      {
        m_logger.Debug("Try add district to center districts relation");
        ScAddr const & arcCityCentralDistrict = m_context.GenerateConnector(ScType::ConstPermPosArc, city, center);
        ScAddr const & arcCentralRelation = m_context.GenerateConnector(
            ScType::ConstPermPosArc, GraphKeynodes::rrel_central_district, arcCityCentralDistrict);
      }
    }
  }
}

void TransportNetBFSAgent::BFSShortestPathsSingleSource(int start, std::vector<int> & dist, int numberOfDistricts) const
{
  std::queue<int> utilBFSQueue;
  dist[start] = 0;
  utilBFSQueue.push(start);
  m_logger.Debug("Source vertex in BFS: " + std::to_string(start));
  while (!utilBFSQueue.empty())
  {
    int current_numb = utilBFSQueue.front();
    ScAddr current_district =
        m_context.SearchElementBySystemIdentifier(BASE_NAME_OF_NODES + std::to_string(current_numb));
    utilBFSQueue.pop();
    ScIterator5Ptr const it5 = m_context.CreateIterator5(
        current_district,
        ScType::ConstCommonEdge,
        ScType::ConstNode,
        ScType::ConstPermPosArc,
        GraphKeynodes::nrel_road);
    while (it5->Next())
    {
      ScAddr to_district = it5->Get(2);
      int to_district_numb = std::stoi(
          DividerNumberFromString::GetNumberOfRouteOrDistrict(m_context.GetElementSystemIdentifier(to_district)));
      if (dist[to_district_numb] > dist[current_numb] + 1)
      {
        dist[to_district_numb] = dist[current_numb] + 1;
        utilBFSQueue.push(to_district_numb);
      }
    }
  }
}
