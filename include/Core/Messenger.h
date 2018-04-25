#ifndef DISTRIBUTEDMONITOR_MESSENGER_H
#define DISTRIBUTEDMONITOR_MESSENGER_H


#include <vector>
#include <string>
#include <zmq.hpp>
#include <stdexcept>
#include <map>
#include <thread>
#include <regex>
#include <condition_variable>
#include "Utils/Sole/sole.h"
#include "Utils/CallbackRepository.h"
#include "Messages/Message.h"
#include "Messages/AcknowledgeMessage.h"
#include "Serializers/MessageSerializerFactory.h"
#include "SafeSocket.h"
#include "Logger.h"

class Messenger {
public:
    Messenger(zmq::context_t &context, std::pair<std::string, std::string> &selfConfig, std::map<std::string, std::string> &peersConfig);

    ~Messenger();

    bool send(const std::string &name, const Envelope &envelope);

    bool sendBroadcast(const std::string &string);

    bool sendBroadcast(const Envelope &envelope);

    std::string receiveString();

    std::unique_ptr<Envelope> receive();

    void listen();

    void joinReceiver();

    const std::string &getIdentity() const;

    std::vector<std::string> getPeers() const;

    void sendBroadcastWithACK(const Envelope &envelope, sole::uuid requestUUID);

    CallbackRepository<MessageType, Envelope> onReceive;
    CallbackRepository<MessageType, Envelope> onSent;

private:
    bool send(const std::string &name, const std::string &string);
    std::thread *receiverThread = nullptr;
    SafeSocket *inSocket;
    std::map<std::string, SafeSocket *> outSockets;
    std::string identity;
    std::string selfAddress;
    std::map<std::string, std::string> peersConfig;

    SafeSocket *findPeerSocket(const std::string &name);

    void receiverLoop();

    void createInSocket(zmq::context_t &context, std::string address);

    void createOutSockets(zmq::context_t &context, const std::string &identity, const std::map<std::string, std::string> &peersConfig);

    void logSent(const std::string &name, MessageType type, const std::string &message, bool result);

    void logReceived(const std::string &sender, MessageType type, const std::string &string);
};


#endif //DISTRIBUTEDMONITOR_MESSENGER_H
