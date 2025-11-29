#pragma once

#include <sc-memory/sc_agent.hpp>
#include <sc-memory/sc_agent.hpp>
#include <vector>

using TransportNetBFSEvent = ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc>;

class TransportNetBFSAgent : public ScAgent<TransportNetBFSEvent>
{
public:
  TransportNetBFSAgent();
  ScAddr GetEventSubscriptionElement() const override;
  ScTemplate GetInitiationConditionTemplate(TransportNetBFSEvent const & event) const override;
  ScAddr GetActionClass() const override;
  ScResult DoProgram(TransportNetBFSEvent const & event, ScAction & action) override;
  void CalculateCentralRegionAndDiameterBFS(ScAddr & city) const;
  void BFSShortestPathsUtil(std::vector<int> & dist) const;
  int CountCityDistricts() const;
};