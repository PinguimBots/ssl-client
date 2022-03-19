#include "pinguim/renum.hpp"

#include <string_view>

enum class test_enum { start, stop, pause };

namespace namespaced {
    enum class test_enum { start, stop, pause };

    template<typename T>
    struct templated_struct { enum class test_enum{ start, stop, pause }; };
}

// Clang only produces the enum values if the containing
// structs are instanced (if they are templated).
template struct namespaced::templated_struct<void>;

int main()
{
    // If detail::enum_value_view works its safe to say everything else
    // works too.

    constexpr auto namespaced = pinguim::renum::detail::enum_value_view<namespaced::test_enum, namespaced::test_enum(2)>();
    static_assert(namespaced.qualified()      == "namespaced::test_enum::pause");
    static_assert(namespaced.semi_qualified() == "test_enum::pause");
    static_assert(namespaced.unqualified()    == "pause");

    constexpr auto bare = pinguim::renum::detail::enum_value_view<test_enum, test_enum(2)>();
    static_assert(bare.qualified()      == "test_enum::pause");
    static_assert(bare.semi_qualified() == "test_enum::pause");
    static_assert(bare.unqualified()    == "pause");

    using ntte = namespaced::templated_struct<void>::test_enum;
    constexpr auto templated = pinguim::renum::detail::enum_value_view<ntte, ntte(2)>();
    static_assert(templated.qualified()      == "namespaced::templated_struct<void>::test_enum::pause");
    static_assert(templated.semi_qualified() == "test_enum::pause");
    static_assert(templated.unqualified()    == "pause");

    constexpr auto bad = pinguim::renum::detail::enum_value_view<ntte, ntte(100)>();
    static_assert(bad.begin == nullptr);
    static_assert(bad.len == 0);

    return 0;
}
