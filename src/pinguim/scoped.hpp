#pragma once

#include <functional>
#include <exception> // For std::uncaught_exceptions.
#include <utility> // For std::move.

namespace pinguim
{
    // Calls a callback when the scope ends.
    // Is moveable but not copyable.
    // This has a few other uses beyond being a
    // RAII wrapper.
    struct scoped
    {
        template <typename Func>
        constexpr scoped(Func&& on_exit);

        /// A few constructors for special behaviours.

        // Calls f when the scope exits without an exception.
        // You shouldn't return this.
        template <typename Func>
        static constexpr auto on_noexcept(Func&& on_noexcept);

        // Calls f when the scope exits due to thrown exception.
        template <typename Func>
        static constexpr auto on_exception(Func&& f)
        {
            return scoped{ [f=std::forward<Func>(f), ue=std::uncaught_exceptions()]{
                if(ue != std::uncaught_exceptions()) { f(); }
            }};
        }

        // Make it non-copyable.
        scoped() = delete;
        scoped(const scoped&) = delete;
        auto operator=(const scoped&) = delete;

        // But make if move-able.
        inline scoped(scoped&& other);
        inline auto operator=(scoped&& other) -> scoped&;

        // 'I regret my decision' button.
        constexpr auto disarm();

        ~scoped();

    private:
        bool is_armed;
        std::function<void()> on_exit;
    };

    namespace detail
    {
        template <bool CallOnException>
        struct scoped_conditionally;
    }

    // Calls the callback IFF the scoped is exited and due
    // to a thrown exception.
    // Is unmoveable and uncopyable.
    using scoped_except   = detail::scoped_conditionally<true>;

    // Calls the callback IFF the scope is exited without
    // exceptions thrown.
    // Is unmoveable and uncopyable.
    using scoped_noexcept = detail::scoped_conditionally<false>;
}

// Implementations below

template <typename Func>
constexpr pinguim::scoped::scoped(Func&& oe)
    : is_armed{ true }
    , on_exit{ std::forward<Func>(oe) }
{}

inline pinguim::scoped::scoped(scoped&& other)
{ *this = std::move(other); }

inline auto pinguim::scoped::operator=(scoped&& other) -> scoped&
{
    is_armed = std::exchange(other.is_armed, false);
    on_exit  = std::move(other.on_exit);
    return *this;
}

constexpr auto pinguim::scoped::disarm()
{ is_armed = false; }

template <bool CallOnException>
struct pinguim::detail::scoped_conditionally
{
    template <typename Func>
    constexpr scoped_conditionally(Func&& f)
        : is_armed{true}
        , on_exit{ std::forward<Func>(f) }
    {}

    // Make it uncopyable and unmoveable.
    scoped_conditionally() = delete;
    scoped_conditionally(scoped_conditionally&&)      = delete;
    scoped_conditionally(const scoped_conditionally&) = delete;
    auto operator=(scoped_conditionally&&)      -> scoped_conditionally& = delete;
    auto operator=(const scoped_conditionally&) -> scoped_conditionally& = delete;

    constexpr auto disarm()
    { is_armed = false; }

    ~scoped_conditionally()
    {
        if(!is_armed) { return; }

        if constexpr(CallOnException) {
            if(ue != std::uncaught_exceptions()) on_exit();
        } else {
            if(ue == std::uncaught_exceptions()) on_exit();
        }
    }

private:
    bool is_armed;
    std::function<void()> on_exit;
    int ue = std::uncaught_exceptions();
};
