#pragma once

#include <string>
#include <vector>

// TODO: Добавить документацию
class StringFormatter
{
public:
  static void Ltrim(std::string & str);
  static void Rtrim(std::string & str);
  static std::vector<std::string> Split(std::string const & str, char const & separator = ';');
};

class StringOperationInSystemIdentifier
{
public:
  static int GetSystemIdentifier(std::string const &);
};

class DividerNumberFromString
{
public:
  static std::string GetNumberOfRouteOrDistrict(std::string const &);
};