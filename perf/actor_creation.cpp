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

#include <nil/actor/actor.hpp>
#include <nil/actor/actor_ref.hpp>

#include "benchmark_utility.hpp"

#include <nil/actor/core/future-util.hh>

class skynet_singleton_actor : public nil::actor::actor<skynet_singleton_actor> {
public:
    ACTOR_DEFINE_ACTOR(skynet_singleton_actor, (noop));

    void noop() const {
    }
};

class skynet_local_actor : public nil::actor::actor<skynet_local_actor>,
                           public nil::actor::local_actor<skynet_local_actor> {
public:
    ACTOR_DEFINE_ACTOR(skynet_local_actor, (noop));

    void noop() const {
    }
};

template<typename Type>
auto create_actors() {
    constexpr auto create = 10000;

    return nil::actor::parallel_for_each(boost::irange(0, create),
                                         [](int j) { return nil::actor::get<Type>(j)->noop(); });
}

int main(int ac, char **av) {
    return nil::actor::benchmark::run(
        ac, av, {ACTOR_BENCH(create_actors<skynet_singleton_actor>), ACTOR_BENCH(create_actors<skynet_local_actor>)},
        100);
}
