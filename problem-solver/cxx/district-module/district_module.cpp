#include "district_module.hpp"
#include "agents/create_graph_district_agent.hpp"

SC_MODULE_REGISTER(DistrictModule)->Agent<CreateGraphAgent>();