#ifndef DISTRIBUTEDMONITOR_MESSENGER_H
#define DISTRIBUTEDMONITOR_MESSENGER_H


#include <vector>
#include <string>
#include <zmq.hpp>
#include <stdexcept>
#include <map>
#include <thread>
#include <regex>
#include "Messages/Message.h"
#include "Serializers/MessageSerializerFactory.h"
#include "SafeSocket.h"
#include "Logger.h"

class Messenger {
public:
    Messenger(zmq::context_t &context, std::pair<std::string, std::string> &selfConfig, std::map<std::string, std::string> &peers);

    ~Messenger();

    bool send(const std::string &name, const std::string &string);

    bool send(const std::string &name, const Envelope &envelope);

    bool sendBroadcast(const std::string &string);

    bool sendBroadcast(const Envelope &envelope);

    std::string receiveString();

    std::unique_ptr<Envelope> receive();

    void listen();

    void joinReceiver();

    const std::string &getIdentity() const;

private:
    std::thread *receiverThread = nullptr;
    SafeSocket *inSocket;
    std::map<std::string, SafeSocket *> outSockets;
    std::string identity;

    SafeSocket *findPeerSocket(const std::string &name);

    void receiverLoop();

    void createInSocket(zmq::context_t &context, std::string address);

    void createOutSockets(zmq::context_t &context, const std::string &identity, const std::map<std::string, std::string> &peers);

    void logSent(const std::string &name, bool result);
};


#endif //DISTRIBUTEDMONITOR_MESSENGER_H
