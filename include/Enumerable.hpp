#ifndef __ENUMERABLE_H__
#define __ENUMERABLE_H__

#include <utility>
#include <iterator>
#include <functional>
#include <istream>
#include <array>
#include <tuple>
#include <limits>

namespace Enumerable {

namespace imp {
template<typename Enum, typename Block> class Map;
template<typename Enum, typename Block> class Select;

template<typename Block>
class Not {
protected:
  Block m_block;
public:
  explicit Not(const Block& b) : m_block(b) { }
  template<typename T>
  bool operator()(const T& x) const { return !m_block(x); }
};

// Helper class to get a function argument type
template<typename T>
struct function_traits;

template<typename R, typename... Args>
struct function_traits<std::function<R(Args...)>> {
    static const size_t nargs = sizeof...(Args);

    typedef R result_type;

    template<size_t i>
    struct arg {
        typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
    };
};

template<typename Block, typename T, size_t N = std::tuple_size<T>::value, size_t... Ns>
struct apply : public apply<Block, T, N-1, N-1, Ns...>
{ };

template<typename Block, typename T, size_t... Ns>
struct apply<Block, T, 0, Ns...> {
  static auto call(Block b, const T& t) { return b(std::get<Ns>(t)...); }
  template<typename U>
  static auto call2(Block b, U&& x, const T& t) { return b(x, std::get<Ns>(t)...); }
};

// Base. It uses CRTP. A derived class must have a prefix ++ operator,
// the dereference * operator and a cast to bool operator, returning
// false if the enumerable has no more elements.
template<typename Derived, typename T>
class Base {
protected:
  template<typename Block, typename... Ts>
  inline auto call_block(Block b, const std::tuple<Ts...>& x) {
    return apply<Block, std::tuple<Ts...>>::call(b, x);
  }
  template<typename Block, typename U, typename... Ts>
  inline auto call_block(Block b, U&& a, const std::tuple<Ts...>& x) {
    return apply<Block, std::tuple<Ts...>>::call2(b, std::forward<U>(a), x);
  }
  template<typename Block, typename U>
  inline auto call_block(Block b, const U& x) {
    return b(x);
  }
  template<typename Block, typename U, typename V>
  inline auto call_block(Block b, U&& x, const V& y) {
    return b(std::forward<U>(x), y);
  }
public:
  typedef T value_type;
  template<typename Block>
  void each(Block b) {
    auto& self = *static_cast<Derived*>(this);
    for( ; self; ++self)
      call_block(b, *self);
  }

  template<typename Block>
  Map<Derived, Block> map(Block b) {
    auto& self = *static_cast<Derived*>(this);
    return Map<Derived, Block>(self, b);
  }

  template<typename Block, typename U>
  U inject(U&& start, Block b) {
    auto& self = *static_cast<Derived*>(this);
    auto  acc  = std::forward<U>(start);
    for( ; self; ++self)
      acc = call_block(b, acc, *self);
    return acc;
  }

  // template<typename Block>
  // auto inject(Block b) {
  //   typedef typename function_traits<Block>::template arg<0>::type arg0;
  //   return inject(arg0(), b);
  // }

  template<typename Output>
  Output output(Output it) {
    auto& self = *static_cast<Derived*>(this);
    for( ; self; ++self, ++it)
      *it = *self;
    return it;
  }

  template<typename Container>
  auto collect(Container& c) { return output(std::back_inserter(c)); }

  template<typename Block>
  bool all(Block b) {
    auto& self = *static_cast<Derived*>(this);
    for( ; self; ++self)
      if(!call_block(b, *self))
        return false;
    return true;
  }

  template<typename Block>
  bool any(Block b) {
    auto& self = *static_cast<Derived*>(this);
    for( ; self; ++self)
      if(call_block(b, *self))
        return true;
     return false;
  }

  size_t count() {
    auto& self = *static_cast<Derived*>(this);
    size_t res = 0;
    for( ; self; ++self, ++res) ;
    return res;
  }

  template<typename U>
  size_t count(const U& x) {
    auto& self = *static_cast<Derived*>(this);
    size_t res = 0;
    for( ; self; ++self)
      res += *self == x;
    return res;
  }

  Derived& drop(size_t n) {
    auto& self = *static_cast<Derived*>(this);
    for(size_t i = 0; i < n && self; ++i, ++self) ;
    return self;
  }

  value_type max(value_type&& x = typename Derived::value_type()) {
    auto& self = *static_cast<Derived*>(this);
    auto res = std::forward<value_type>(x);
    if(self) {
      res  = *self;
      for(++self; self; ++self)
        res = std::max(res, *self);
    }
    return res;
  }

  value_type min(value_type&& x = typename Derived::value_type()) {
    auto& self = *static_cast<Derived*>(this);
    auto res = std::forward<value_type>(x);
    if(self) {
      res  = *self;
      for(++self; self; ++self)
        res = std::min(res, *self);
    }
    return res;
  }

