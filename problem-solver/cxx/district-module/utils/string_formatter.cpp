#include "string_formatter.hpp"
#include <stdexcept>

void StringFormatter::Ltrim(std::string & str)
{
  int start_pos = 0;
  while (start_pos < str.size() and std::isspace(str[start_pos]))
  {
    start_pos++;
  }
  int length = str.size() - start_pos;
  str = str.substr(start_pos, length);
}

void StringFormatter::Rtrim(std::string & str)
{
  int start_pos = str.size() - 1;
  while (start_pos >= 0 and std::isspace(str[start_pos]))
  {
    start_pos--;
  }
  int length = start_pos + 1;
  str = str.substr(0, length);
}

std::vector<std::string> StringFormatter::Split(std::string const & str, char const & separator)
{
  std::vector<std::string> result_of_split;
  size_t start_pos = 0;
  size_t pos = 0;

  while ((pos = str.find(separator, start_pos)) != std::string::npos)
  {
    if (pos != start_pos)
    {
      result_of_split.push_back(str.substr(start_pos, pos - start_pos));
    }
    start_pos = pos + 1;
  }

  if (start_pos < str.length())
  {
    result_of_split.push_back(str.substr(start_pos));
  }

  return result_of_split;
}

int StringOperationInSystemIdentifier::GetSystemIdentifier(std::string const & str)
{
  int underscore_symbol = str.size() - 1;
  while (underscore_symbol >= 0 && str[underscore_symbol] != '_')
  {
    underscore_symbol -= 1;
  }
  if (underscore_symbol < 0)
  {
    throw std::logic_error("Incorrect string");
  }
  return std::stoi(str.substr(underscore_symbol + 1));
}

std::string DividerNumberFromString::GetNumberOfRoute(std::string const & str)
{
  auto start_pos = str.size() - 1;
  while (start_pos >= 0 && isdigit(str[start_pos]))
  {
    start_pos--;
  }
  if (start_pos < 0)
  {
    throw std::logic_error("Incorrect input name of route");
  }
  return str.substr(start_pos + 1);
}