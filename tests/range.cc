#include <gtest/gtest.h>
#include <Enumerable.hpp>

namespace  {
using namespace Enumerable;

TEST(Range, Basic) {
  std::vector<int> v, exp {2, 3, 4, 5};
  range(2, 6).collect(v);
  EXPECT_EQ(exp, v);
} // Suite.Test

TEST(Range, Step) {
  std::vector<int> v, exp {-5, -3, -1, 1};
  range(-5, 2, 2).collect(v);
  EXPECT_EQ(exp, v);
} // Range.Step


TEST(Range, map) {
  std::vector<int> v, exp {3, 5, 7, 9, 11, 13};
  range(1, 7)
    .map([](auto x) { return 2 * x + 1; })
    .collect(v);
  EXPECT_EQ(exp, v);
} // Range.map

} // namespace
