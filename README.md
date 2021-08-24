# =nil; Actor Virtual Actor Model Library

=nil; Actor is a lightweight modern actor library built on top of
the [=nil; Actor.Core Library](https://github.com/nilfoundation/actor-core). It helps writing distributed applications
using virtual actors. It allows developers to write highly scalable applications while greatly simplifying discovery,
state management, actor lifetime and more.

The [Virtual Actor Model](http://research.microsoft.com/apps/pubs/default.aspx?id=210931) is a variation of
the [Actor Model](https://en.wikipedia.org/wiki/Actor_model) where "virtual" actors (transcendental entities combining
state and execution context) share and process information by exchanging messages.

It is heavily inspired by the [Microsoft Orleans](https://dotnet.github.io/orleans/Documentation/index.html) project.

# Code Example

First we need to define an `actor`:

```cpp
class hello_actor : public nil::actor::actor<hello_actor> {
public:
    using KeyType = std::string;

    nil::actor::future<> say_hello() const {
        nil::actor::print("Hello, %s.\n", key);
        return nil::actor::make_ready_future();
    }

    ACTOR_DEFINE_ACTOR(hello_actor, (say_hello));
};
```

And then call the actor activation from anywhere in your actor code using
an `actor_ref`:

```cpp
auto ref = nil::actor::get<hello_actor>("actor");
auto future = ref->say_hello();
// wait or attach a continuation to the returned future.
```

# Performance

=nil; Actor is built on Actor and benefits from a
lock-free, shared-nothing design. Compared to typical actor model implementations,
it doesn't use any locking or complex cache-unfriendly concurrent data-structures internally.

Specifically, this gives =nil; Actor an advantage on many-to-many communication patterns, because there is no contention
on mailboxes. Also, because =nil; Actor doesn't have per-actor mailboxes and actor' messages aren't processed in
batches, it has better latency characteristics.

# Going forward

=nil; Actor is a small project and currently lacks:

- [ ] **Clustering.** An actor system on a local machine is nice, but the concept shines when actors are allowed to
  migrate across a set of clustered machines freely.
- [ ] **Persistence.** Actors could write their states in storage or non-volatile memory to provide failure-safety.
- [ ] **Steaming.** Message-passing doesn't always map well to domain-specific problems. A streaming API would be nice
  to have.
- [ ] **Actor as heterogeneous compute abstraction.** Actors are useful to abstract over hardware because the
  abstraction's requirements are very loose. [CAF](https://actor-framework.org/) has demonstrated that actors work well
  to abstract a GPU.

# License

This project is licensed under the [MIT license](https://github.com/nilfoundation/actor-cluster/blob/master/LICENSE).
