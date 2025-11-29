#include "district_module.hpp"
#include "agents/bfs_agent.hpp"
#include "agents/dfs_agent.hpp"
#include "agents/create_graph_district_agent.hpp"

SC_MODULE_REGISTER(DistrictModule)
    ->Agent<CreateGraphAgent>()
    ->Agent<TransportNetDFSAgent>()
    ->Agent<TransportNetBFSAgent>();