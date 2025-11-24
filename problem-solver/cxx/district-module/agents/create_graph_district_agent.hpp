#pragma once

#include <sc-memory/sc_agent.hpp>

class CreateGraphAgent : public ScActionInitiatedAgent
{
public:
  CreateGraphAgent();
  ScAddr GetActionClass() const override;
  ScAddr GetDistrict(std::string const& nameOfDistrict, int&number_of_districts);
  ScResult DoProgram(ScActionInitiatedEvent const & event, ScAction & action) override;
private:
  std::map<std::string, std::string> translate_map_;
  ScAddrVector districts_;
};