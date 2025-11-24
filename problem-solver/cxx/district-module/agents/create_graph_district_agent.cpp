#include "create_graph_district_agent.hpp"
#include <sc-memory/sc_memory_headers.hpp>
#include "keynodes/graph_district_keynodes.hpp"
#include "../utils/string_formatter.hpp"
#include "../settings/settings.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <map>

CreateGraphAgent::CreateGraphAgent()
{
  m_logger = utils::ScLogger(utils::ScLogger::ScLogType::File, "logs/CreateGraphAgent.log", utils::ScLogLevel::Debug);
}

ScAddr CreateGraphAgent::GetActionClass() const
{
  return GraphKeynodes::action_construct_an_undirected_transport_graph;
}

void CreateGraphAgent::GetDistrict(
    ScAddr & district_node,
    std::string const & nameOfDistrict,
    int & number_of_districts)
{
  auto district = this->translate_map_.find(nameOfDistrict);
  if (district == this->translate_map_.end())
  {
    std::string const & resultSystemIdentifier = BASE_NAME_OF_NODES + std::to_string(number_of_districts);
    district_node = m_context.GenerateNode(ScType::ConstNode);
    m_context.SetElementSystemIdentifier(resultSystemIdentifier, district_node);

    ScAddr const & linkName = m_context.GenerateLink(ScType::ConstNodeLink);
    m_context.SetLinkContent(linkName, nameOfDistrict);

    ScAddr const & arcCommonAddr = m_context.GenerateConnector(ScType::ConstCommonArc, district_node, linkName);
    ScAddr const & resDistrict =
        m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::nrel_name, arcCommonAddr);

    this->districts_.push_back(district_node);
    this->translate_map_[nameOfDistrict] = resultSystemIdentifier;
    number_of_districts++;
  }
  else
  {
    std::string const & resultSystemIdentifier = this->translate_map_[nameOfDistrict];
    int const & resIndex = StringOperationInSystemIdentifier::GetSystemIdentifier(resultSystemIdentifier);
    district_node = this->districts_[resIndex];
  }
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

  // ScStructure city = m_context.GenerateStructure();

  auto resOfSplit = StringFormatter::Split(scv_file_data, '\n');
  int number_of_districts = 0;
  for (auto const & data : resOfSplit)
  {
    m_logger.Debug(data);

    auto const & district_elements = StringFormatter::Split(data, ';');
    auto const & startDistrict = district_elements[0];
    auto const & endDistrict = district_elements[1];
    auto const & typeOfRoute = district_elements[2];

    ScAddr start_district_node;
    ScAddr end_district_node;
    m_logger.Debug("Start district try to create");
    try
    {
      GetDistrict(start_district_node, startDistrict, number_of_districts);
    }
    catch (std::logic_error const & e)
    {
      m_logger.Error(e.what());
      return action.FinishUnsuccessfully();
    }

    m_logger.Info("Start district create");
    m_logger.Debug("End district try to create");
    try
    {
      GetDistrict(end_district_node, endDistrict, number_of_districts);
    }
    catch (std::logic_error const & e)
    {
      m_logger.Error(e.what());
      return action.FinishUnsuccessfully();
    }
    m_logger.Info("End district create");

    m_logger.Debug("Try to create road between two districts");
    ScAddr const & road = m_context.GenerateConnector(ScType::ConstCommonEdge, start_district_node, end_district_node);
    m_logger.Info("Create road between two districts");

    m_logger.Debug("Try to nrel road between two districts");
    ScAddr const & nrelRoad = m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::nrel_road, road);
    m_logger.Info("Create nrel road between two districts");

    ScStructure route = m_context.GenerateStructure();
    // route << start_district_node << end_district_node << road << node_of_road;

    // ScAddr type_of_route = m_context.GenerateNode(ScType::ConstNode);
    // if (typeOfRoute.find("автобус") != std::string::npos)
    // {
    //   m_context.SetElementSystemIdentifier(GraphKeynodes::concept_bus_route, type_of_route);
    // }
    // else if (typeOfRoute.find("трамвай") != std::string::npos)
    // {
    //   m_context.SetElementSystemIdentifier(GraphKeynodes::concept_tram_route, type_of_route);
    // }
    // else
    // {
    //   m_context.SetElementSystemIdentifier(GraphKeynodes::concept_subway_route, type_of_route);
    // }

    // ScAddr route_belong = m_context.GenerateConnector(ScType::ConstPermPosArc, type_of_route, route);

    // city << route << type_of_route << route_belong;
  }

  return action.FinishSuccessfully();
}