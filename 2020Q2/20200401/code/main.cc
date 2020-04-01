#include <iostream>
#include <tuple>

void print() {}

template<typename T, typename... Types>
void print(const T& firstArg, const Types&... args) {
	std::cout << firstArg << " " << sizeof...(args) << std::endl;
	print(args...);
}

template <typename... Types>
void print(const Types&... args) {
  std::cout << "print(...)" << std::endl;
}

template <typename T>
T my_max(T value) {
  return value;
}

template <typename T, typename... Types>
T my_max(T value, Types... args) {
  return std::max(value, my_max(args...));
}

template<typename... Values> class tuple;
template<> class tuple<> {};

template<typename Head, typename... Tail>
class tuple<Head, Tail...>
  : private tuple<Tail...>
{
  typedef tuple<Tail...> inherited;
  public:
    tuple() {}
    tuple(Head v, Tail... vtail) : m_head(v), inherited(vtail...) {}
    Head& head() {return m_head;}
    inherited& tail() {return *this;}
  protected:
    Head m_head;
};

template<typename... Values> class tup;
template<> class tup<> {};

template<typename Head, typename... Tail>
class tup<Head, Tail...>
{
  typedef tup<Tail...> composited;
  public:
    tup() {}
    tup(Head v, Tail... vtail) : m_head(v), m_tail(vtail...) {}
    Head& head() {return m_head;}
    composited& tail() {return m_tail;}
  protected:
    Head m_head;
    composited m_tail;
};

int main(int argc, char *argv[]) {
	print(2, "hello", 1);

  std::cout << my_max(1, 5, 8, 4, 6) << std::endl;

	tuple<int, float, std::string> t(1, 2.3, "hello");
	std::cout << t.head() << " " << t.tail().head() << " " << t.tail().tail().head() << std::endl;
  tup<int, float, std::string> t1(1, 2.3, "hello");
  std::cout << t1.head() << " " << t1.tail().head() << " " << t1.tail().tail().head() << std::endl;

  std::tuple<int, float, std::string> t2(1, 2.3, "hello");
  std::get<0>(t2) = 4;
  std::cout << std::get<0>(t2) << " " << std::get<1>(t2) << " " << std::get<2>(t2) << std::endl;

  auto t3 = std::make_tuple(2, 3.4, "World");
  
  std::cout << std::tuple_size<decltype(t3)>::value <<std::endl;
  std::tuple_element<1, decltype(t3)>::type f = 1.2;

	return 0;
}