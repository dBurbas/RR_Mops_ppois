#include <sc-memory/sc_keynodes.hpp>

class GraphKeynodes : public ScKeynodes
{
public:
  static inline ScKeynode const action_construct_an_undirected_transport_graph{
      "action_construct_an_undirected_transport_graph",
      ScType::ConstNodeClass};
  static inline ScKeynode const action_check_connectivity{"action_check_connectivity", ScType::ConstNodeClass};
  static inline ScKeynode const action_find_shortest_ways{"action_find_shortest_ways", ScType::ConstNodeClass};
  static inline ScKeynode const concept_ready_to_analyze_city_routes{
      "concept_ready_to_analyze_city_routes",
      ScType::ConstNodeClass};
  static inline ScKeynode const nrel_result_connectivity{"nrel_result_connectivity", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_result_bridges{"nrel_result_bridges", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_file_path{"nrel_file_path", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_road{"nrel_road", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_name{"nrel_name", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_number{"nrel_number", ScType::ConstNodeNonRole};
  static inline ScKeynode const concept_bus_route{"concept_bus_route", ScType::ConstNodeClass};
  static inline ScKeynode const concept_tram_route{"concept_tram_route", ScType::ConstNodeClass};
  static inline ScKeynode const concept_subway_route{"concept_subway_route", ScType::ConstNodeClass};
};
