#include "create_graph_district_agent.hpp"
#include <sc-memory/sc_memory_headers.hpp>
#include "keynodes/graph_district_keynodes.hpp"
#include <string>

CreateGraphAgent::CreateGraphAgent()
{
  m_logger = utils::ScLogger(utils::ScLogger::ScLogType::File, "logs/CreateGraphAgent.log", utils::ScLogLevel::Debug);
}

ScAddr CreateGraphAgent::GetActionClass() const
{
  return ScSetProcessingKeynodes::action_construct_an_undirected_transport_graph;
}

ScResult CreateGraphAgent::DoProgram(ScAction & action)
{
  ScAddr const & linkAddr = action.GetArgument(ScSetProcessingKeynodes::nrel_file_path);

  std::string path_to_scv_file;
  m_context.GetLinkContent(linkAddr, path_to_scv_file);
  m_logger.Debug("We get path and ready to create graph");
  return action.FinishSuccessfully();
}