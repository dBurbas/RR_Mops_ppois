#include <sc-memory/sc_keynodes.hpp>

class ScSetProcessingKeynodes : public ScKeynodes
{
public:
  static inline ScKeynode const action_construct_an_undirected_transport_graph{
      "action_construct_an_undirected_transport_graph",
      ScType::ConstNodeClass};
  static inline ScKeynode const nrel_file_path{
      "nrel_file_path",
      ScType::ConstNodeNonRole};
};
