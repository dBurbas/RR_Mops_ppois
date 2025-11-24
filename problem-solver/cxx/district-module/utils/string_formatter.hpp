#pragma once

#include <string>
#include <vector>

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