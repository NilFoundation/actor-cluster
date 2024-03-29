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
#include <nil/actor/utility.hpp>

#include "benchmark_utility.hpp"

#include <nil/actor/core/execution_stage.hh>

static constexpr std::size_t CreationCount = 16000000;

class create_actor : public nil::actor::actor<create_actor> {
public:
    ACTOR_DEFINE_ACTOR(create_actor, (process));
    void process() {
        auto volatile sint = std::sin(37.2);
        auto volatile res = sint * sint;
        // defeat dead code elimination
        assert(res > 0);
    }
};

static int i;    // need to be not on stack
nil::actor::future<> fork_join_create_buffered() {
    i = 0;
    return create_actor::clear_directory().then([] {
        return nil::actor::with_buffer(100, [](auto &buffer) {
            return nil::actor::do_until([] { return i >= CreationCount; },
                                        [&buffer] { return buffer(nil::actor::get<create_actor>(i++)->process()); });
        });
    });
}

int main(int ac, char **av) {
    return nil::actor::benchmark::run(ac, av, {ACTOR_BENCH(fork_join_create_buffered)}, 10);
}
