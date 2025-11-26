#include "find_shortest_ways_in_city_agent.hpp"
#include "../keynodes/graph_district_keynodes.hpp"

FindShortestWaysAgent::FindShortestWaysAgent()
{
  m_logger =
      utils::ScLogger(utils::ScLogger::ScLogType::File, "logs/FindShortestWayshAgent.log", utils::ScLogLevel::Debug);
}

ScAddr FindShortestWaysAgent::GetEventSubscriptionElement() const
{
  return GraphKeynodes::concept_ready_to_analyze_city_routes;
}

ScTemplate FindShortestWaysAgent::GetInitiationConditionTemplate(WaysEvent const & event) const
{
  ScTemplate templ;
  templ.Triple(GraphKeynodes::concept_ready_to_analyze_city_routes, ScType::VarPermPosArc, ScType::VarNode);
  return templ;
}

ScAddr FindShortestWaysAgent::GetActionClass() const
{
  return GraphKeynodes::action_find_shortest_ways;
}

ScResult FindShortestWaysAgent::DoProgram(WaysEvent const & event, ScAction & action)
{
  m_logger.Debug("Agent start to check connectivity");
  return action.FinishSuccessfully();
}