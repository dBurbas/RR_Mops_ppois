#include <gtest/gtest.h>
#include <map>
#include <string>
#include "../utils/utils.hpp"
#include "../settings/settings.hpp"
#include <sc-memory/sc_memory.hpp>

TEST(OperationsWithMapTest, GetNumberByFuncGetMin)
{
  std::map<std::string, int> map_1{
      {"a", 1},
      {"b", 5},
  };

  int value_a = OperationsWithMap::GetNumberMin(map_1, "a");
  int value_b = OperationsWithMap::GetNumberMin(map_1, "b");

  EXPECT_EQ(value_a, 1);
  EXPECT_EQ(value_b, 5);
}

TEST(OperationsWithMapTest, GetNumberByFuncGetMinIncorrect)
{
  std::map<std::string, int> map_1{
      {"artem", 1},
  };

  int value = OperationsWithMap::GetNumberMin(map_1, "something");
  EXPECT_EQ(value, -1);
}

TEST(OperationsWithMapTest, GetNumberByFuncGetMax)
{
  std::map<std::string, int> map_1{
      {"x", 10},
      {"y", 20},
  };

  int value_x = OperationsWithMap::GetNumberMax(map_1, "x");
  int value_y = OperationsWithMap::GetNumberMax(map_1, "y");

  EXPECT_EQ(value_x, 10);
  EXPECT_EQ(value_y, 20);
}

TEST(OperationsWithMapTest, GetNumberByFuncGetMaxIncorrect)
{
  std::map<std::string, int> map_1{
      {"dmitriy", 7},
  };

  int value = OperationsWithMap::GetNumberMax(map_1, "something");
  EXPECT_EQ(value, INF);
}
