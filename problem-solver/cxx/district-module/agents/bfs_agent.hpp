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

  void CalculateCentralRegionAndDiameterBFS(ScAddr & city);
  void FindCentralDistricts(ScAddr & city, std::vector<int> const & eccentrics, int const radius);
  void BFSShortestPathsSingleSource(int start, std::vector<int> & dist, int numberOfDistricts) const;

  ScAddrUnorderedSet GetDistricts(ScAddr const & city);
  void FindShortestWays(
      ScAddrUnorderedSet const & districts,
      ScAddr const & startVertex,
      std::map<std::string, int> & resDists);
  void SearchShortestWaysInCity(ScAddrUnorderedSet const & districts, ScAddr & city);
};