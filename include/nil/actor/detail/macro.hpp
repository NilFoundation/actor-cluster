//---------------------------------------------------------------------------//
// Copyright (c) 2018-2021 Mikhail Komarov <nemo@nil.foundation>
//
// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//---------------------------------------------------------------------------//

#pragma once

#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/hana.hpp>

#include <nil/actor/core/future.hh>

#include <nil/actor/detail/message_identifier.hpp>

/// \exclude
#define ACTOR_LITERAL(lit) #lit

#define ACTOR_MAKE_IDENTITY(actor, handler)                                                    \
    boost::hana::uint<nil::actor::detail::crc32(ACTOR_LITERAL(actor::handler),                \
                                                 sizeof(ACTOR_LITERAL(actor::handler)) - 1)> { \
    }

/// \exclude
#define ACTOR_MAKE_TAG(a, data, i, tag)        \
    static constexpr auto tag() {                    \
        return ACTOR_MAKE_IDENTITY(data, tag); \
    }

/// \exclude
#define ACTOR_MAKE_TAG_ALT(a, data, i, tag)                                                                 \
    template<typename... Args, typename T = data>                                                                 \
    inline constexpr nil::actor::futurize_t<std::result_of_t<decltype (&T::tag)(T, Args...)>> tag(Args &&...args) \
        const {                                                                                                   \
        return ref.tell(ACTOR_MAKE_IDENTITY(data, tag), std::forward<Args>(args)...);                       \
    }

/// \exclude
#define ACTOR_MAKE_TUPLE(a, data, i, name) \
    boost::hana::make_pair(ACTOR_MAKE_IDENTITY(data, name), &data::name),

/// \exclude
#define ACTOR_MAKE_VTABLE(name, seq)                                                                         \
    static constexpr auto make_vtable() {                                                                          \
        return boost::hana::make_map(BOOST_PP_SEQ_FOR_EACH_I(ACTOR_MAKE_TUPLE, name, seq)                    \
                                         boost::hana::make_pair(BOOST_HANA_STRING("nil::actor_dummy"), nullptr)); \
    }

#ifdef ACTOR_REMOTE
#include <nil/actor/cluster/detail/macro.hpp>
#else
/// \exclude
#define ACTOR_REMOTE_MAKE_VTABLE(name, seq)
#endif

/// \brief Expands with enclosing actor internal definitions
///
/// Example:
/// ```cpp
/// class simple_actor : public nil::actor::actor<simple_actor> {
/// public:
///     nil::actor::future<> my_message() const;
///     nil::actor::future<> another_message() const;
///
///     ACTOR_DEFINE_ACTOR(simple_actor, (my_message)(another_message));
/// };
/// ```
/// \unique_name ACTOR_DEFINE_ACTOR
/// \requires `name` shall be a [nil::actor::actor]() derived type
/// \requires `seq` shall be a sequence of zero or more message handler (Example: `(handler1)(handler2)`)
#define ACTOR_DEFINE_ACTOR(name, seq)                                                                         \
private:                                                                                                            \
    KeyType key;                                                                                                    \
                                                                                                                    \
public:                                                                                                             \
    explicit name(KeyType &&key) noexcept : key(std::move(key)) {                                                   \
    }                                                                                                               \
    struct internal {                                                                                               \
        template<typename Ref>                                                                                      \
        struct interface {                                                                                          \
            static_assert(std::is_same_v<typename Ref::ActorType, name>, "actor_ref used with invalid actor type"); \
            Ref const &ref;                                                                                         \
            explicit interface(Ref const &ref) : ref(ref) {                                                         \
            }                                                                                                       \
            explicit interface(interface const &) = delete;                                                         \
            explicit interface(interface &&) = delete;                                                              \
            BOOST_PP_SEQ_FOR_EACH_I(ACTOR_MAKE_TAG_ALT, name, seq)                                            \
            constexpr auto operator->() {                                                                           \
                return this;                                                                                        \
            }                                                                                                       \
        };                                                                                                          \
        struct message {                                                                                            \
            BOOST_PP_SEQ_FOR_EACH_I(ACTOR_MAKE_TAG, name, seq)                                                \
        private:                                                                                                    \
            friend class nil::actor::detail::vtable<name>;                                                         \
            ACTOR_MAKE_VTABLE(name, seq)                                                                      \
            ACTOR_REMOTE_MAKE_VTABLE(name, seq)                                                               \
        };                                                                                                          \
    };                                                                                                              \
    using message = detail::message; /* FIXME: workaround */\

