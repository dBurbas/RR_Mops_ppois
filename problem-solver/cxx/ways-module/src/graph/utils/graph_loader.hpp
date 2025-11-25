#pragma once

#include "../graph.hpp"

class GraphLoader
{
public:
  Graph LoadGraph(std::string const & path, char const & separator = ';');
};