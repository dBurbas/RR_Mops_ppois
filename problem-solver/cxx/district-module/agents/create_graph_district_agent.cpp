#include "create_graph_district_agent.hpp"
#include <sc-memory/sc_memory_headers.hpp>
#include "../keynodes/graph_district_keynodes.hpp"
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

int CreateGraphAgent::GetNumberOfCurrentSystemIdentifier(std::string const & baseName)
{
  int left = 0;
  int right = MAX_COUNT_OF_NODES;

  while (left < right)
  {
    int mid = left + (right - left) / 2;
    if (m_context.SearchElementBySystemIdentifier(baseName + std::to_string(mid)).IsValid())
    {
      left = mid + 1;
    }
    else
    {
      right = mid;
    }
  }
  if (left >= MAX_COUNT_OF_NODES)
  {
    return 0;
  }

  return left;
}

void CreateGraphAgent::GetDistrict(ScAddr & districtNode, std::string const & nameOfDistrict)
{
  auto district = this->translateMap_.find(nameOfDistrict);
  if (district == this->translateMap_.end())
  {
    std::string const & resultSystemIdentifier =
        BASE_NAME_OF_NODES + std::to_string(GetNumberOfCurrentSystemIdentifier(BASE_NAME_OF_NODES));
    districtNode = m_context.GenerateNode(ScType::ConstNode);
    m_context.SetElementSystemIdentifier(resultSystemIdentifier, districtNode);

    ScAddr const & linkName = m_context.GenerateLink(ScType::ConstNodeLink);
    m_context.SetLinkContent(linkName, nameOfDistrict);

    ScAddr const & arcCommonAddr = m_context.GenerateConnector(ScType::ConstCommonArc, districtNode, linkName);
    ScAddr const & resDistrict =
        m_context.GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::nrel_main_idtf, arcCommonAddr);
    ScAddr const & districtToClassOfDistricts =
        m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::concept_district, districtNode);
    // this->districts_.push_back(districtNode);
    this->translateMap_[nameOfDistrict] = resultSystemIdentifier;
  }
  else
  {
    std::string const & resultSystemIdentifier = this->translateMap_[nameOfDistrict];
    districtNode = m_context.SearchElementBySystemIdentifier(resultSystemIdentifier);
    // int const & resIndex = StringOperationInSystemIdentifier::GetSystemIdentifier(resultSystemIdentifier);
    // districtNode = this->districts_[resIndex];
  }
}

void CreateGraphAgent::GetTypeOfRoute(ScAddr & typeOfRoute, std::string const & nameOfRoute, ScStructure const & route)
{
  if (nameOfRoute.find("автобус") != std::string::npos)
  {
    typeOfRoute = m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::concept_bus_route, route);
  }
  else if (nameOfRoute.find("трамвай") != std::string::npos)
  {
    typeOfRoute = m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::concept_tram_route, route);
  }
  else
  {
    typeOfRoute = m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::concept_subway_route, route);
  }
}

