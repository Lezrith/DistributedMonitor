#include <SafeSocket.h>
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
            auto message = std::make_unique<StringMessage>("test");
            Envelope envelope(std::move(message));
            m.send("alpha", envelope);
            message = std::make_unique<StringMessage>("kill");
            Envelope envelope2(std::move(message));
            m.send("alpha", envelope2);
        } else {
            Messenger m(context, selfConfig, peers);
            auto e = m.receive();
            m.listen();
        }
    } catch (const std::exception &ex) {
        LoggerSingleton::getInstance()->log(ex.what());
    }
    return 0;
}
