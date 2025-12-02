#include <sc-memory/test/sc_test.hpp>
#include <sc-memory/sc_memory_headers.hpp>
#include "agents/bfs_agent.hpp"
#include "agents/create_graph_district_agent.hpp"
#include "keynodes/graph_district_keynodes.hpp"
#include "settings/settings.hpp"

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

    ScWaiter waiter;
    waiter.Wait(500);
    return city;
  }

  ScAddr GetElementByMainIdentifier(std::string const & district)
  {
    ScIterator5Ptr it = m_ctx->CreateIterator5(
        ScType::ConstNode,
        ScType::ConstCommonArc,
        ScType::ConstNodeLink,
        ScType::ConstPermPosArc,
        ScKeynodes::nrel_main_idtf);
    while (it->Next())
    {
      auto infoLink = it->Get(2);
      std::string nameDistrict;
      m_ctx->GetLinkContent(infoLink, nameDistrict);
      if (nameDistrict == district)
      {
        return it->Get(0);
      }
    }
    return ScAddr();
  }

  int GetDistBetweenDistr(
      std::string const & nameFirstDistrict,
      std::string const & nameSecondDistrict,
      ScAddr const & city)
  {
    ScAddrVector structures;
    auto firstDistrict = GetElementByMainIdentifier(nameFirstDistrict);
    ScIterator3Ptr it = m_ctx->CreateIterator3(ScType::ConstNodeStructure, ScType::ConstPermPosArc, firstDistrict);
    while (it->Next())
    {
      auto wayStruct = it->Get(0);
      if (wayStruct != city)
      {
        structures.push_back(wayStruct);
      }
    }
    auto secondDistrict = GetElementByMainIdentifier(nameSecondDistrict);
    ScAddr structFind = ScAddr();
    for (auto const & st : structures)
    {
      ScIterator3Ptr itByDistInSt = m_ctx->CreateIterator3(st, ScType::ConstPermPosArc, secondDistrict);
      if (itByDistInSt->Next())
      {
        structFind = st;
      }
    }
    ScIterator5Ptr itLengthWay = m_ctx->CreateIterator5(
        structFind,
        ScType::ConstCommonArc,
        ScType::ConstNodeLink,
        ScType::ConstPermPosArc,
        GraphKeynodes::nrel_length_way);
    if (!itLengthWay->Next())
    {
      return INF;
    }
    std::string dist;
    auto linkDist = itLengthWay->Get(2);
    m_ctx->GetLinkContent(linkDist, dist);
    return std::stoi(dist);
  }

  int FindDiameterValueOfCity(ScAddr const & city)
  {
    ScIterator5Ptr it = m_ctx->CreateIterator5(
        city,
        ScType::ConstCommonArc,
        ScType::ConstNodeLink,
        ScType::ConstPermPosArc,
        GraphKeynodes::nrel_transport_net_diameter);
    if (!it->Next())
    {
      return 0;
    }
    auto linkDiameter = it->Get(2);
    std::string diameter;
    m_ctx->GetLinkContent(linkDiameter, diameter);
    return std::stoi(diameter);
  }

  int GetCountOfCentralDist(ScAddr const & city)
  {
    ScIterator5Ptr it = m_ctx->CreateIterator5(
        city,
        ScType::ConstPermPosArc,
        ScType::ConstNode,
        ScType::ConstPermPosArc,
        GraphKeynodes::rrel_central_district);
    int count = 0;
    while (it->Next())
    {
      count++;
    }
    return count;
  }
};

TEST_F(BFSAgentTest, BFSAgentCheckDistanceBetweenDistricts)
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

  int dist = GetDistBetweenDistr("Центральный", "Заельцовский", city);
  int dist2 = GetDistBetweenDistr("Ленинский", "Кировский", city);
  int dist3 = GetDistBetweenDistr("Ленинский", "Кировск", city);
  EXPECT_EQ(dist, 3);
  EXPECT_EQ(dist2, 2);
  EXPECT_EQ(dist3, INF);
}

TEST_F(BFSAgentTest, BFSAgentCheckDiameter)
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

  int diameter = FindDiameterValueOfCity(city);
  int countCentralDist = GetCountOfCentralDist(city);

  EXPECT_EQ(diameter, 4);
  EXPECT_EQ(countCentralDist, 8);
}

TEST_F(BFSAgentTest, BFSAgentCheckDiameter2)
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

  int diameter = FindDiameterValueOfCity(city);
  int countCentralDist = GetCountOfCentralDist(city);

  EXPECT_EQ(diameter, 7);
  EXPECT_EQ(countCentralDist, 2);
}

TEST_F(BFSAgentTest, BFSAgentCheckNoDiameter)
{
  std::string const csvData =
      "Дзержинский;Железнодорожный;автобус №23 \n"
      "Заельцовский;Центральный;автобус №31 \n";

  ScAddr city = CreateAndAnalyzeGraph(csvData);
  EXPECT_TRUE(city.IsValid());

  int diameter = FindDiameterValueOfCity(city);
  int countCentralDist = GetCountOfCentralDist(city);
  
  EXPECT_EQ(diameter, 0);
  EXPECT_EQ(countCentralDist, 0);
}