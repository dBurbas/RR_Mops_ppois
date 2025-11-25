#include "graph_loader.hpp"
#include "../../utils/string_formatter.hpp"
#include <fstream>

Graph GraphLoader::LoadGraph(std::string const & path, char const & separator)
{
  std::ifstream file(path);

  if (!file.is_open())
  {
    throw std::runtime_error("Problems with opening file by this path:\t" + path);
  }

  auto graph = Graph();
  std::string line;

  while (std::getline(file, line))
  {
    std::string start_district, end_district, type_of_route;
    auto result_of_split = StringFormatter::Split(line, separator);
    if (result_of_split.size() != 3)
    {
      file.close();
      throw std::logic_error("Your scv file is not correct!");
    }
    for (auto str : result_of_split)
    {
      StringFormatter::Rtrim(str);
      StringFormatter::Ltrim(str);
      if (str.empty())
      {
        throw std::invalid_argument("Invalid data in your scv file!");
      }
    }
    start_district = result_of_split[0];
    end_district = result_of_split[1];
    type_of_route = result_of_split[2];
    try
    {
      graph.AddVertex(start_district, end_district, type_of_route);
    }
    catch (...)
    {
      file.close();
    }
  }
  file.close();
  return graph;
}
