#include <sc-memory/test/sc_test.hpp>
#include <sc-memory/sc_memory_headers.hpp>
#include "agents/create_graph_district_agent.hpp"
#include "keynodes/graph_district_keynodes.hpp"
#include "settings/settings.hpp"

using AgentTest = ScMemoryTest;

TEST_F(AgentTest, CreateGraphAgentBuildsCityGraphFromCsvFormat)
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

  ScIterator5Ptr const itCityName = m_ctx->CreateIterator5(
      city, ScType::ConstCommonArc, ScType::ConstNodeLink, ScType::ConstPermPosArc, GraphKeynodes::nrel_name);
  EXPECT_TRUE(itCityName->Next());

  auto cityNameLink = itCityName->Get(2);
  std::string cityName;
  m_ctx->GetLinkContent(cityNameLink, cityName);
  EXPECT_TRUE(cityName == BASE_NAME_OF_CITY);

  ScIterator3Ptr const itCityClass = m_ctx->CreateIterator3(GraphKeynodes::concept_city, ScType::ConstPermPosArc, city);
  EXPECT_TRUE(itCityClass->Next());

  ScIterator3Ptr const itReady =
      m_ctx->CreateIterator3(GraphKeynodes::action_ready_to_analyze_city_routes, ScType::ConstPermPosArc, city);
  EXPECT_TRUE(itReady->Next());

  m_ctx->UnsubscribeAgent<CreateGraphAgent>();
}

TEST_F(AgentTest, CreateGraphAgentFailsIfNoFilePathLink)
{
  m_ctx->SubscribeAgent<CreateGraphAgent>();

  ScAction action = m_ctx->GenerateAction(GraphKeynodes::action_construct_an_undirected_transport_graph);

  action.InitiateAndWait();

  EXPECT_FALSE(action.IsFinishedSuccessfully());

  m_ctx->UnsubscribeAgent<CreateGraphAgent>();
}

TEST_F(AgentTest, CreateGraphAgentBuildsCityGraphFromCsvFormatIncorrect)
{
  m_ctx->SubscribeAgent<CreateGraphAgent>();

  ScAction action = m_ctx->GenerateAction(GraphKeynodes::action_construct_an_undirected_transport_graph);

  std::string const csvData2 =
      "Ленинский;Кировский;автобус 1\n"
      "Кировский;Октябрьский;самолет 3";

  ScAddr const & linkCsv2 = m_ctx->GenerateLink(ScType::ConstNodeLink);
  m_ctx->SetLinkContent(linkCsv2, csvData2);

  ScAddr const & arcCommon2 = m_ctx->GenerateConnector(ScType::ConstCommonArc, action, linkCsv2);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::nrel_file_path, arcCommon2);

  action.InitiateAndWait();
  EXPECT_FALSE(action.IsFinishedSuccessfully());

  m_ctx->UnsubscribeAgent<CreateGraphAgent>();
}

TEST_F(AgentTest, CreateGraphAgentBuildsCityGraphFromCsvFormatIncorrectEmptyDistr)
{
  m_ctx->SubscribeAgent<CreateGraphAgent>();

  ScAction action = m_ctx->GenerateAction(GraphKeynodes::action_construct_an_undirected_transport_graph);

  std::string const csvData2 =
      "Ленинский;;автобус 1\n"
      "Кировский;Октябрьский;трамвай 3";

  ScAddr const & linkCsv2 = m_ctx->GenerateLink(ScType::ConstNodeLink);
  m_ctx->SetLinkContent(linkCsv2, csvData2);

  ScAddr const & arcCommon2 = m_ctx->GenerateConnector(ScType::ConstCommonArc, action, linkCsv2);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, GraphKeynodes::nrel_file_path, arcCommon2);

  action.InitiateAndWait();
  EXPECT_FALSE(action.IsFinishedSuccessfully());

  m_ctx->UnsubscribeAgent<CreateGraphAgent>();
}