void CreateGraphAgent::GenerateRoutes(std::vector<std::string> const & resOfSplit, ScStructure & city)
{
  int numberOfDistricts = 0;
  int numberOfRoutes = 0;
  for (auto const & data : resOfSplit)
  {
    m_logger.Debug(data);

    auto const & districtElements = StringFormatter::Split(data, ';');
    auto const & startDistrict = districtElements[0];
    auto const & endDistrict = districtElements[1];
    auto const & nameOfRoute = districtElements[2];

    ScAddr startDistrictNode;
    ScAddr endDistrictNode;

    m_logger.Debug("Start district try to create");
    try
    {
      GetDistrict(startDistrictNode, startDistrict);
    }
    catch (std::logic_error const & e)
    {
      throw std::runtime_error("GenerateRoutes error: " + std::string(e.what()));
    }
    m_logger.Info("Start district create");

    m_logger.Debug("End district try to create");
    try
    {
      GetDistrict(endDistrictNode, endDistrict);
    }
    catch (std::logic_error const & e)
    {
      throw std::runtime_error("GenerateRoutes error: " + std::string(e.what()));
    }
    m_logger.Info("End district create");

    m_logger.Debug("Try to create road between two districts");
    ScAddr const & road = m_context.GenerateConnector(ScType::ConstCommonEdge, startDistrictNode, endDistrictNode);
    m_logger.Info("Create road between two districts");

    m_logger.Debug("Try to nrel road between two districts");
    ScAddr const & nrelRoad = m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::nrel_road, road);
    m_logger.Info("Create nrel road between two districts");

    m_logger.Debug("Try create structure");
    ScStructure route = m_context.GenerateStructure();
    std::string const & nameRoute =
        BASE_NAME_OF_ROUTE + std::to_string(GetNumberOfCurrentSystemIdentifier(BASE_NAME_OF_ROUTE));
    m_context.SetElementSystemIdentifier(nameRoute, route);
    numberOfRoutes++;
    route << startDistrictNode << endDistrictNode << road << nrelRoad;
    m_logger.Info("Success create structure");

    m_logger.Debug("Try to create undefined type of route");
    ScAddr typeOfRoute;
    GetTypeOfRoute(typeOfRoute, nameOfRoute, route);
    m_logger.Info("Success create type of route");

    m_logger.Debug("Try create link number of route");
    ScAddr const & linkNumberOfRoute = m_context.GenerateLink(ScType::ConstNodeLink);
    std::string numberOfRouteToDivide = DividerNumberFromString::GetNumberOfRouteOrDistrict(nameOfRoute);
    m_context.SetLinkContent(linkNumberOfRoute, numberOfRouteToDivide);
    m_logger.Info("Create success link nubmer or route");

    m_logger.Debug("Try create connection between route and link");
    ScAddr const & arcCommonAddr = m_context.GenerateConnector(ScType::ConstCommonArc, route, linkNumberOfRoute);
    ScAddr const & nrelNumber =
        m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::nrel_number, arcCommonAddr);
    m_logger.Info("Success create connection between route and link");

    m_logger.Debug("Try to add route to city");
    city << route << arcCommonAddr << nrelNumber << linkNumberOfRoute;
    m_logger.Info("Succesffuly add route to city");
  }
}

ScResult CreateGraphAgent::DoProgram(ScActionInitiatedEvent const & event, ScAction & action)
{
  ScIterator5Ptr const it5 = m_context.CreateIterator5(
      action, ScType::ConstCommonArc, ScType::ConstNodeLink, ScType::ConstPermPosArc, GraphKeynodes::nrel_file_path);
  it5->Next();
  ScAddr const & elementAddr = it5->Get(2);

  if (!elementAddr.IsValid())
  {
    m_logger.Warning("We can't find link");
    return action.FinishUnsuccessfully();
  }

  std::string scvFileData;
  m_logger.Info("Find link");

  m_context.GetLinkContent(elementAddr, scvFileData);
  m_logger.Info("We get path and ready to create graph");

  ScStructure city = m_context.GenerateStructure();

  auto resOfSplit = StringFormatter::Split(scvFileData, '\n');
  try
  {
    GenerateRoutes(resOfSplit, city);
  }
  catch (std::runtime_error const & e)
  {
    m_logger.Error(e.what());
    action.FinishUnsuccessfully();
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

  m_logger.Debug("Try to add city to class of cities");
  ScAddr const & CityPermArcClass =
      m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::concept_city, city);
  m_logger.Info("Succesffuly add city to class of cities");

  m_logger.Debug("Try to create concept which make signalize that graph was built");
  ScAddr const & actionSuccessBuildGraph =
      m_context.GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::action_ready_to_analyze_city_routes, city);
  m_logger.Info("Successful create concept which make signalize that graph was built");

  action.SetResult(city);

  return action.FinishSuccessfully();
}