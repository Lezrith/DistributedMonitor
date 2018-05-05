#include <Core/SafeSocket.h>
#include <Messages/PrivilegeMessage.h>
#include <Core/DistributedConditionalVariable.h>
#include <Demo/IntVectorSerializer.h>
#include "Core/DistributedMonitor.h"
#include "main.h"

std::map<std::string, std::string> readConfiguration(std::string &path) {
    std::map<std::string, std::string> peers;
    std::ifstream file(path);
    std::string name, address;
    while (file >> name >> address) {
        address.insert(0, "tcp://");
        peers.insert(std::pair<std::string, std::string>(name, address));
    }
    return peers;
};

int main(int argc, char *argv[]) {
    std::string configPath = "peers.config";

    try {
        if (argc < 2) {
            return -1;
        }

        std::string name = std::string(argv[1]);
        zmq::context_t context(1);
        auto peers = readConfiguration(configPath);
        std::pair<std::string, std::string> selfConfig = *(peers.find(name));
        auto m = std::make_shared<Messenger>(context, selfConfig, peers);
        auto monitorUUID = sole::rebuild("36073ca4-6681-446f-beae-5155faf993c0");
        DistributedMonitor<std::vector<int>, IntVectorSerializer> monitor(m, monitorUUID);

//        auto dm = std::make_shared<DistributedMutex>(m, mutexuuid);
//        DistributedConditionalVariable cv(m, cvuuid, dm);
        m->listen();
        sleep(10);
        for (int i = 0; i < 10; i++) {
            std::mt19937_64 eng{std::random_device{}()};  // or seed however you want
            std::uniform_int_distribution<> dist{1000, 5000};
            std::this_thread::sleep_for(std::chrono::milliseconds{dist(eng)});
            monitor->emplace_back(::getpid());
        }
//        dm->lock();
//        if (name == "beta") {
//            cv.wait();
//        } else {
//            sleep(10);
//            cv.signal();
//        }
//        dm->unlock();
        getchar();
        auto vector = *monitor.getStateUnsafe().get();
        for (auto &&item : vector) {
            std::cout << item << " ";
        }
        std::flush(std::cout);
        m->joinReceiver();

    } catch (const std::exception &ex) {
        LoggerSingleton::getInstance()->log(ex.what());
    }
    return 0;
}
