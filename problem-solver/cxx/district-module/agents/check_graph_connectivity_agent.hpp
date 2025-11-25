#pragma once

#include <sc-memory/sc_agent.hpp>

class CheckConnectivityAgent : public ScAgent<ScEventAfterGenerateOutgoingArc<ScType::ConstCommonArc>>
{
public:
};