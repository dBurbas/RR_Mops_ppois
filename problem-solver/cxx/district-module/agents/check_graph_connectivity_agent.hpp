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
  ScAddrUnorderedSet GetDistricts(ScAddr const & city);
  void DFSOperations(ScAddrUnorderedSet & districts, ScAddrUnorderedSet & visited);
  std::string GetResultAnswer(ScAddrUnorderedSet const & resDistricts);
  ScResult DoProgram(ConnectivityEvent const & event, ScAction & action) override;
};