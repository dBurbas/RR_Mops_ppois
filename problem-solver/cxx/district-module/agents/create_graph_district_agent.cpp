#include "create_graph_district_agent.hpp"
#include <sc-memory/sc_memory_headers.hpp>
#include "keynodes/graph_district_keynodes.hpp"
#include "../utils/string_formatter.hpp"
#include <string>
#include <vector>
#include <fstream>

CreateGraphAgent::CreateGraphAgent()
{
  m_logger = utils::ScLogger(utils::ScLogger::ScLogType::File, "logs/CreateGraphAgent.log", utils::ScLogLevel::Debug);
}

ScAddr CreateGraphAgent::GetActionClass() const
{
  return GraphKeynodes::action_construct_an_undirected_transport_graph;
}

ScResult CreateGraphAgent::DoProgram(ScActionInitiatedEvent const & event, ScAction & action)
{
  m_logger.Debug("start");
  ScIterator5Ptr const it5 = m_context.CreateIterator5(
      action, ScType::ConstCommonArc, ScType::ConstNodeLink, ScType::ConstPermPosArc, GraphKeynodes::nrel_file_path);
  it5->Next();
  ScAddr const & elementAddr = it5->Get(2);
  std::string scv_file_data;

  m_logger.Debug("Find link");

  m_context.GetLinkContent(elementAddr, scv_file_data);

  m_logger.Debug("We get path and ready to create graph\n");
  ScStructure city = m_context.GenerateStructure();
  auto resOfSplit = StringFormatter::Split(scv_file_data, '\n');
  for (auto const & district : resOfSplit)
  {
    m_logger.Debug(district);
    auto const & district_elements = StringFormatter::Split(district, ';');
    auto const & startDistrict = district_elements[0];
    auto const & endDistrict = district_elements[1];
    auto const & typeOfRoute = district_elements[2];

    ScAddr start_district_node = m_context.GenerateNode(ScType::ConstNode);
    m_context.SetElementSystemIdentifier(startDistrict, start_district_node);

    ScAddr end_district_node = m_context.GenerateNode(ScType::ConstNode);
    m_context.SetElementSystemIdentifier(endDistrict, end_district_node);

    ScAddr road = m_context.GenerateConnector(ScType::ConstCommonEdge, start_district_node, end_district_node);
    ScAddr node_of_road = m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::nrel_road, road);

    ScStructure route = m_context.GenerateStructure();
    route << start_district_node << end_district_node << road << node_of_road;

    ScAddr type_of_route = m_context.GenerateNode(ScType::ConstNode);
    if (typeOfRoute.find("автобус") != std::string::npos)
    {
      m_context.SetElementSystemIdentifier(GraphKeynodes::concept_bus_route, type_of_route);
    }
    else if (typeOfRoute.find("трамвай") != std::string::npos)
    {
      m_context.SetElementSystemIdentifier(GraphKeynodes::concept_tram_route, type_of_route);
    }
    else
    {
      m_context.SetElementSystemIdentifier(GraphKeynodes::concept_subway_route, type_of_route);
    }

    ScAddr route_belong = m_context.GenerateConnector(ScType::ConstPermPosArc, type_of_route, route);

    city << route << type_of_route << route_belong;
  }

  return action.FinishSuccessfully();
}