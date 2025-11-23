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

ScResult CreateGraphAgent::DoProgram(ScActionInitiatedEvent const & event, ScAction & action)
{
  m_logger.Debug("start");
  ScIterator5Ptr const it5 = m_context.CreateIterator5(
      action,
      ScType::ConstCommonArc,
      ScType::ConstNodeLink,
      ScType::ConstPermPosArc,
      ScSetProcessingKeynodes::nrel_file_path);
  it5->Next();
  ScAddr const & elementAddr = it5->Get(2);
  std::string path_to_scv_file;
  m_logger.Debug("Find link");

  m_context.GetLinkContent(elementAddr, path_to_scv_file);

  m_logger.Debug("We get path and ready to create graph");
  m_logger.Debug(path_to_scv_file);
  return action.FinishSuccessfully();
}