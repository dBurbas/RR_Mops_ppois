#include <sc-memory/test/sc_test.hpp>
#include <sc-memory/sc_memory_headers.hpp>
#include "agents/bfs_agent.hpp"
#include "agents/create_graph_district_agent.hpp"
#include "keynodes/graph_district_keynodes.hpp"
#include "settings/settings.hpp"
#include <thread>
#include <chrono>

using AgentTest = ScMemoryTest;

class BFSAgentTest : public AgentTest
{
protected:
  void SetUp() override
  {
    ScMemoryTest::SetUp();
    m_ctx->SubscribeAgent<CreateGraphAgent>();
    m_ctx->SubscribeAgent<TransportNetBFSAgent>();
  }

  void TearDown() override
  {
    m_ctx->UnsubscribeAgent<TransportNetBFSAgent>();
    m_ctx->UnsubscribeAgent<CreateGraphAgent>();
    ScMemoryTest::TearDown();
  }

  ScAddr CreateAndAnalyzeGraph(std::string const & csvData)
  {
    ScAction createAction = m_ctx->GenerateAction(GraphKeynodes::action_construct_an_undirected_transport_graph);

    ScAddr const linkCsv = m_ctx->GenerateLink(ScType::ConstNodeLink);
    m_ctx->SetLinkContent(linkCsv, csvData);

    ScAddr const arcCommon = m_ctx->GenerateConnector(ScType::ConstCommonArc, createAction, linkCsv);
    m_ctx->GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::nrel_file_path, arcCommon);

    createAction.InitiateAndWait();
    ScStructure city = createAction.GetResult();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    return city;
  }

  // ScAddr GetElementByMainIdentifier(std::string const & district)
  // {
  //   ScIterator5Ptr it = m_ctx->CreateIterator5(
  //       ScType::ConstNode,
  //       ScType::ConstCommonArc,
  //       ScType::ConstNodeLink,
  //       ScType::ConstPermPosArc,
  //       ScKeynodes::nrel_main_idtf);
  //   while (it->Next())
  //   {
  //     auto infoLink = it->Get(2);
  //     std::string nameDistrict = m_ctx->GetLinkContent(infoLink);
  //     if (nameDistrict == district)
  //     {
  //       return it->Get(0);
  //     }
  //   }
  //   return ScAddr();
  // }
};