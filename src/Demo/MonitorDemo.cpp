#include "Demo/MonitorDemo.h"

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
    sleep(5); // sleep some time to ensure all other nodes are running
    if (identity != "alpha") {
        for (int i = 0; i < 10; i++) { // put some numbers into shared vector to simulate work
            waitRandomMilisenconds(1000, 5000);
            monitor->emplace_back(::getpid());
        }
        auto lockedMonitor = monitor.manuallyLock();
        lockedMonitor.signal();
    } else {
        {
            auto lockedMonitor = monitor.manuallyLock();
            while (lockedMonitor->size() < 30) { // wait until other nodes are done with their work
                lockedMonitor.wait();
            }
            auto vector = *monitor.getStateUnsafe().get();
            for (auto &&item : vector) {
                std::cout << item << " ";
            }
            std::cout << std::endl;
            std::flush(std::cout);
        }
        messenger->shutdown(); // send shutdown signal to other nodes
    }
    messenger->joinReceiver();
}