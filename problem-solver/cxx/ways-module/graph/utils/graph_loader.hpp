#pragma once

#include "../graph.hpp"

class GraphLoader
{
public:
  static Graph LoadGraph(std::string const & path, char const & separator = ';');
};