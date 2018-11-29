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

template <template<typename...> typename Unit, typename Types>
class GenScatterHierarchy;

template <template<typename...> typename Unit, typename... Types>
class GenScatterHierarchy<Unit, TypeList<Types...>> : public Unit<Types>... {
public:
    using Result = TypeList<Unit<Types>...>;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <template<typename...> typename Unit, typename TList, typename Root = NullType>
class GenLinearHierarchy : public Unit<typename TList::Head, GenLinearHierarchy<Unit, typename TList::Tail, Root>> {
};

template <template<typename...> typename Unit, typename Root>
class GenLinearHierarchy<Unit, TypeList<>, Root> : public Unit<Root> {
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
class Holder {
public:
    T value;
};

class A {
};

class B {
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main() {
    using Root = GenScatterHierarchy<Holder, TypeList<A, int, long, B>>;
    using Tail = GenLinearHierarchy<Holder, TypeList<A, int, long, B>>;
    return 0;
}
