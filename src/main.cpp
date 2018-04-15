#include <SafeSocket.h>
#include <Messages/PrivilegeMessage.h>
#include "Messages/Envelope.h"
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
        if (name == "beta") {
            Messenger m(context, selfConfig, peers);
            m.listen();
            std::deque<std::string> d;
            d.emplace_back("alpha");
            d.emplace_back("beta");
            d.emplace_back("gamma");
            std::map<std::string, int> r;
            r["alpha"] = 12;
            r["gamma"] = 54;
            auto message = std::make_unique<PrivilegeMessage>(d, r);
            std::cout << message->getmutexUUID() << std::endl;

            Envelope envelope(std::move(message));

            m.send("alpha", envelope);
            //message = std::make_unique<StringMessage>("kill");
            //Envelope envelope2(std::move(message));
            //m.send("alpha", envelope2);
        } else {
            Messenger m(context, selfConfig, peers);
            auto e = m.receive();
            std::cout << (dynamic_cast<const PrivilegeMessage *>(e->getPayload()))->getmutexUUID() << std::endl;
            m.listen();
        }
    } catch (const std::exception &ex) {
        LoggerSingleton::getInstance()->log(ex.what());
    }
    return 0;
}
