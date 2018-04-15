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
        if (name == "alpha") {
            Messenger m(context, selfConfig, peers);
            m.listen();
            Message *message = new StringMessage("test");
            Envelope envelope(message);
            m.send("beta", envelope);
            delete message;
            message = new StringMessage("kill");
            Envelope envelope2(message);
            m.send("beta", envelope2);
            delete message;
        } else {
            Messenger m(context, selfConfig, peers);
            m.listen();
        }
    } catch (const std::exception &ex) {
        LoggerSingleton::getInstance()->log(ex.what());
    }
    return 0;
}
