#pragma once

#include "pinguim/standalone/forward.hpp"
#include "pinguim/container.hpp"
#include "pinguim/aliases.hpp"

// Both <initializer_list> and <new> are quite
// fast to include and codegen so it should be fine.
#include <initializer_list>
#include <new>

namespace pinguim
{
    template <typename T>
    class list
    {
    public:
        constexpr list() = default;
        constexpr list(u64);

        // Copy constructors.
        constexpr list(list const&);
        constexpr list(T* begin, T* end); // Assumes begin != end.
        static constexpr auto from_container(container<T> const&) -> list; // TODO

        // Move constructors.
        constexpr list(list&&) noexcept;
        static constexpr auto from_container(container<T>&&) noexcept -> list;

        // Elementwise move constructors.
        constexpr list(std::initializer_list<T>);

        ~list();

        constexpr auto operator=(list const&) -> list&;
        constexpr auto operator=(list&&) noexcept -> list&;

        constexpr auto operator[](u64)       -> T&;
        constexpr auto operator[](u64) const -> T const&;

        [[nodiscard]] constexpr auto size()     const -> u64;
        [[nodiscard]] constexpr auto capacity() const -> u64;

        [[nodiscard]] constexpr auto begin() -> T*;
        [[nodiscard]] constexpr auto end()   -> T*;
        [[nodiscard]] constexpr auto begin() const -> T const*;
        [[nodiscard]] constexpr auto end()   const -> T const*;

        constexpr auto reserve(u64) -> list&;

        constexpr auto push_back(T const&)     -> list&;
        constexpr auto push_back(T&&)          -> list&;
        template <typename... Args>
        constexpr auto emplace_back(Args&&...) -> list&;

        [[nodiscard]] constexpr auto surrender() noexcept -> container<T>;

        // Capacity remains unchanged.
        constexpr auto clear() -> list&;

    private:
        container<T> elements = {nullptr, 0, 0};
    };
}

template <typename T>
constexpr pinguim::list<T>::list(u64 size)
{
    elements.data = static_cast<T*>(::operator new(size * sizeof(T)));
    elements.capacity = size;
}

template <typename T>
constexpr pinguim::list<T>::list(const list& other)
{ *this = other; }

template <typename T>
constexpr pinguim::list<T>::list(T* begin, T* end)
{
    reserve( static_cast<u64>(end - begin + 1) );
    for(auto e = begin; e != end; ++e) push_back(*e);
}

template <typename T>
constexpr auto pinguim::list<T>::from_container(container<T> const& container) -> list
{
    auto l = list(container.size);
    for(auto i = 0_u64; i < container.size; ++i) l.push_back(container.data[i]);
    return l;
}

template <typename T>
constexpr pinguim::list<T>::list(list&& other) noexcept
{ *this = s::move(other); }

template <typename T>
constexpr auto pinguim::list<T>::from_container(container<T>&& container) noexcept -> list
{
    auto l = list();
    l.elements = s::move(container);
    container = {nullptr, 0, 0};
    return l;
}

template <typename T>
constexpr pinguim::list<T>::list(std::initializer_list<T> l)
{
    reserve( l.size() );
    for(auto& e : l) push_back( s::move(e) );
}

template <typename T>
pinguim::list<T>::~list<T>()
{
    clear();
    if(elements.data) delete elements.data;
}

template <typename T>
constexpr auto pinguim::list<T>::operator=(const list& other) -> list&
{
    clear();
    reserve(other.elements.size);

    for(auto const& e : other) push_back(e);

    return *this;
}

template <typename T>
constexpr auto pinguim::list<T>::operator=(list&& other) noexcept -> list&
{
    this->~list();
    elements = s::move(other.elements);
    other.elements = {nullptr, 0, 0};

    return *this;
}

template <typename T>
constexpr auto pinguim::list<T>::operator[](u64 pos) -> T&
{ return elements.data[pos]; }

template <typename T>
constexpr auto pinguim::list<T>::operator[](u64 pos) const -> T const&
{ return elements.data[pos]; }

template <typename T>
constexpr auto pinguim::list<T>::size() const -> u64
{ return elements.size; }

template <typename T>
constexpr auto pinguim::list<T>::capacity() const -> u64
{ return elements.capacity; }

template <typename T>
constexpr auto pinguim::list<T>::begin() -> T*
{ return elements.data; }

template <typename T>
constexpr auto pinguim::list<T>::end() -> T*
{ return elements.data + elements.size; }

template <typename T>
constexpr auto pinguim::list<T>::begin() const -> T const*
{ return elements.data; }

template <typename T>
constexpr auto pinguim::list<T>::end() const -> T const*
{ return elements.data + elements.size; }

template <typename T>
constexpr auto pinguim::list<T>::reserve(u64 new_size) -> list&
{
    if(new_size <= elements.capacity) return *this;

    auto new_list = list(new_size);
    for(auto& e : *this) new_list.push_back( s::move(e) );
    *this = s::move(new_list);

    return *this;
}

template <typename T>
constexpr auto pinguim::list<T>::push_back(T const& new_element) -> list&
{
    if(elements.size == elements.capacity) reserve(elements.capacity ? elements.capacity * 2 : 1);
    auto e = new ( elements.data + elements.size++ ) T;
    *e = new_element;
    return *this;
}

template <typename T>
constexpr auto pinguim::list<T>::push_back(T&& new_element) -> list&
{
    if(elements.size == elements.capacity) reserve(elements.capacity ? elements.capacity * 2 : 1);
    auto e = new ( elements.data + elements.size++ ) T;
    *e = s::move(new_element);
    return *this;
}

template <typename T>
template <typename... Args>
constexpr auto pinguim::list<T>::emplace_back(Args&&... args) -> list&
{
    if(elements.size == elements.capacity) reserve(elements.capacity ? elements.capacity * 2 : 1);
    new ( elements.data + elements.size++ ) T( s::forward<Args>(args)... );
    return *this;
}

template <typename T>
constexpr auto pinguim::list<T>::surrender() noexcept -> container<T>
{
    auto const ret = elements;
    elements = {nullptr, 0, 0};
    return ret;
}

template <typename T>
constexpr auto pinguim::list<T>::clear() -> list&
{
    for(auto& e : *this) e.~T();
    elements.size = 0;
    return *this;
}
