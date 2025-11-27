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
  ScAddrUnorderedSet DFSConnection(
      ScAddrUnorderedSet & districts,
      ScAddr const & startDistrict,
      ScAddrUnorderedSet & visitedDistricts);
  void DFSBridges(ScAddrUnorderedSet & districts, std::map<std::string, std::string> & bridges);
  void GetResultOfConnectivity(
      std::vector<std::vector<std::string>> & resComponents,
      ScAddrUnorderedSet & districts,
      ScAddrUnorderedSet & visitedDistricts);
  ScResult DoProgram(ConnectivityEvent const & event, ScAction & action) override;
};