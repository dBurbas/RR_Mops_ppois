#include "graph.hpp"
#include "validator/validator.hpp"
#include <stdexcept>

void Graph::AddVertex(
    std::string const & start_district,
    std::string const & end_district,
    std::string const & type_of_route)
{
  auto vertex_validator = VertexValidator();
  auto graph = this->adjacency_lists_;
  if (vertex_validator.Validate(graph[start_district], std::make_pair(end_district, type_of_route)))
  {
    throw std::logic_error("Such vertex already exist!");
  }
  if (vertex_validator.Validate(graph[end_district], std::make_pair(start_district, type_of_route)))
  {
    throw std::logic_error("Such vertex already exist!");
  }
  graph[start_district].push_back(std::make_pair(end_district, type_of_route));
  graph[end_district].push_back(std::make_pair(start_district, type_of_route));
}

std::map<std::string, std::vector<std::pair<std::string, std::string>>> Graph::GetGraph() const
{
  return this->adjacency_lists_;
}

std::vector<std::pair<std::string, std::string>> Graph::GetVertices(std::string const & vertex)
{
  if (this->adjacency_lists_.count(vertex) == 0)
  {
    throw std::logic_error("Such vertex is not exist!");
  }
  return this->adjacency_lists_[vertex];
}