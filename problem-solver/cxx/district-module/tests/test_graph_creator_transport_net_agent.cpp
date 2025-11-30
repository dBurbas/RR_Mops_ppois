
#include <sc-memory/test/sc_test.hpp>
#include <sc-memory/sc_memory_headers.hpp>
#include "agents/create_graph_district_agent.hpp"
#include "keynodes/graph_district_keynodes.hpp"
#include "settings/settings.hpp"

using AgentTest = ScMemoryTest;

TEST_F(AgentTest, CreateGraphAgent_BuildsCityGraphFromCsv)
{
  m_ctx->SubscribeAgent<CreateGraphAgent>();

  ScAction action = m_ctx->GenerateAction(GraphKeynodes::action_construct_an_undirected_transport_graph);

  std::string const csvData =
      "Ленинский;Кировский;автобус 1\n"
      "Кировский;Октябрьский;трамвай 3";

  ScAddr const & linkCsv = m_ctx->GenerateLink(ScType::ConstNodeLink);
  m_ctx->SetLinkContent(linkCsv, csvData);

  ScAddr const & arcCommon = m_ctx->GenerateConnector(ScType::ConstCommonArc, action, linkCsv);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::nrel_file_path, arcCommon);

  action.InitiateAndWait();

  EXPECT_TRUE(action.IsFinishedSuccessfully());

  ScStructure city = action.GetResult();
  EXPECT_FALSE(city.IsEmpty());

  ScIterator3Ptr const itCityClass = m_ctx->CreateIterator3(GraphKeynodes::concept_city, ScType::ConstPermPosArc, city);
  EXPECT_TRUE(itCityClass->Next());

  ScIterator3Ptr const itReady =
      m_ctx->CreateIterator3(GraphKeynodes::action_ready_to_analyze_city_routes, ScType::ConstPermPosArc, city);
  EXPECT_TRUE(itReady->Next());
}
