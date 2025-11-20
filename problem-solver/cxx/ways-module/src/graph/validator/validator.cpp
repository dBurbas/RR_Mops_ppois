#include "validator.hpp"
#include <algorithm>

bool VertexValidator::Validate(
    std::vector<std::pair<std::string, std::string>> const & list_vertex,
    std::pair<std::string, std::string> const & vertex) const
{
  return std::find(list_vertex.begin(), list_vertex.end(), vertex) != list_vertex.end();
}