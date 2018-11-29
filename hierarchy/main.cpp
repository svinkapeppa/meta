#include <iostream>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class NullType {
};

template <typename...>
class TypeList;

template <>
class TypeList<> {
public:
    using Head = NullType;
    using Tail = NullType;
};

template <typename T, typename... U>
class TypeList<T, U...> {
public:
    using Head = T;
    using Tail = TypeList<U...>;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <int index>
class Fibonacci {
public:
    constexpr static auto Value = Fibonacci<index - 1>::Value + Fibonacci<index - 2>::Value;
};

template <>
class Fibonacci<0> {
public:
    constexpr static auto Value = 1;
};

template <>
class Fibonacci<1> {
public:
    constexpr static auto Value = 1;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T, typename... Types>
class PushFront;

template <typename T, typename... Types>
class PushFront<TypeList<Types...>, T> {
public:
    using Result = TypeList<T, Types...>;
};

template<typename T>
struct PushFront<NullType, T> {
public:
    using Result = TypeList<T>;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename TList, int index>
class Take {
public:
    constexpr static auto Nullable = (index == 0) || std::is_same_v<TypeList<>, TList>;
    using Result = std::conditional_t<Nullable, NullType, typename PushFront<typename Take<typename TList::Tail, index - 1>::Result, typename TList::Head>::Result>;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename TList, int index>
class Remove;

template <int index>
class Remove<TypeList<>, index> {
public:
    using Result = TypeList<>;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename TList, typename... TLists>
class Join;

template <typename TList>
class Join<TList, TypeList<TypeList<>>> {
public:
    using Result = TypeList<TList>;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename TList, int index>
class GenFibonacciTypeLists {
private:
    using BranchTail = typename Take<typename TList::Tail, Fibonacci<index>::Value>::Result;
    using Branch = typename PushFront<BranchTail, typename TList::Head>::Result;
    using Rest = typename Remove<typename TList::Tail, Fibonacci<index>::Value>::Result;

public:
    using Result = typename Join<Branch, typename GenFibonacciTypeLists<Rest, index + 1>::Result>::Result;
};

template <int index>
class GenFibonacciTypeLists<TypeList<>, index> {
public:
    using Result = TypeList<TypeList<>>;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <template<typename> typename Unit, typename... Types>
class GenScatterHierarchy;

template <template<typename> typename Unit, typename... Types>
class GenScatterHierarchy<Unit, TypeList<Types...>> : public Unit<Types>... {
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <template<typename, typename> typename Unit, typename TList, typename Root = NullType>
class GenLinearHierarchy : public Unit<typename TList::Head, GenLinearHierarchy<Unit, typename TList::Tail, Root>> {
};

template <template<typename, typename> typename Unit, typename Root>
class GenLinearHierarchy<Unit, TypeList<>, Root> : public Root {
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <template <typename, typename> typename Unit, typename Root, typename... TLists>
class GenLinearBranches;

template <template <typename, typename> typename Unit, typename Root, typename... TLists>
class GenLinearBranches<Unit, Root, TypeList<TLists...>> {
public:
    using Result = TypeList<GenLinearHierarchy<Unit, TLists, Root>...>;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <template <typename> typename SUnit, template <typename, typename> typename LUnit, typename Root, typename TList>
using GenFibonacciHierarchy = GenScatterHierarchy<SUnit, typename GenLinearBranches<LUnit, Root, typename GenFibonacciTypeLists<TList, 0>::Result>::Result>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
class ScatterUnit {
};

template <typename T, typename U>
class LinearUnit : public U {
};

class A {
};

class B {
};

class C {
};

class D {
};

class E {
};

class F {
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main() {
    using TList = TypeList<A>;
    using FibonacciHierarchy = GenFibonacciHierarchy<ScatterUnit, LinearUnit, NullType, TList>;
    return 0;
}
