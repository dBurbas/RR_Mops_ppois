#include "check_graph_connectivity_agent.hpp"
#include "../keynodes/graph_district_keynodes.hpp"

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

ScResult CheckConnectivityAgent::DoProgram(ConnectivityEvent const & event, ScAction & action)
{
  m_logger.Info("Agent start to check connectivity");
  return action.FinishSuccessfully();
}