# Distributed Monitor

## Summary

This repository contains a C++14 implementation of a [monitor](https://en.wikipedia.org/wiki/Monitor_(synchronization)) extended for use in distributed systems. For that purpose two synchronization mechanisms were created: distributed mutex based on [Suzuki-Kasami algorithm](https://en.wikipedia.org/wiki/Suzuki%E2%80%93Kasami_algorithm) and distributed conditional variable using simple "wait-for-reply" pattern. Distributed monitor can be used to safe guard access to state shared across multiple nodes and to synchronize said state after every change.

Example usage of those functions is shown in `Demo/MonitorDemo`.

ZMQ was used as a communication channel.

## How to build

To build a project go to project main directory and use:

```bash
cmake .
make
```

## Configuration

Program uses configuration provided in `peers.config`. File should contain a line for every node in the network. Line should contain unique node identity (name) and a pair of ip and port the peer node is running at: `<node_identity> <ip>:<port>`. For example:

```plaintext
alpha  127.0.0.1:5555
beta   127.0.0.1:5556
```

## Running the example

Example program takes up to 3 arguments. First argument is configuration file. Second is a node identity in configuration. Third parameter is optional and is used to turn on additional debug logging. If present it should read "DEBUG".

```bash
    ./DistributedMonitor <config_file> <node_identity> (DEBUG)
e.g ./DistributedMonitor peers.config alpha
```

Helper script can be used to run nodes on the same machine:

```bash
     ./runHelper.sh <config_file>
e.g. ./runHelper.sh peers.config
```

## How to use

### Monitor

**Note: all UUIDs can be created using either `sole::uuid0()` or `sole::rebuild(string)`**

In order to create and use Distributed Monitor first thing you need to do is provide it with type of the protected state and the serializer for said state (which extends `Serializer`). Constructor takes 2 arguments. First is an object of `Messenger` class used to comunicate with peers. Second is a UUID of the monitor. It is used to distinguish between different monitors running on the same network. For the same monitor UUID should be the same in all nodes.

```cpp
     DistributedMonitor<State, StateSerializer> monitor(Messenger messenger, UUID monitorUUID, ...);

e.g. DistributedMonitor<std::vector<int>, IntVectorSerializer> monitor(messenger, monitorUUID);
```

Upon monitor creation underlying state is also created. Every parameter after second will be passed to `State` constructor.

Arrow operator (`->`) is used to access the state inside a monitor. Every access to state will be mutualy exclusive across all nodes and will cause the state to synchronize.

```cpp
// mutex is acquired
monitor->push_back(112);
// mutex is released and state is synchronized
```

If you want to retain ownership for a longer period you can use `manuallyLock()` method. This method will return a wrapper. State is locked until as long as it is in scope. You can call `wait()`, `signal()` and `signalAll()` on the wrapper to respectively wait on the monitor, wake one or wake all nodes.

```cpp
// mutex is acquired
auto lockedMonitor = monitor.manuallyLock();
lockedMonitor.signal();
int number = lockedMonitor->at(0);
monitor->push_back(1);
// mutex is stil locked and will be released when lockedMonitor goes out of scope
```

### Mutex

You can also use mutex separetly from the monitor. It takes the same two arguments as the monitor: messenger and mutex UUID. The mutex can be either locked to acquire exclusive access to critial section or unlocked to allow access by some other node. If a node want to lock mutex which is already claimed by some other node it will be stopped until the mutex is unlocked.

```cpp
DistributedMutex(Messenger messenger, UUID mutexUUID);

e.g.
DistributedMutex dMutex(messenger, mutexUUID);
dMutex->lock();
// critical section code
dMutex->unlock();
```

### Conditional variable

Compared to previous mechanisms conditional variable takes additional parameter being `DistributedMutex` instance the variable is based on. Standard functions can be used: `wait()` to wait for wake up signal from other nodes, `signal()` to wake one of currently waiting nodes or `signalAll()` to wake up all waiting nodes. After a node is woke up associated mutex will be automatically reacquired.

```cpp
DistributedConditionalVariable>(Messenger messenger, UUID cvUUID, DistributedMutex distributedMutex);

e.g.
DistributedConditionalVariable cv(messenger, cvUUID, distributedMutex);

// mutex should be locked before call to wait();
distributedMutex.lock();
while (!someCondition) {
    // wait until other node calls signal on this conditional variable
    // undelying mutex is unlocked
    cv.wait();
    // after wait returns mutex is once again locked
}
// do some work
// wake up other node
cv.signal();
// unlock the mutex so other node can get it after it wakes up
distributedMutex.unlock();
```

## How it works

Mutual exclusion is achived using the Suzuki-Kasami algorithm. Explanation would be a bit longish so please check the original paper or the wikipedia.

Internaly `ConditionalVariable` class stores queue of nodes waiting on this variable. When `wait()` is called node sends a blocking broadcast to notify peers it is waiting. When all other nodes respond with acknowledge underlying mutex will be unlocked and node will suspend. When `signal()` or `signalAll()` is used all other nodes are notified about which nodes will wake up and will pop them of the local queue. Calling node waits for acknowlegment from other nodes and will proceed to wake up either first or all waiting nodes.

Every access to the protected state requires acquiring lock on a distributed mutex associated with the monitor thus preventing simultaneous access to the state. After every access to the protected state or if `LockedMonitor` object goes out of scope changes will be send to all peers. Mutex will not be released until peers confirm new state was received and updated.

## Mentions

I want to thanks the creators of [sole](https://github.com/r-lyeh-archived/sole) library which is used across the project to handle UUIDs.
