#pragma once

#include <string>
#include <map>
#include <utility>
#include <vector>

class Graph
{
public:
  void AddVertex(
      std::string const & start_district,
      std::string const & end_district,
      std::string const & type_of_route);

  std::map<std::string, std::vector<std::pair<std::string, std::string>>> GetGraph() const;

  std::vector<std::pair<std::string, std::string>> GetVertices(std::string const & vertex);

private:
  std::map<std::string, std::vector<std::pair<std::string, std::string>>> adjacency_lists_;
};