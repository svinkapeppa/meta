#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class NullType {};

template <typename ...> struct TypeList;

template<>
struct TypeList<> {
    using Head = NullType;
    using Tail = NullType;
};

template<typename T, typename ... U>
struct TypeList<T, U ...> {
    using Head = T;
    using Tail = TypeList<U ...>;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Types> struct PushFront;

template<typename T, typename ... Types>
struct PushFront<TypeList<Types ...>, T> {
    using Result = TypeList<T, Types ...>;
};

template<typename T>
struct PushFront<NullType, T> {
    using Result = TypeList<T>;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename TypeList>
struct Length {
    constexpr static auto Value = Length<typename TypeList::Tail>::Value + 1;
};

template<>
struct Length<NullType> {
    constexpr static auto Value = 0;
};

template<>
struct Length<TypeList<>> {
    constexpr static auto Value = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename TypeList, typename T> struct Erase;

template<typename T>
struct Erase<TypeList<>, T> {
    using Result = NullType;
};

template<typename ... Tail, typename T>
struct Erase<TypeList<T, Tail ...>, T> {
    using Result = typename TypeList<T, Tail ...>::Tail;
};

template<typename Head, typename ... Tail, typename T>
struct Erase<TypeList<Head, Tail ...>, T> {
    using _ = typename Erase<typename TypeList<Head, Tail ...>::Tail, T>::Result;
    using Result = std::conditional_t<std::is_same_v<_, NullType>,
        TypeList<Head>, typename PushFront<_, Head>::Result>;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class A {};
class B {};
class C {};
class D {};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main() {
    static_assert(Length<NullType>::Value == 0, "!");
    static_assert(Length<TypeList<>>::Value == 0, "!");
    static_assert(Length<TypeList<NullType>>::Value == 1, "!");
    static_assert(Length<TypeList<A>>::Value == 1, "!");
    static_assert(Length<TypeList<A, B>>::Value == 2, "!");
    static_assert(Length<TypeList<A, B, C>>::Value == 3, "!");
    static_assert(Length<TypeList<A, B, C, D>>::Value == 4, "!");
    static_assert(std::is_same_v<NullType, Erase<TypeList<>, A>::Result>, "!");
    static_assert(std::is_same_v<TypeList<>, Erase<TypeList<A>, A>::Result>, "!");
    static_assert(std::is_same_v<TypeList<A>, Erase<TypeList<A, B>, B>::Result>, "!");
    static_assert(std::is_same_v<TypeList<A>, Erase<TypeList<B, A>, B>::Result>, "!");
    static_assert(std::is_same_v<TypeList<A, B>, Erase<TypeList<C, A, B>, C>::Result>, "!");
    static_assert(std::is_same_v<TypeList<A, B>, Erase<TypeList<A, C, B>, C>::Result>, "!");
    static_assert(std::is_same_v<TypeList<A, B>, Erase<TypeList<A, B, C>, C>::Result>, "!");
    static_assert(std::is_same_v<TypeList<A, B, C>, Erase<TypeList<D, A, B, C>, D>::Result>, "!");
    static_assert(std::is_same_v<TypeList<A, B, C>, Erase<TypeList<A, D, B, C>, D>::Result>, "!");
    static_assert(std::is_same_v<TypeList<A, B, C>, Erase<TypeList<A, B, D, C>, D>::Result>, "!");
    static_assert(std::is_same_v<TypeList<A, B, C>, Erase<TypeList<A, B, C, D>, D>::Result>, "!");
    return 0;
}
