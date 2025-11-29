#pragma once

#include <sc-memory/sc_agent.hpp>
#include <sc-memory/sc_agent.hpp>

using WaysEvent = ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc>;

class TransportNetBFSEvent : public ScAgent<WaysEvent>
{
public:
  TransportNetBFSEvent();
  ScAddr GetEventSubscriptionElement() const override;
  ScTemplate GetInitiationConditionTemplate(WaysEvent const & event) const override;
  ScAddr GetActionClass() const override;
  ScAddrUnorderedSet GetDistricts(ScAddr const & city);
  void FindShortestWays(
      ScAddrUnorderedSet const & districts,
      ScAddr const & startVertex,
      std::map<std::string, int> & resDists);
  void SearchShortestWaysInCity(ScAddrUnorderedSet const & districts, ScAddr & city);
  ScResult DoProgram(WaysEvent const & event, ScAction & action) override;
};