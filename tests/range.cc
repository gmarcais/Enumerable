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

TEST(Range, Zip) {
  EXPECT_TRUE(zip(range(0, 5), range(10, 5, -1)).all([](auto x, auto y) { return x + y == 10; }));
  //  EXPECT_TRUE(zip(range(0, 5), range(10, 5, -1)).all([](auto x) { return std::get<0>(x) + std::get<1>(x) == 10; }));
} // Range.Zip

TEST(Times, Basic) {
  std::vector<int> v, exp {0, 1, 2};
  times(3).collect(v);
  EXPECT_EQ(exp, v);
} // Times.Basic

TEST(Times, All) {
  EXPECT_TRUE(times(10).map([](auto x) { return 2 * x; })
              .all([](auto x) { return x % 2 == 0; }));
  EXPECT_FALSE(times(10).map([](auto x) { return 3 * x + 1; })
               .all([](auto x) { return x % 2 == 0; }));

} // Times.all

TEST(Times, Any) {
  EXPECT_TRUE(times(10).map([](auto x) { return 2 * x; })
              .any([](auto x) { return x % 4 == 0; }));
  EXPECT_FALSE(times(10).any([](auto x) { return x < 0; }));
} // Times.Any

TEST(Times, Count) {
  EXPECT_EQ((size_t)7, times(7).count());
} // Times.Count

TEST(Times, Select) {
  std::vector<int> v, exp{4, 5, 6};
  times(7).select([](auto x) { return x > 3; }).collect(v);
  EXPECT_EQ(exp, v);
} // Times.Select

TEST(Times, Reject) {
  std::vector<int> v, exp{0, 1, 2, 3};
  times(7).reject([](auto x) { return x > 3; }).collect(v);
  EXPECT_EQ(exp, v);
} // Times.Reject

TEST(Times, Drop) {
  EXPECT_EQ((size_t)5, times(10).drop(5).count());
} // Times.Drop


TEST(Container, Inject) {
  std::vector<int> i{1, 5, 6};
  auto res = container(i).inject(0, [](auto a, auto x) { return a + x; });
  EXPECT_EQ(12, res);
} // Container.inject

TEST(Container, Max) {
  std::vector<int> i{3, 5, 2, 10, 1}, e;
  auto res = container(i).max();
  EXPECT_EQ(10, res);
  res = container(i).max(100);
  EXPECT_EQ(10, res);
  res = container(e).max(100);
  EXPECT_EQ(100, res);
} // Container.Max

TEST(Container, Min) {
  std::vector<int> i{3, 5, 2, 10, 1}, e;
  auto res = container(i).min();
  EXPECT_EQ(1, res);
  res = container(i).min(100);
  EXPECT_EQ(1, res);
  res = container(e).min(100);
  EXPECT_EQ(100, res);
} // Container.Min

TEST(Lines, Count) {
  std::istringstream is("Hello\nCoucou\n");
  EXPECT_EQ((size_t)2, lines(is).count());
} // Lines.ForLoop

TEST(Lines, ForLoop) {
  std::istringstream is("hello\n# Comment\n\nVOila");
  auto ls = lines(is).reject([](auto& l) { return l.size() == 0 || l[0] == '#'; });
  std::vector<std::string> filtered;
  for(auto& l : ls)
    filtered.push_back(l);

  EXPECT_EQ(filtered[0], "hello");
  EXPECT_EQ(filtered[1], "VOila");
}

} // namespace
