# Affinity Patch for the C++ Actor Framework

The affinity patch for CAF is a fork of the [C++ Actor Framework](https://github.com/actor-framework/actor-framework) that allows the use of the thread to core affinity feature of Linux and Windows systems.
The thread to core affinity makes it possible to bind CAF threads to a specific subset of available cores) to take advantage of memory locality.

**NOTE:** We do not support versioning. We always try to push the affinity patch on the last available version of CAF. In order to be sure which CAF version you are using check the `CAF_VERSION` macro.

## How to use the affinity

The affinity patch provides two ways to set threads to core affinity.
Thought configurations parameters or programmatically via the `affinity_manager`.

### Configuration parameters

The patch permits to set different affinity configuration for individual CAF thread type (i.e. runtime threads, detached actor, blocking actor, and other caf threads).
The affinity can be set by using the following configuration parameters in the `affinity` group:

- `affinity.worker-cores`
- `affinity.detached-cores`
- `affinity.blocking-cores`
- `affinity.other-cores`

Each parameter has to provide an *affinity string* that specifies where each thread of a given type is allowed to run.
In particular, the *affinity string* is composed by a set of groups enclosed in angle brackets `< >`.
A group hosts a collection of cores separated by commas `,` or a range of them delimited with a single dash `-`.

For example, the *affinity string* `"<0> <2-4> <1,5>"` allows placing the first thread spawned by CAF on the core with id 0, the second thread on cores 2, 3 and 4, and the third thread on cores 1 and 5.
The next thread spawned will be placed again on the first group, i.e. core 0.

### Affinity Manager

The affinity manager provides the public method `set_actor_affinity` that can be used to move a detached actor to a specific set of cores.
Note that a detached actor can also be moved multiple times and the last call overrides the previous affinity configurations.

This is a simple example in which a detached actor is spawned and then moved to core 3 and 4.

```(c++)
  caf::actor det1 = system.spawn<caf::detached>(actor1);
  auto& aff_manager = system.affinity_manager();
  aff_manager.set_actor_affinity(det1, std::set<int>{3,4});
```

For more information check the example `examples/affinity_example.cpp`.

## Simple example

The following command runs a CAF program that confines the CAF runtime thread in the first 4 cores of the CPU (from 0 to 3) and the blocking and detached actors in the next 4 cores (from 4 to 7).

```(bash)
my_program --scheduler.worker-threads=4
           --affinity.worker-cores="<0-3>"
           --affinity.detached-cores="<4-7>"
```

This resource partitioning avoids that other CAF threads interrupt the CAF runtime threads. Thus the event-based actor can be more reactive providing a reduce message latency. 

## Installation and Documentation

See the original CAF [repository](https://github.com/actor-framework/actor-framework) and [documentation](https://actor-framework.readthedocs.io/en/stable/).

## License and Acknowledgement

This software and the modifications are distributed with the CAF [LICENSE](./LICENSE).

We thanks the CAF developer and contributors for the awesome work and to give us the possibility to extend it with our new features.
