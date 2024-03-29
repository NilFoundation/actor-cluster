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
#include <nil/actor/core/execution_stage.hh>
#include <nil/actor/utility.hpp>
#include "benchmark_utility.hpp"

static constexpr std::size_t RingSize = 1000000;
static constexpr std::size_t MessageCount = 1000000;

template<std::size_t MaxContiguousSegmentPerShard>
struct contiguous_placement_strategy {
    nil::actor::shard_id operator()(std::size_t hash) const noexcept {
        return (std::size_t)(hash * (nil::actor::smp::count / (double)MaxContiguousSegmentPerShard)) %
               nil::actor::smp::count;
    }
};

class thread_ring_actor : public nil::actor::actor<thread_ring_actor, contiguous_placement_strategy<25000UL>> {

public:
    ACTOR_DEFINE_ACTOR(thread_ring_actor, (ping));
    nil::actor::actor_id next = (key + 1) % RingSize;

    nil::actor::future<> ping(int remaining) {
        if (remaining > 0) {
            return nil::actor::get<thread_ring_actor>(next)->ping(remaining - 1);
        }
        return nil::actor::make_ready_future();
    }
};

nil::actor::future<> thread_ring() {
    return thread_ring_actor::clear_directory().then(
        [] { return nil::actor::get<thread_ring_actor>(0)->ping(MessageCount); });
}

int main(int ac, char **av) {
    return nil::actor::benchmark::run(ac, av, {ACTOR_BENCH(thread_ring)}, 10);
}
