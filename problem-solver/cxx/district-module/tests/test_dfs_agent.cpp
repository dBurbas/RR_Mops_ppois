#include <sc-memory/test/sc_test.hpp>
#include <sc-memory/sc_memory_headers.hpp>
#include "agents/create_graph_district_agent.hpp"
#include "agents/dfs_agent.hpp"
#include "keynodes/graph_district_keynodes.hpp"
#include "settings/settings.hpp"

using AgentTest = ScMemoryTest;

class AgentTestCreationGraph : public AgentTest
{
protected:
  ScAction CreateGraphActionWithCsv(std::string const & csvData)
  {
    m_ctx->SubscribeAgent<CreateGraphAgent>();

    ScAction action = m_ctx->GenerateAction(GraphKeynodes::action_construct_an_undirected_transport_graph);

    ScAddr const & linkCsv = m_ctx->GenerateLink(ScType::ConstNodeLink);
    m_ctx->SetLinkContent(linkCsv, csvData);

    ScAddr const & arcCommon = m_ctx->GenerateConnector(ScType::ConstCommonArc, action, linkCsv);
    m_ctx->GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::nrel_file_path, arcCommon);

    return action;
  }
};

TEST_F(AgentTestCreationGraph, DFSAgentFindConnectivityComponents)
{
  std::string const csvData =
      "Ленинский;Кировский;автобус 1\n"
      "Кировский;Октябрьский;трамвай 3";

  ScAction action = CreateGraphActionWithCsv(csvData);

  action.InitiateAndWait();

  EXPECT_TRUE(action.IsFinishedSuccessfully());

  m_ctx->UnsubscribeAgent<CreateGraphAgent>();

  m_ctx->SubscribeAgent<TransportNetDFSAgent>();

  ScAction actionDfs = m_ctx->GenerateAction(GraphKeynodes::action_transport_net_dfs);

  EXPECT_TRUE(actionDfs.InitiateAndWait());
  EXPECT_TRUE(actionDfs.IsFinishedSuccessfully());
}
