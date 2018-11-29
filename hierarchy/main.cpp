#include <iostream>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class NullType {
};

template <typename...>
class TypeList;

template <>
class TypeList<> {
    using Head = NullType;
    using Tail = NullType;
};

template <typename T, typename... U>
class TypeList<T, U...> {
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

template <typename TList>
class GenFibonacciTypeLists {
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
using GenFibonacciHierarchy = GenScatterHierarchy<SUnit, typename GenLinearBranches<LUnit, Root, typename GenFibonacciTypeLists<TList>::Result>::Result>;

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
    using TList = TypeList<A, B, C, D, E, F>;
    using FibonacciHierarchy = GenFibonacciHierarchy<ScatterUnit, LinearUnit, NullType, TList>;
    return 0;
}
