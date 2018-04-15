#include <Messages/Envelope.h>
#include "Messenger.h"

Messenger::Messenger(zmq::context_t &context, std::pair<std::string, std::string> &selfConfig, std::map<std::string, std::string> &peers) : identity(selfConfig.first) {
    this->createInSocket(context, selfConfig.second);
    this->createOutSockets(context, selfConfig.first, peers);
}

void Messenger::createOutSockets(zmq::context_t &context, const std::string &identity, const std::map<std::string, std::string> &peers) {
    for (auto &&peer : peers) {
        auto *socket = new SafeSocket(context, zmq::socket_type::dealer, identity);
        socket->connect(peer.second);
        this->outSockets.insert(std::pair<std::string, SafeSocket *>(peer.first, socket));

        LoggerSingleton::getInstance()->log("Connected outSocket to " + peer.second);
    }
}

void Messenger::createInSocket(zmq::context_t &context, std::string address) {
    inSocket = new SafeSocket(context, zmq::socket_type::router);
    address = regex_replace(address, std::regex(R"(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})"), "*");
    inSocket->bind(address);

    LoggerSingleton::getInstance()->log("Bound inSocket to " + address);
}

Messenger::~Messenger() {
    this->joinReceiver();
    delete this->inSocket;
    for (auto &&socket : this->outSockets) {
        delete socket.second;
    }
}

bool Messenger::send(const std::string &name, const std::string &string) {
    SafeSocket *socket = this->findPeerSocket(name);
    bool result = socket->send(string);
    this->logSent(name, result);
    return result;
}

void Messenger::logSent(const std::string &name, bool result) {
    if (result) {
        LoggerSingleton::getInstance()->log("Sent to " + name);
    } else {
        LoggerSingleton::getInstance()->log("Error while sending to " + name);
    }
}

std::string Messenger::receiveString() {
    return this->inSocket->receive();
}

SafeSocket *Messenger::findPeerSocket(const std::string &name) {
    auto iter = this->outSockets.find(name);
    if (iter == this->outSockets.end()) {
        throw std::logic_error("Socket with id = " + name + "not found");
    }
    return iter->second;
}

bool Messenger::sendBroadcast(const std::string &string) {
    bool result = true;
    for (auto &&socket : this->outSockets) {
        socket.second->lock();
    }
    for (auto &&socket : this->outSockets) {
        result = socket.second->sendUnsafe(string) && result;
        this->logSent(socket.first, result);
    }
    for (auto &&socket : this->outSockets) {
        socket.second->unlock();
    }
    return result;
}

void Messenger::listen() {
    if (this->receiverThread != nullptr) {
        throw std::logic_error("Receiver is already running");
    }
    this->receiverThread = new std::thread(&Messenger::receiverLoop, this);
    LoggerSingleton::getInstance()->log("Receiver started");
}

void Messenger::joinReceiver() {
    if (this->receiverThread != nullptr && this->receiverThread->joinable()) {
        this->receiverThread->join();
        LoggerSingleton::getInstance()->log("Receiver stopped");
    } else {
        LoggerSingleton::getInstance()->log("Receiver was already stopped");
    }
}

void Messenger::receiverLoop() {
    bool shouldStop = false;
    while (!shouldStop) {
        std::string from = this->receiveString();
        std::string message = this->receiveString();
        if (message == "kill") {
            shouldStop = true;
        }
        LoggerSingleton::getInstance()->log(message.append(" from ").append(from));
    }
}

bool Messenger::send(const std::string &name, const Envelope &envelope) {
    MessageSerializerFactory factory;
    auto serializer = factory.createSerializer(envelope.getType());
    std::string serializedEnvelope = serializer->serialize(envelope);

    return this->send(name, serializedEnvelope);
}

bool Messenger::sendBroadcast(const Envelope &envelope) {
    MessageSerializerFactory factory;
    auto serializer = factory.createSerializer(envelope.getType());
    std::string serializedEnvelope = serializer->serialize(envelope);

    return this->sendBroadcast(serializedEnvelope);
}

std::unique_ptr<Envelope> Messenger::receive() {
    MessageSerializerFactory factory;
    auto serializer = factory.createSerializer(MessageType::ENVELOPE);
    std::string sender = this->receiveString();
    std::string serializedEnvelope = this->receiveString();
    auto message = serializer->deserialize(serializedEnvelope);
    auto envelope = std::unique_ptr<Envelope>(dynamic_cast<Envelope *>(message.release()));
    envelope->setSender(sender);
    LoggerSingleton::getInstance()->log(serializedEnvelope.append(" from ").append(sender));

    return envelope;
}

const std::string &Messenger::getIdentity() const {
    return this->identity;
}