  template<typename Block>
  Select<Derived, Block> select(Block b) {
    auto& self = *static_cast<Derived*>(this);
    return Select<Derived, Block>(self, b);
  }

  template<typename Block>
  Select<Derived, Not<Block>> reject(Block b) {
    auto& self = *static_cast<Derived*>(this);
    return Select<Derived, Not<Block>>(self, Not<Block>(b));
  }
};

// Range
template<typename T>
class Range : public Base<Range<T>, T> {
  T m_current;
  T m_end;
  T m_step;
public:
  typedef T value_type;
  Range(T start, T end, T step = 1) : m_current(start), m_end(end), m_step(step) { }
  operator bool() const { return m_current < m_end; }
  void operator++() { m_current += m_step; }
  T operator*() const { return m_current; }
};

// Map
template<typename Enum, typename Block>
class Map : public Base<Map<Enum, Block>, typename std::result_of<Block(typename Enum::value_type)>::type> {
  Enum  m_enumerable;
  Block m_block;
public:
  typedef typename Enum::value_type                      arg_type;
  typedef typename std::result_of<Block(arg_type)>::type value_type;

  Map(Enum e, Block b) : m_enumerable(e), m_block(b) { }
  operator bool() const { return m_enumerable; }
  void operator++() { ++m_enumerable; }
  value_type operator*() const { return m_block(*m_enumerable); }
};

// Select
template<typename Enum, typename Block>
class Select : public Base<Select<Enum, Block>, typename Enum::value_type> {
public:
  typedef typename Enum::value_type value_type;
protected:
  Enum                                         m_enumerable;
  Block                                        m_block;
  typename std::remove_const<value_type>::type m_value;
  bool                                         m_has_value;
public:
  Select(Enum e, Block b) : m_enumerable(e), m_block(b) { operator++(); }
  operator bool() const { return m_has_value; }
  void operator++() {
    m_has_value = false;
    for( ; m_enumerable && !m_has_value; ++m_enumerable) {
      m_has_value = m_block(m_value = *m_enumerable);
    }
  }
  const value_type& operator*() const { return m_value; }
};

// To standard iterator
template<typename Enum>
class Iterator : public std::iterator<std::input_iterator_tag, typename Enum::value_type> {
  Enum* m_enumerable;
  typedef typename std::iterator<std::input_iterator_tag, typename Enum::value_type> super;
public:
  typedef typename super::value_type value_type;
  typedef typename super::value_type pointer;

  Iterator() : m_enumerable(nullptr) { }
  Iterator(Enum& e) : m_enumerable(new Enum(e)) { }
  Iterator(const Enum& rhs) : m_enumerable(new Enum(*rhs.m_enumerable)) { }
  ~Iterator() { delete m_enumerable; }
  Iterator& operator=(const Enum& rhs) {
    delete m_enumerable;
    m_enumerable = new Enum(*rhs.m_enumerable);
    return *this;
  }
  template<typename Enum2>
  bool operator==(const Iterator<Enum2>& rhs) {
    return (m_enumerable && (bool)*m_enumerable) == (rhs.m_enumerable && (bool)*rhs.m_enumerable);
  }
  template<typename Enum2>
  bool operator!=(const Iterator<Enum2>& rhs) { return !(*this == rhs); }

  value_type operator*() const { return **m_enumerable; }
  pointer operator->() const { return &**m_enumerable; }
  Iterator& operator++() { ++*m_enumerable; return *this; }
};

// From iterator
template<typename Iterator>
class StdIterator : public Base<StdIterator<Iterator>, typename std::iterator_traits<Iterator>::value_type> {
  Iterator m_first, m_last;
public:
  typedef typename std::iterator_traits<Iterator>::value_type value_type;

  StdIterator(Iterator first, Iterator last) : m_first(first), m_last(last) { }
  operator bool() const { return m_first != m_last; }
  void operator++() { ++m_first; }
  value_type operator*() const { return *m_first; }
};

class IstreamLines : public Base<IstreamLines, std::string> {
  std::istream& m_is;
  std::string   m_line;
  bool          m_have_line;
public:
  typedef const std::string value_type;
  IstreamLines(std::istream& is) : m_is(is) { operator++(); }
  operator bool() const { return m_have_line; }
  void operator++() { m_have_line = std::getline(m_is, m_line); }
  value_type& operator*() const { return m_line; }
};

template<typename Enum, typename T>
Iterator<Enum> begin(Base<Enum, T>& e) { return Iterator<Enum>(*static_cast<Enum*>(&e)); }

template<typename Enum, typename T>
Iterator<Enum> end(Base<Enum, T>& e) { return Iterator<Enum>(); }


template<typename... Enums>
class Cat : public Base<Cat<Enums...>, typename std::common_type<Enums...>::type> {
  typedef typename std::common_type<Enums...>::type enum_type;
  static const size_t N = sizeof...(Enums);

