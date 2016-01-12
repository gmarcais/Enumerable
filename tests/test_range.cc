#include <iostream>
#include <vector>
#include <fstream>

#include <Enumerable.hpp>
// using Enumerable::times;
// using Enumerable::cat;
// using Enumerable::zipa;
// using Enumerable::container;
// using Enumerable::range;
// using Enumerable::lines;
using namespace Enumerable;

int main(int argc, char *argv[]) {
  range(2, 6).each([](int i) { std::cout << i << '\n'; });
  std::cout << "-----\n";
  times(5)
    .map([](int i) { return 2 * i; })
    .each([](int i) { std::cout << i << '\n'; });
  std::cout << "-----\n";
  for(auto it : range(2, 6))
    std::cout << it << '\n';
  std::cout << "-----\n";
  std::cout << times(5).inject(0, [](int a, int i) { return a + i + 1; }) << '\n';

  std::cout << "-----\n";
  std::vector<int> v = { 1, 3, 5, 9, 3 };
  std::cout << container(v).inject(1, [](int a, int i) { return a * i; }) << '\n';

  std::cout << container(v).all([](int i) { return i % 2 == 1; }) << '\n';
  std::cout << container(v).any([](int i) { return i % 2 == 0; }) << '\n';
  std::cout << container(v).count(3) << '\n';
  std::cout << container(v).max() << ' ' << container(v).min() << '\n';

  std::cout << container(v).select([](int i) { return i > 3; }).count() << '\n';
  std::cout << container(v).reject([](int i) { return i > 3; }).count() << '\n';

  { std::ifstream is("include/Enumerable.hpp");
    std::cout << lines(is).count() << '\n';
  }

  { std::ifstream is("include/Enumerable.hpp");
    std::cout << lines(is).map([](const std::string& l) { return l.size(); }).max() << '\n';
  }

  { std::ifstream is("include/Enumerable.hpp");
    std::cout << lines(is).map([](const auto& l) { return l.size(); }).min() << '\n';
  }

  { std::ifstream is("include/Enumerable.hpp");
    std::cout << lines(is).select([](const std::string& l) { return l.size() > 0; }).count() << '\n';
  }

  { std::ifstream is("include/Enumerable.hpp");
    for(const auto& line : lines(is))
      std::cout << line.size() << ' ';
    std::cout << '\n';
  }

  for(auto it : cat(times(5), times(7)))
    std::cout << it << ' ';
  std::cout << '\n';

  for(const auto& it : zipa(range(1, 4), range(5, 8)))
    std::cout << '(' << it[0] << ',' << it[1] << ')' << ' ';
  std::cout << '\n';

  { std::ifstream is("include/Enumerable.hpp");
    for(auto it : zip(lines(is), range(-100)))
      std::cout << std::get<1>(it) << ": " << std::get<0>(it) << '\n';
  }
  return 0;
}
