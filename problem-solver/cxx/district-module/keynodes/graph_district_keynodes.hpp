#include <sc-memory/sc_keynodes.hpp>

class GraphKeynodes : public ScKeynodes
{
public:
  static inline ScKeynode const action_construct_an_undirected_transport_graph{
      "action_construct_an_undirected_transport_graph",
      ScType::ConstNodeClass};
  static inline ScKeynode const action_transport_net_dfs{"action_transport_net_dfs", ScType::ConstNodeClass};
  // static inline ScKeynode const action_find_shortest_ways{"action_find_shortest_ways", ScType::ConstNodeClass};
  static inline ScKeynode const action_transport_net_bfs{"action_transport_net_bfs", ScType::ConstNodeClass};
  // static inline ScKeynode const action_find_shortest_ways{"action_find_shortest_ways", ScType::ConstNodeClass};
  // static inline ScKeynode const action_calculate_route_net_diameter_and_central{
  //     "action_calculate_route_net_diameter_and_central",
  //     ScType::ConstNodeClass};
  static inline ScKeynode const action_ready_to_analyze_city_routes{
      "action_ready_to_analyze_city_routes",
      ScType::ConstNodeClass};
  // static inline ScKeynode const nrel_result_connectivity{"nrel_result_connectivity", ScType::ConstNodeNonRole};
  // static inline ScKeynode const nrel_result_bridge{"nrel_result_bridge", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_transport_net_diameter{"nrel_transport_net_diameter", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_file_path{"nrel_file_path", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_road{"nrel_road", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_name{"nrel_name", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_bridge{"nrel_bridge", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_shortest_way{"nrel_shortest_way", ScType::ConstNodeNonRole};
  static inline ScKeynode const rrel_bridge{"rrel_bridge", ScType::ConstNodeRole};
  static inline ScKeynode const rrel_graph{"rrel_graph", ScType::ConstNodeRole};
  static inline ScKeynode const rrel_subgraph{"rrel_subgraph", ScType::ConstNodeRole};
  static inline ScKeynode const rrel_central_district{"rrel_central_district", ScType::ConstNodeRole};
  static inline ScKeynode const nrel_number{"nrel_number", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_length_way{"nrel_length_way", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_connectivity_component{"nrel_connectivity_component", ScType::ConstNodeNonRole};
  static inline ScKeynode const concept_bus_route{"concept_bus_route", ScType::ConstNodeClass};
  static inline ScKeynode const concept_tram_route{"concept_tram_route", ScType::ConstNodeClass};
  static inline ScKeynode const concept_subway_route{"concept_subway_route", ScType::ConstNodeClass};
  static inline ScKeynode const concept_city{"concept_city", ScType::ConstNodeClass};
  static inline ScKeynode const concept_district{"concept_district", ScType::ConstNodeClass};
  static inline ScKeynode const concept_connect_graph{"concept_connect_graph", ScType::ConstNodeClass};
  static inline ScKeynode const concept_noconnect_graph{"concept_noconnect_graph", ScType::ConstNodeClass};
  static inline ScKeynode const concept_shortest_way{"concept_shortest_way", ScType::ConstNodeClass};
  // static inline ScKeynode const concept_route{"concept_route", ScType::ConstNodeClass};
  // static inline ScKeynode const concept_bridge{"concept_bridge", ScType::ConstNodeClass};
};
