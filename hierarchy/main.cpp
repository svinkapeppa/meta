#include <algorithm>
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

/*
    This template class calculates legnth of the typelist.
    Main logic is to count the value in a recursive way.
*/

template <typename TypeList>
class Length {
public:
    constexpr static auto Value = Length<typename TypeList::Tail>::Value + 1;
};

/*
    Enables to count length of NullType.
    It's needed because of the realization of empty typelist.
*/

template <>
class Length<NullType> {
public:
    constexpr static auto Value = 0;
};

/*
    Partial specialization which sets starting point of counting.
*/

template <>
class Length<TypeList<>> {
public:
    constexpr static auto Value = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
   This template class generates Fibonacci numbers.
   Realization is quite simple.
   As a convention zeroth and first numbers of the sequence are both 1.
*/

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

/*
    This template class allows to push type (T) into typelist.
    This functionality is available because of partial template specialization.
*/

template <typename T, typename... Types>
class PushFront;

/*
    Main logic lies here.
    Constructs new typelist with new type (T) as head, and other types (Types) as tail.
*/

template <typename T, typename... Types>
class PushFront<TypeList<Types...>, T> {
public:
    using Result = TypeList<T, Types...>;
};

/*
    Partial specialiazation in case of pushing into NullType.
    As a convention it just constructs new typelist containing one type (T).
    This behaviour is often exploited through the rest of the file.
*/

template<typename T>
struct PushFront<NullType, T> {
public:
    using Result = TypeList<T>;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
   This class takes first N (index) elements of the given typelist (TList).
   Note that if N (index) is too big, then it will be reduced to length of the typelist (TList).
   So it either takes required number of types (index), or takes the whole typelist (TList).
*/

template <typename TList, int index>
class Take {
private:
    constexpr static auto Size = std::min(index, Length<TList>::Value);

public:
    using Result = typename PushFront<typename Take<typename TList::Tail, Size - 1>::Result, typename TList::Head>::Result;
};

/*
    Stoping criteria of taking elements of typelist (TList).
    Result is NullType because of the logic of PushFront class.
*/

template <typename TList>
class Take<TList, 0> {
public:
    using Result = NullType;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
   This class removes first N (index) elements of the given typelist (TList).
   Note that if N (index) is too big, then it will be reduced to length of the typelist (TList).
   So it either removes required number of types (index), or destroys the whole typelist (TList).
*/

template <typename TList, int index>
class Remove {
private:
    constexpr static auto Size = std::min(index, Length<TList>::Value);

public:
    using Result = typename Remove<typename TList::Tail, Size - 1>::Result;
};

template <typename TList>
class Remove<TList, 0> {
public:
    using Result = TList;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
    This class generates typelists, which will be than used to generate linear branches.
    Idea is following:
    1. Take N (index) types from typelist (TList);
    2. Create temporary typelist;
    3. Remove this first N (index) types from typelist (TList);
    4. Recursively repeat and then join all temporary typelists together.
*/

template <typename TList, int index = 0>
class GenFibonacciTypeLists {
private:
    using BranchTail = typename Take<typename TList::Tail, Fibonacci<index>::Value>::Result;
    using Branch = typename PushFront<BranchTail, typename TList::Head>::Result;
    using Rest = typename Remove<typename TList::Tail, Fibonacci<index>::Value>::Result;

public:
    using Result = typename PushFront<typename GenFibonacciTypeLists<Rest, index + 1>::Result, Branch>::Result;
};

template <int index>
class GenFibonacciTypeLists<TypeList<>, index> {
public:
    using Result = TypeList<TypeList<>>;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
    This class generates scatter hierarchy.
    See [1] for details.
*/

template <template<typename> typename Unit, typename... Types>
class GenScatterHierarchy;

template <template<typename> typename Unit, typename... Types>
class GenScatterHierarchy<Unit, TypeList<Types...>> : public Unit<Types>... {
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
    This class generates linear hierarchy.
    See [1] for details.
*/

template <template<typename, typename> typename Unit, typename TList, typename Root = NullType>
class GenLinearHierarchy : public Unit<typename TList::Head, GenLinearHierarchy<Unit, typename TList::Tail, Root>> {
};

template <template<typename, typename> typename Unit, typename Root>
class GenLinearHierarchy<Unit, TypeList<>, Root> : public Root {
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
    This class generates branches of the Fibonacci hierarchy.
*/

template <template <typename, typename> typename Unit, typename Root, typename... TLists>
class GenLinearBranches;

template <template <typename, typename> typename Unit, typename Root, typename... TLists>
class GenLinearBranches<Unit, Root, TypeList<TLists...>> {
public:
    using Result = TypeList<GenLinearHierarchy<Unit, TLists, Root>...>;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
    Realization of generating the Fibonacci hierarchy is the following:
    1. Create linear branches of the requested size;
    2. Make root inherit from these branches.
*/

template <template <typename> typename SUnit, template <typename, typename> typename LUnit, typename Root, typename TList>
using GenFibonacciHierarchy = GenScatterHierarchy<SUnit,
    typename GenLinearBranches<LUnit, Root, typename GenFibonacciTypeLists<TList>::Result>::Result>;

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
    Literature:
    1. Modern C++ Design: Generic Programming and Design Patterns Applied, Andrei Alexandrescu
*/
