#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "../utils/string_formatter.hpp"

TEST(StringFormatterTest, LtriManyLeftSpaces)
{
  std::string str = "   Artem";
  StringFormatter::Ltrim(str);
  EXPECT_EQ(str, "Artem");
}

TEST(StringFormatterTest, LtrimNoSpaces)
{
  std::string str = "Artem";
  StringFormatter::Ltrim(str);
  EXPECT_EQ(str, "Artem");
}

TEST(StringFormatterTest, LtrimManySpacesInEmptyString)
{
  std::string str = "    ";
  StringFormatter::Ltrim(str);
  EXPECT_EQ(str, "");
}

TEST(StringFormatterTest, LtrimManyRightSpaces)
{
  std::string str = "Something     ";
  StringFormatter::Ltrim(str);
  EXPECT_EQ(str, "Something     ");
}

TEST(StringFormatterTest, RtrimManyRightSpaces)
{
  std::string str = "Dmitriy   ";
  StringFormatter::Rtrim(str);
  EXPECT_EQ(str, "Dmitriy");
}

TEST(StringFormatterTest, RtrimNoSpaces)
{
  std::string str = "Dmitriy";
  StringFormatter::Rtrim(str);
  EXPECT_EQ(str, "Dmitriy");
}

TEST(StringFormatterTest, RtrimManySpacesSpacesInEmptyString)
{
  std::string str = "   ";
  StringFormatter::Rtrim(str);
  EXPECT_EQ(str, "");
}

TEST(StringFormatterTest, RtrimManyLeftSpaces)
{
  std::string str = "    Something";
  StringFormatter::Rtrim(str);
  EXPECT_EQ(str, "    Something");
}

TEST(StringFormatterTest, SplitSemicolon)
{
  std::string str = "Солигорск;Минск;автобус №5";
  auto parts = StringFormatter::Split(str, ';');

  ASSERT_EQ(parts.size(), 3);
  EXPECT_EQ(parts[0], "Солигорск");
  EXPECT_EQ(parts[1], "Минск");
  EXPECT_EQ(parts[2], "автобус №5");
}

TEST(StringFormatterTest, SplitWithIncorrectParamsBetweenSeparators)
{
  std::string str = ",a,,b,";
  auto parts = StringFormatter::Split(str, ',');

  ASSERT_EQ(parts.size(), 2);
}

TEST(StringFormatterTest, SplitWithoutSeparatorSymbol)
{
  std::string str = "abc";
  auto parts = StringFormatter::Split(str, ';');

  ASSERT_EQ(parts.size(), 1);
  EXPECT_EQ(parts[0], "abc");
}

TEST(StringOperationInSystemIdentifierTest, GetIntNumberLogicError)
{
  ASSERT_THROW(
      StringOperationInSystemIdentifier::GetSystemIdentifier("Artem"),
      std::logic_error
    );
}

TEST(StringOperationInSystemIdentifierTest, GetIntNumber)
{
  std::string str = "route_42";
  int number = StringOperationInSystemIdentifier::GetSystemIdentifier(str);
  EXPECT_EQ(number, 42);
}

TEST(StringOperationInSystemIdentifierFailTest, GetDigitsLogicError)
{
  ASSERT_THROW(
      DividerNumberFromString::GetNumberOfRouteOrDistrict("Artem"),
      std::logic_error
    );
}

TEST(DividerNumberFromStringTest, GetDigits)
{
  std::string str = "route42";
  std::string number = DividerNumberFromString::GetNumberOfRouteOrDistrict(str);
  EXPECT_EQ(number, "42");
}