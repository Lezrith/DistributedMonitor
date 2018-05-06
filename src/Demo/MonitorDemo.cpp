#include "Demo/MonitorDemo.h"

#define N 10

void waitRandomMilisenconds(int from, int to) {
    std::mt19937_64 eng{std::random_device{}()};
    std::uniform_int_distribution<> dist{from, to};
    std::this_thread::sleep_for(std::chrono::milliseconds{dist(eng)});
}

std::map<std::string, std::string> readConfiguration(const std::string &path) {
    std::map<std::string, std::string> peers;
    std::ifstream file(path);
    std::string name, address;
    while (file >> name >> address) {
        address.insert(0, "tcp://");
        peers.insert(std::pair<std::string, std::string>(name, address));
    }
    return peers;
};

void monitorDemo(const std::string &configFile, const std::string identity) {
    zmq::context_t context(1);
    auto peers = readConfiguration(configFile);
    std::pair<std::string, std::string> selfConfig = *(peers.find(identity));

    auto messenger = std::make_shared<Messenger>(context, selfConfig, peers);

    auto monitorUUID = sole::rebuild("36073ca4-6681-446f-beae-5155faf993c0");
    DistributedMonitor<std::vector<int>, IntVectorSerializer> monitor(messenger, monitorUUID);

    messenger->listen(); // start the receiver thread
    LoggerSingleton::getInstance()->log(NORMAL, "Receiver started listening for messages");
    sleep(5); // sleep some time to ensure all other nodes are running
    if (identity != "alpha") {
        LoggerSingleton::getInstance()->log(NORMAL, "Work started");
        for (int i = 0; i < N; i++) { // put some numbers into shared vector to simulate work
            waitRandomMilisenconds(1000, 5000);
            monitor->emplace_back(::getpid());
            LoggerSingleton::getInstance()->log(NORMAL, "Placed number in shared vector");
        }
        LoggerSingleton::getInstance()->log(NORMAL, "Work is finished");
        auto lockedMonitor = monitor.manuallyLock();
        lockedMonitor.signal();
        LoggerSingleton::getInstance()->log(NORMAL, "Signaled other nodes that work is finished");
    } else {
        {
            auto lockedMonitor = monitor.manuallyLock();
            unsigned long finalSize = (peers.size() - 1) * N;
            while (lockedMonitor->size() < finalSize) { // wait until other nodes are done with their work
                LoggerSingleton::getInstance()->log(NORMAL, "Waiting for others to finish");
                lockedMonitor.wait();
                LoggerSingleton::getInstance()->log(NORMAL, "Woke up - checking condition");
            }
            LoggerSingleton::getInstance()->log(NORMAL, "All other nodes finished work");
            auto vector = *monitor.getStateUnsafe().get();
            std::stringstream ss;
            for (auto &&item : vector) {
                ss << item << " ";
            }
            LoggerSingleton::getInstance()->log(NORMAL, "Final vector: " + ss.str());
        }
        LoggerSingleton::getInstance()->log(NORMAL, "Sending shutdown signal to all nodes");
        messenger->shutdown(); // send shutdown signal to other nodes
    }
    messenger->joinReceiver();
}