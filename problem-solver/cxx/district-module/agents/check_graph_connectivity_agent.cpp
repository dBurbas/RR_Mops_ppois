#include "check_graph_connectivity_agent.hpp"
#include "../keynodes/graph_district_keynodes.hpp"
#include "../utils/utils.hpp"
#include "../settings/settings.hpp"
#include <string>
#include <set>

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

void CheckConnectivityAgent::DFSOperations(ScAddrUnorderedSet & districts, ScAddrUnorderedSet & visitedDistricts)
{
  ScAddrStack stack;
  ScAddr startDistrict = *districts.begin();
  stack.push(startDistrict);
  visitedDistricts.insert(startDistrict);
  m_logger.Info("We start dfs");
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
      }
    }
  }
  m_logger.Info("We finish dfs");
}

std::string CheckConnectivityAgent::GetResultAnswer(ScAddrUnorderedSet const & resDistricts)
{
  std::string resultAnswer;
  if (resDistricts.size() == 0)
  {
    resultAnswer = POSITIVE_ANSWER;
  }
  else
  {
    resultAnswer = NEGATIVE_ANSWER;
    resultAnswer += "\n Список не достижимых вершин:\n";
    for (auto const & notConnectedDistrict : resDistricts)
    {
      std::string mainIdent = m_context.GetElementSystemIdentifier(notConnectedDistrict);
      resultAnswer += mainIdent;
      resultAnswer += "\n";
    }
  }
  return resultAnswer;
}

ScResult CheckConnectivityAgent::DoProgram(ConnectivityEvent const & event, ScAction & action)
{
  m_logger.Info("Agent start to check connectivity");
  ScAddr const & city = event.GetArcTargetElement();
  ScAddrUnorderedSet districts = GetDistricts(city);
  ScAddrUnorderedSet visitedDistricts;

  DFSOperations(districts, visitedDistricts);

  m_logger.Info("Check what districts are stay alone");
  ScAddrUnorderedSet resDistricts = OperationsWithUnorderedSet::FindElemNotInIntersection(districts, visitedDistricts);
  std::string resultAnswer = GetResultAnswer(resDistricts);
  m_logger.Info("Finish check what districts are stay alone");

  m_logger.Info("Create and connect result link");
  ScAddr const & linkResultConnectivity = m_context.GenerateLink(ScType::ConstNodeLink);
  m_context.SetLinkContent(linkResultConnectivity, resultAnswer);
  ScAddr const & resArcOfConnectivity =
      m_context.GenerateConnector(ScType::ConstCommonArc, city, linkResultConnectivity);
  m_logger.Info("Link was successful create and connect to city");

  m_logger.Info("Try to create nrel result of connectivity");
  ScAddr const & nameCityArcNrel = m_context.GenerateConnector(
      ScType::ConstPermPosArc, GraphKeynodes::nrel_result_connectivity, resArcOfConnectivity);
  m_logger.Info("Succesfully create nrel result of connectivity");

  return action.FinishSuccessfully();
}