  size_t    m_i;
  std::array<enum_type, N> m_enums;
public:
  typedef typename enum_type::value_type value_type;
  Cat(Enums... es) : m_i(0), m_enums({{es...}}) { }
  operator bool() const { return m_i < N && m_enums[m_i] ; }
  void operator++() {
    ++m_enums[m_i];
    while(!m_enums[m_i] && m_i < N)
      ++m_i;
  }
  value_type operator*() const { return *m_enums[m_i]; }
};

template<typename... Enums>
class Zipa : public Base<Zipa<Enums...>, typename std::common_type<Enums...>::type> {
  typedef typename std::common_type<Enums...>::type enum_type;
  typedef typename enum_type::value_type            common_value_type;
  static const size_t N = sizeof...(Enums);
public:
  typedef std::array<common_value_type, N> value_type;
  Zipa(Enums... es) : m_enums({{es...}}) { }
  operator bool() const {
    for(size_t i = 0; i < N; ++i)
      if(!m_enums[i]) return false;
    return true;
  }
  void operator++() {
    for(size_t i = 0; i < N; ++i)
      ++m_enums[i];
  }
  const value_type& operator*() const {
    for(size_t i = 0; i < N; ++i)
      m_values[i] = *(m_enums[i]);
    return m_values;
  }

protected:
  std::array<enum_type, N>          m_enums;
  mutable value_type                m_values;
};

// Helper functors for Zip
// Cast to bool and AND all elements in tuple T
template<typename T, size_t N = std::tuple_size<T>::value> struct more {
  static bool call(const T& e) { return std::get<N-1>(e) && more<T, N - 1>::call(e); }
};
template<typename T> struct more<T, 0> {
  static bool call(const T& e) { return true; }
};

// Increment with operator++ every element in tuple T
template<typename T, size_t N = std::tuple_size<T>::value> struct inc {
  static void call(T& e) {
    ++std::get<N-1>(e);
    inc<T, N-1>::call(e);
  }
};
template<typename T> struct inc<T, 0> {
  static void call(const T& e) { }
};

// Create a tuple from the results of calling operator* on the elements of tuple T
template<typename T, size_t N = std::tuple_size<T>::value, size_t... S>
struct star : public star<T, N-1, N-1, S...>
{ };

template<typename T, size_t... S>
struct star<T, 0, S...> {
  static auto call(const T& e) {
    return std::make_tuple(*std::get<S>(e)...);
  }
};

template<typename... Enums>
class Zip : public Base<Zip<Enums...>, std::tuple<typename Enums::value_type...>> {
  typedef typename std::tuple<Enums...> enum_type;
public:
  typedef std::tuple<typename Enums::value_type...> value_type;
  Zip(Enums... es) : m_enums(es...) { }
  operator bool() const { return more<enum_type>::call(m_enums); }
  void operator++() { inc<enum_type>::call(m_enums); }
  value_type operator*() const { return star<enum_type>::call(m_enums); }

 protected:
  enum_type          m_enums;
};
} // namespace imp

// Functions available directly in Enumerable namespace
template<typename T = int>
imp::Range<T> range(T start = 0, T end = std::numeric_limits<T>::max(), T step = 1) { return imp::Range<T>(start, end, step); }

template<typename T>
imp::Range<T> times(T t) { return imp::Range<T>(0, t); }

template<typename Enum, typename Block>
imp::Map<Enum, Block> map(Enum& e, Block b) { return imp::Map<Enum, Block>(e, b); }

template<typename Iterator>
imp::StdIterator<Iterator> make(Iterator first, Iterator last) { return imp::StdIterator<Iterator>(first, last); }

template<typename Container>
auto container(Container& c) -> imp::StdIterator<decltype(begin(c))> {
  return make(begin(c), end(c));
}

template<typename Container>
auto container(const Container& c) -> imp::StdIterator<decltype(begin(c))> {
  return make(begin(c), end(c));
}

imp::IstreamLines lines(std::istream& is) { return imp::IstreamLines(is);}

template<typename... Enums>
imp::Cat<Enums...> cat(Enums... es) {
  return imp::Cat<Enums...>(es...);
}

template<typename... Enums>
imp::Zipa<Enums...> zipa(Enums... es) { return imp::Zipa<Enums...>(es...); }

template<typename... Enums>
imp::Zip<Enums...> zip(Enums... es) { return imp::Zip<Enums...>(es...); }

namespace imp {
// Numeric operator become maps
template<typename Derived, typename T, typename U>
auto operator+(Base<Derived, T> e, U x) { return e.map([=](T&& y) { return y + x; }); }
// template<typename Derived, typename T, typename U>
// auto operator+(U x, Base<Derived, T> e) { return e.map([=](T&& y) { return x + y; }); }

} // namespace imp

} // namespace Enumerable



#endif /* __ENUMERABLE_H__ */
