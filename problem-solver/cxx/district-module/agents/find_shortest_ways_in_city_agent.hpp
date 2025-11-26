#pragma once

#include <sc-memory/sc_agent.hpp>
#include <sc-memory/sc_agent.hpp>

using WaysEvent = ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc>;

class FindShortestWaysAgent : public ScAgent<WaysEvent>
{
public:
  FindShortestWaysAgent();
  ScAddr GetEventSubscriptionElement() const override;
  ScTemplate GetInitiationConditionTemplate(WaysEvent const & event) const override;
  ScAddr GetActionClass() const override;
  ScResult DoProgram(WaysEvent const & event, ScAction & action) override;
};