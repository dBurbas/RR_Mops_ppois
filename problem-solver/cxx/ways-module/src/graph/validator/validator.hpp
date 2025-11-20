#pragma once

#include <vector>
#include <utility>
#include <string>

class VertexValidator
{
public:
  bool Validate(
      std::vector<std::pair<std::string, std::string>> const & list_vertex,
      std::pair<std::string, std::string> const & vertex) const;
};
