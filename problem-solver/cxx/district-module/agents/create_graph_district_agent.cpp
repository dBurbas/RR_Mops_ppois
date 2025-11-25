#include "create_graph_district_agent.hpp"
#include <sc-memory/sc_memory_headers.hpp>
#include "keynodes/graph_district_keynodes.hpp"
#include "../utils/string_formatter.hpp"
#include "../settings/settings.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <map>
// TODO: Добавить документацию
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
        m_context.GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::nrel_main_idtf, arcCommonAddr);

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
  // TODO: Разбить на отдельные функции
  // TODO: Переделать под SC Code Style :D
  // TODO: убрать лишние m_logger.Debug
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

    m_logger.Debug("Try create structure");
    // TODO: Добавить системный идентификатор для структур route-ов по типу route_0, route_1 ... 
    ScStructure route = m_context.GenerateStructure();
    route << start_district_node << end_district_node;
    m_logger.Info("Success create structure");

    m_logger.Debug("Try to create undefined type of route");
    ScAddr type_of_route;
    if (typeOfRoute.find("автобус") != std::string::npos)
    {
      type_of_route = m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::concept_bus_route, route);
    }
    else if (typeOfRoute.find("трамвай") != std::string::npos)
    {
      type_of_route = m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::concept_tram_route, route);
    }
    else
    {
      type_of_route = m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::concept_subway_route, route);
    }
    m_logger.Info("Success create type of route");

    m_logger.Debug("Try create link number of route");
    ScAddr const & linkNumberOfRoute = m_context.GenerateLink(ScType::ConstNodeLink);
    std::string number_of_route = DividerNumberFromString::GetNumberOfRoute(typeOfRoute);
    m_context.SetLinkContent(linkNumberOfRoute, number_of_route);
    m_logger.Info("Create success link nubmer or route");

    m_logger.Debug("Try create connection between route and link");
    ScAddr const & arcCommonAddr = m_context.GenerateConnector(ScType::ConstCommonArc, route, linkNumberOfRoute);
    ScAddr const & nrelNumber =
        m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::nrel_number, arcCommonAddr);
    m_logger.Info("Success create connection between route and link");
    m_logger.Debug("Try to add route to city");
    city << route;
    m_logger.Info("Succesffuly add route to city");
  }
  m_logger.Debug("Try to create city name");
  ScAddr const & linkCityName = m_context.GenerateLink(ScType::ConstNodeLink);
  // TODO: Подумать на счет того чтобы брать имя города из имени файла csv к примеру: Маршруты_Новосибирска.csv
  m_context.SetLinkContent(linkCityName, BASE_NAME_OF_CITY);
  m_logger.Info("Succesffuly add name to city");
  ScAddr const & nameCityArc = m_context.GenerateConnector(ScType::ConstCommonArc, city, linkCityName);
  m_logger.Debug("Try to create nrel city name");
  ScAddr const & nameCityArcNrel =
      m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::nrel_name, nameCityArc);
  m_logger.Info("Succesfully create nrel city name");
  // TODO: Сделать привязку структуры города к результату (подсказал Никита Владимирович ЗотоВ)
  // ScStructure result = m_context.GenerateStructure();
  // result << element1;
  // action.SetResult(result);
  return action.FinishSuccessfully();
}