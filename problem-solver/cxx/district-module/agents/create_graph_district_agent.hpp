#pragma once

#include <sc-memory/sc_agent.hpp>

class CreateGraphAgent : public ScActionInitiatedAgent
{
public:
  CreateGraphAgent();
  ScAddr GetActionClass() const override;
  ScResult DoProgram(ScActionInitiatedEvent const & event, ScAction & action) override;
};