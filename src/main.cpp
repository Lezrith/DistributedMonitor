#include <Core/SafeSocket.h>
#include <Messages/PrivilegeMessage.h>
#include <Core/DistributedConditionalVariable.h>
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
        auto cvuuid = sole::rebuild("36073ca4-6681-446f-beae-5155faf993c0");
        auto mutexuuid = sole::rebuild("15e6c267-394d-4a19-b83a-a17e8c43f07f");
        auto dm = std::make_shared<DistributedMutex>(m, mutexuuid);
        DistributedConditionalVariable cv(m, cvuuid, dm);
        m->listen();
        dm->lock();
        if (name == "beta") {
            cv.wait();
        } else {
            sleep(10);
            cv.signal();
        }
        dm->unlock();
        getchar();

    } catch (const std::exception &ex) {
        LoggerSingleton::getInstance()->log(ex.what());
    }
    return 0;
}
