#pragma once
#include <sc-memory/sc_agent.hpp>
using ConnectivityEvent = ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc>;

class CheckConnectivityAgent : public ScAgent<ConnectivityEvent>
{
public:
  CheckConnectivityAgent();
  ScAddr GetEventSubscriptionElement() const override;
  ScTemplate GetInitiationConditionTemplate(ConnectivityEvent const & event) const override;
  ScAddr GetActionClass() const override;
  int GetCountOfEdges(ScAddr const & el);
  ScAddr GetElementByIterator(ScAddr const & el, int index);
  ScAddrUnorderedSet GetDistricts(ScAddr const & city);
  std::string GetMainIndentifier(std::string const & district);
  ScAddrUnorderedSet DFSConnection(
      ScAddrUnorderedSet & districts,
      ScAddr const & startDistrict,
      ScAddrUnorderedSet & visitedDistricts);
  void DFSBridges(
      ScAddrUnorderedSet & districts,
      std::vector<std::pair<std::string, std::string>> & bridges,
      std::vector<std::pair<ScAddr, ScAddr>> & bridgesAddr);
  void DefineTypeOfGraph(
      std::vector<ScAddrUnorderedSet> & resComponents,
      ScAddr & city,
      ScAddrUnorderedSet & resDistricts);
  void GetResultOfConnectivity(
      std::vector<ScAddrUnorderedSet> & resComponents,
      ScAddrUnorderedSet & districts,
      ScAddrUnorderedSet & visitedDistricts);
  void GetComponents(std::vector<ScAddrUnorderedSet> & resComponents, ScAddr & city, ScAddr & nodeTuple);
  void GetBridges(std::vector<std::pair<ScAddr, ScAddr>> & bridgesAddr, ScAddr & nodeTuple);
  ScResult DoProgram(ConnectivityEvent const & event, ScAction & action) override;
};