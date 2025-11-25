#pragma once

#include <sc-memory/sc_agent.hpp>

class CreateGraphAgent : public ScActionInitiatedAgent
{
public:
  CreateGraphAgent();
  ScAddr GetActionClass() const override;
  void GetDistrict(ScAddr & districtNode, std::string const & nameOfDistrict, int & numberOfDistricts);
  void GetTypeOfRoute(ScAddr & typeOfRoute, std::string const & nameOfRoute, ScStructure const & route);
  void GenerateRoutes(std::vector<std::string> const & resOfSplit, ScStructure & city);
  ScResult DoProgram(ScActionInitiatedEvent const & event, ScAction & action) override;

private:
  std::map<std::string, std::string> translate_map_;
  ScAddrVector districts_;
};