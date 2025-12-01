#include <sc-memory/test/sc_test.hpp>
#include <sc-memory/sc_memory_headers.hpp>
#include "agents/dfs_agent.hpp"
#include "agents/create_graph_district_agent.hpp"
#include "keynodes/graph_district_keynodes.hpp"
#include "settings/settings.hpp"
#include <thread>
#include <chrono>

using AgentTest = ScMemoryTest;

class DFSAgentTest : public AgentTest
{
protected:
  void SetUp() override
  {
    ScMemoryTest::SetUp();
    m_ctx->SubscribeAgent<CreateGraphAgent>();
    m_ctx->SubscribeAgent<TransportNetDFSAgent>();
  }

  void TearDown() override
  {
    m_ctx->UnsubscribeAgent<TransportNetDFSAgent>();
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

  bool CheckConnection(ScAddr const & concept, ScAddr const & element)
  {
    if (!concept.IsValid() || !element.IsValid())
      return false;

    ScIterator3Ptr it = m_ctx->CreateIterator3(concept, ScType::ConstPermPosArc, element);
    return it->Next();
  }

  int CountElementsWithRole(ScAddr const & nodeTuple, ScAddr const & role)
  {
    if (!nodeTuple.IsValid() || !role.IsValid())
      return 0;

    int count = 0;
    ScIterator5Ptr it =
        m_ctx->CreateIterator5(nodeTuple, ScType::ConstPermPosArc, ScType::Unknown, ScType::ConstPermPosArc, role);

    while (it->Next())
    {
      count++;
    }
    return count;
  }

  ScAddr FindNodeTuple(ScAddr const & city)
  {
    if (!city.IsValid())
      return ScAddr();

    ScIterator3Ptr it3 =
        m_ctx->CreateIterator3(GraphKeynodes::nrel_bridge, ScType::ConstPermPosArc, ScType::ConstNodeTuple);

    while (it3->Next())
    {
      ScAddr tuple = it3->Get(2);

      ScIterator5Ptr it5 = m_ctx->CreateIterator5(
          tuple, ScType::ConstPermPosArc, city, ScType::ConstPermPosArc, GraphKeynodes::rrel_graph);

      if (it5->Next())
        return tuple;
    }

    return ScAddr();
  }

  int CountComponentsOfConnectivity(ScAddr const & city)
  {
    if (!city.IsValid())
      return 0;

    int componentCount = 0;
    ScIterator5Ptr it = m_ctx->CreateIterator5(
        city,
        ScType::ConstCommonArc,
        ScType::ConstNodeStructure,
        ScType::ConstPermPosArc,
        GraphKeynodes::nrel_connectivity_component);

    while (it->Next())
    {
      componentCount++;
    }

    return componentCount;
  }
};

TEST_F(DFSAgentTest, DFSAgentAnalyzesConnectedGraph)
{
  std::string const csvData =
      "Центральный;Кировский;автобус №5\n"
      "Центральный;Ленинский;метро, линия 1 \n"
      "Кировский;Советский;автобус №12 \n"
      "Ленинский;Октябрьский;автобус №7 \n"
      "Советский;Дзержинский;трамвай №4 \n"
      "Октябрьский;Заельцовский;автобус №15 \n"
      "Дзержинский;Железнодорожный;автобус №23 \n"
      "Заельцовский;Железнодорожный;автобус №31 \n";

  ScAddr city = CreateAndAnalyzeGraph(csvData);
  EXPECT_TRUE(city.IsValid());

  EXPECT_TRUE(CheckConnection(GraphKeynodes::concept_connect_graph, city));
  EXPECT_FALSE(CheckConnection(GraphKeynodes::concept_noconnect_graph, city));
}

TEST_F(DFSAgentTest, DFSAgentAnalyzesDisconnectedGraph)
{
  std::string const csvData =
      "Ленинский;Кировский;автобус 1\n"
      "Октябрьский;Центральный;трамвай 2";

  ScAddr city = CreateAndAnalyzeGraph(csvData);
  EXPECT_TRUE(city.IsValid());

  EXPECT_TRUE(CheckConnection(GraphKeynodes::concept_noconnect_graph, city));
  EXPECT_FALSE(CheckConnection(GraphKeynodes::concept_connect_graph, city));
}

TEST_F(DFSAgentTest, DFSAgentFindsConnectivityComponents)
{
  std::string const csvData =
      "Ленинский;Кировский;автобус 1\n"
      "Кировский;Октябрьский;трамвай 2\n"
      "Центральный;Советский;автобус 3";

  ScAddr city = CreateAndAnalyzeGraph(csvData);
  EXPECT_TRUE(city.IsValid());

  int componentCount = CountComponentsOfConnectivity(city);
  EXPECT_EQ(componentCount, 2);
  EXPECT_TRUE(CheckConnection(GraphKeynodes::concept_noconnect_graph, city));
}

TEST_F(DFSAgentTest, DFSAgentFindsBridgesInCyclicGraph)
{
  std::string const csvData =
      "Центральный;Кировский;автобус №5\n"
      "Центральный;Ленинский;метро, линия 1 \n"
      "Кировский;Советский;автобус №12 \n"
      "Ленинский;Октябрьский;автобус №7 \n"
      "Советский;Дзержинский;трамвай №4 \n"
      "Октябрьский;Заельцовский;автобус №15 \n"
      "Дзержинский;Железнодорожный;автобус №23 \n"
      "Заельцовский;Железнодорожный;автобус №31 \n";

  ScAddr city = CreateAndAnalyzeGraph(csvData);
  EXPECT_TRUE(city.IsValid());

  ScAddr nodeTuple = FindNodeTuple(city);
  EXPECT_TRUE(nodeTuple.IsValid());

  int bridgeCount = CountElementsWithRole(nodeTuple, GraphKeynodes::rrel_bridge);

  EXPECT_EQ(bridgeCount, 0);
}

TEST_F(DFSAgentTest, DFSAgentIdentifiesBridgeInGraph)
{
  std::string const csvData =
      "Центральный;Ленинский;метро, линия 1 \n"
      "Кировский;Советский;автобус №12 \n"
      "Ленинский;Октябрьский;автобус №7 \n"
      "Советский;Дзержинский;трамвай №4 \n"
      "Октябрьский;Заельцовский;автобус №15 \n"
      "Дзержинский;Железнодорожный;автобус №23 \n"
      "Заельцовский;Железнодорожный;автобус №31 \n";

  ScAddr city = CreateAndAnalyzeGraph(csvData);
  EXPECT_TRUE(city.IsValid());

  ScAddr nodeTuple = FindNodeTuple(city);
  EXPECT_TRUE(nodeTuple.IsValid());

  int bridgeCount = CountElementsWithRole(nodeTuple, GraphKeynodes::rrel_bridge);

  EXPECT_EQ(bridgeCount, 7);
}