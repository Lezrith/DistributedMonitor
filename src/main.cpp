#include <SafeSocket.h>
#include <Messages/PrivilegeMessage.h>
#include "Messages/Envelope.h"
#include "DistributedMutex.h"
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
        Messenger m(context, selfConfig, peers);
        m.listen();
        DistributedMutex mutex(m, sole::rebuild("daca9e3a-cbdd-4797-a5dc-4071fd623dbf"));
        sleep(5);
        LoggerSingleton::getInstance()->log("before lock");
        mutex.lock();
        LoggerSingleton::getInstance()->log("after lock");
        sleep(5);
        LoggerSingleton::getInstance()->log("before unlock");
        mutex.unlock();
        LoggerSingleton::getInstance()->log("after unlock");
        getchar();

    } catch (const std::exception &ex) {
        LoggerSingleton::getInstance()->log(ex.what());
    }
    return 0;
}
