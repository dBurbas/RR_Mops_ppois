#include <sc-memory/test/sc_test.hpp>
#include <sc-memory/sc_memory_headers.hpp>
#include "agents/create_graph_district_agent.hpp"
#include "keynodes/graph_district_keynodes.hpp"
#include "settings/settings.hpp"

using AgentTest = ScMemoryTest;

class CreationGraphAgentTest : public AgentTest
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

TEST_F(CreationGraphAgentTest, CreateGraphAgentBuildsCityGraphFromCsvFormat)
{
  std::string const csvData =
      "Ленинский;Кировский;автобус 1\n"
      "Кировский;Октябрьский;трамвай 3";

  ScAction action = CreateGraphActionWithCsv(csvData);

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

TEST_F(CreationGraphAgentTest, CreateGraphAgentFailsIfNoFilePathLink)
{
  m_ctx->SubscribeAgent<CreateGraphAgent>();

  ScAction action = m_ctx->GenerateAction(GraphKeynodes::action_construct_an_undirected_transport_graph);

  action.InitiateAndWait();

  EXPECT_FALSE(action.IsFinishedSuccessfully());

  m_ctx->UnsubscribeAgent<CreateGraphAgent>();
}

TEST_F(CreationGraphAgentTest, CreateGraphAgentBuildsCityGraphFromCsvFormatIncorrect)
{
  std::string const csvData2 =
      "Ленинский;Кировский;автобус 1\n"
      "Кировский;Октябрьский;самолет 3";

  ScAction action = CreateGraphActionWithCsv(csvData2);

  action.InitiateAndWait();

  EXPECT_FALSE(action.IsFinishedSuccessfully());

  m_ctx->UnsubscribeAgent<CreateGraphAgent>();
}

TEST_F(CreationGraphAgentTest, CreateGraphAgentBuildsCityGraphFromCsvFormatIncorrectEmptyDistr)
{
  std::string const csvData3 =
      "Ленинский;;автобус 1\n"
      "Кировский;Октябрьский;трамвай 3";

  ScAction action = CreateGraphActionWithCsv(csvData3);

  action.InitiateAndWait();

  EXPECT_FALSE(action.IsFinishedSuccessfully());

  m_ctx->UnsubscribeAgent<CreateGraphAgent>();
}