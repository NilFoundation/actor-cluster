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

class fibonacci_actor : public nil::actor::actor<fibonacci_actor> {
public:
    ACTOR_DEFINE_ACTOR(fibonacci_actor, (fib));

    int result = -1;

    nil::actor::future<int> fib() {
        if (key <= 2) {
            result = 1;
            return nil::actor::make_ready_future<int>(result);
        } else if (result > 0) {
            return nil::actor::make_ready_future<int>(result);
        } else {
            auto f1 = nil::actor::get<fibonacci_actor>(key - 1)->fib();
            auto f2 = nil::actor::get<fibonacci_actor>(key - 2)->fib();
            return nil::actor::when_all_succeed(std::move(f1), std::move(f2)).then([this](auto r1, auto r2) {
                return result = r1 + r2;
            });
        }
    }
};

auto fib() {
    return fibonacci_actor::clear_directory().then(
        [] { return nil::actor::get<fibonacci_actor>(36)->fib().discard_result(); });
}

int main(int ac, char **av) {
    return nil::actor::benchmark::run(ac, av,
                                      {
                                          ACTOR_BENCH(fib),
                                      },
                                      10);
}
