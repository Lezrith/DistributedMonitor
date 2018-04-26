#include <Core/SafeSocket.h>
#include <Messages/PrivilegeMessage.h>
#include "Messages/Envelope.h"
#include "Core/DistributedMutex.h"
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
        m.onReceive.subscribe(MessageType::STRING, [&m](const Envelope &e) {
            auto stringMessage = dynamic_cast<const StringMessage *>(e.getPayload());
            sole::uuid uuid = sole::rebuild(stringMessage->getText());
            std::unique_ptr<Message> ack = std::make_unique<AcknowledgeMessage>(uuid);
            m.send("beta", Envelope(ack));
        });
        if (name == "beta") {
            sole::uuid uuid = sole::uuid0();
            std::unique_ptr<Message> str = std::make_unique<StringMessage>(uuid.str());
            m.sendBroadcastWithACK(Envelope(str), uuid);
        }
        getchar();

    } catch (const std::exception &ex) {
        LoggerSingleton::getInstance()->log(ex.what());
    }
    return 0;
}
