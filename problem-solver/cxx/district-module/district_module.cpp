#include "district_module.hpp"
#include "agents/find_shortest_ways_in_city_agent.hpp"
#include "agents/check_graph_connectivity_agent.hpp"
#include "agents/create_graph_district_agent.hpp"

SC_MODULE_REGISTER(DistrictModule)
    ->Agent<CreateGraphAgent>()
    ->Agent<CheckConnectivityAgent>()
    ->Agent<FindShortestWaysAgent>();