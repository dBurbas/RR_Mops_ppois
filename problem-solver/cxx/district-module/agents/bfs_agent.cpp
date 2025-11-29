#include "bfs_agent.hpp"
#include "../keynodes/graph_district_keynodes.hpp"
#include "../settings/settings.hpp"
#include <queue>
#include "../utils/string_formatter.hpp"
#include <algorithm>

TransportNetBFSAgent::TransportNetBFSAgent()
{
  m_logger = utils::ScLogger(utils::ScLogger::ScLogType::File, "logs/BFSAgent.log", utils::ScLogLevel::Debug);
}

ScAddr TransportNetBFSAgent::GetEventSubscriptionElement() const
{
  return GraphKeynodes::concept_ready_to_analyze_city_routes;
}

ScTemplate TransportNetBFSAgent::GetInitiationConditionTemplate(TransportNetBFSEvent const & event) const
{
  ScTemplate templ;
  templ.Triple(GraphKeynodes::concept_ready_to_analyze_city_routes, ScType::VarPermPosArc, ScType::VarNode);
  return templ;
}

ScAddr TransportNetBFSAgent::GetActionClass() const
{
  return GraphKeynodes::action_transport_net_bfs;
}

ScResult TransportNetBFSAgent::DoProgram(TransportNetBFSEvent const & event, ScAction & action)
{
  m_logger.Info("Agent start to calculate route web diameter");
  ScIterator3Ptr const it3 =
      m_context.CreateIterator3(GraphKeynodes::concept_city, ScType::ConstPosArc, ScType::ConstNodeStructure);
  if (it3->Next())
  {
    ScAddr city = it3->Get(2);
    CalculateCentralRegionAndDiameterBFS(city);
  }
  else
  {
    m_logger.Error("No city");
    action.FinishUnsuccessfully();
  }
  return action.FinishSuccessfully();
}

void TransportNetBFSAgent::CalculateCentralRegionAndDiameterBFS(ScAddr & city) const
{
  int numberOfDistricts = CountCityDistricts();
  std::vector<int> eccentricities(numberOfDistricts);
  int diameter = 0;
  int radius = numberOfDistricts;
  for (size_t i = 0; i < numberOfDistricts; i++)
  {
    std::vector<int> distances(numberOfDistricts, numberOfDistricts);
    std::queue<int> utilBFSQueue;
    distances[i] = 0;
    utilBFSQueue.push(i);
    // TODO: вынести в отдельную функцию BFS одной вершины
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
        if (distances[to_district_numb] > distances[current_numb] + 1)
        {
          distances[to_district_numb] = distances[current_numb] + 1;
          utilBFSQueue.push(to_district_numb);
        }
        m_logger.Debug("Pushed" + std::to_string(to_district_numb));
      }
    }
    int i_max = 0;
    for (int current = 0; current < numberOfDistricts; current++)
    {
      if (distances[current] == numberOfDistricts)
      {
        throw std::runtime_error("Graph is not connected");
      }
      i_max = std::max(i_max, distances[current]);
    }
    eccentricities[i] = i_max;
    diameter = std::max(diameter, i_max);
    radius = std::min(radius, i_max);
  }
  ScAddrVector centers;
  for (int i = 0; i < numberOfDistricts; i++)
  {
    if (eccentricities[i] == radius)
    {
      ScAddr center = m_context.SearchElementBySystemIdentifier(BASE_NAME_OF_NODES + std::to_string(i));
      if (center.IsValid())
      {
        m_logger.Debug("Try add district in class of center districts");
        ScAddr const & arcCommonAddr =
            m_context.GenerateConnector(ScType::ConstCommonArc, GraphKeynodes::concept_center_district, center);
      }
    }
  }
  // TODO:вынести в отдельные функции ниже
  m_logger.Debug("Try create link diameter of transport net");
  ScAddr const & linkDiameterTransportNet = m_context.GenerateLink(ScType::ConstNodeLink);
  m_context.SetLinkContent(linkDiameterTransportNet, std::to_string(diameter));
  m_logger.Info("Create success link of diameter of transport net");
  m_logger.Debug("Try create connection between city and link");
  ScAddr const & arcCommonAddr = m_context.GenerateConnector(ScType::ConstCommonArc, city, linkDiameterTransportNet);
  ScAddr const & nrelTransportNetDiameter =
      m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::nrel_transport_net_diameter, arcCommonAddr);
  m_logger.Info("Success create connection between route and link");
}

void TransportNetBFSAgent::BFSShortestPathsUtil(std::vector<int> & dist) const {}

int TransportNetBFSAgent::CountCityDistricts() const
{
  m_logger.Debug("Agent start to make counting of districts");
  int numb = 0;
  std::string districtName;
  m_logger.Debug("Agent start to make cycle of counting of districts");
  while (true)
  {
    m_logger.Debug(numb);

    districtName = BASE_NAME_OF_NODES + std::to_string(numb);
    try
    {
      ScAddr district = m_context.SearchElementBySystemIdentifier(districtName);
      if (!district.IsValid())
        break;
      m_logger.Debug(districtName);
    }
    catch (utils::ExceptionInvalidParams const & e)
    {
      m_logger.Error("Agent: SearchElementBySystemIdentifier throws exception in counting city districts");
      throw std::runtime_error("Transport Net BFS agent: SearchElementBySystemIdentifier error");
      break;
    }
    ++numb;
    if (numb > 10000)
    {
      m_logger.Error("Agent: Infinite cycle during counting city districts");
      throw std::runtime_error("Transport Net BFS agent: Infinite cycle during counting districts");
      break;
    }
  };
  m_logger.Debug("Agent end counting districts");
  return numb;
}
