#include <Messages/Envelope.h>
#include "Messenger.h"

Messenger::Messenger(zmq::context_t &context, std::pair<std::string, std::string> &selfConfig, std::map<std::string, std::string> &peersConfig) : identity(selfConfig.first),
                                                                                                                                                  selfAddress(selfConfig.second),
                                                                                                                                                  peersConfig(peersConfig) {
    this->createInSocket(context, selfConfig.second);
    this->createOutSockets(context, selfConfig.first, peersConfig);
}

void Messenger::createOutSockets(zmq::context_t &context, const std::string &identity, const std::map<std::string, std::string> &peersConfig) {
    for (auto &&config : peersConfig) {
        auto *socket = new SafeSocket(context, zmq::socket_type::dealer, identity);
        socket->connect(config.second);
        this->outSockets.insert(std::pair<std::string, SafeSocket *>(config.first, socket));

        LoggerSingleton::getInstance()->log("Connected outSocket to " + config.second);
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
    //this->logSent(name, MessageType::STRING, string, result);
    return result;
}

void Messenger::logSent(const std::string &name, MessageType type, const std::string &message, bool result) {
    if (result) {
        std::stringstream ss;
        ss << "Sent " << messageTypeToString(type) << " message(" << message << ") to " << name;
        LoggerSingleton::getInstance()->log(ss.str());
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
        auto envelope = this->receive();
        shouldStop = envelope->getPayloadType() == MessageType::POISON;
    }
}

bool Messenger::send(const std::string &name, const Envelope &envelope) {
    MessageSerializerFactory factory;
    auto serializer = factory.createSerializer(envelope.getType());
    std::string serializedEnvelope = serializer->serialize(envelope);
    bool result = this->send(name, serializedEnvelope);
    this->logSent(name, envelope.getPayloadType(), serializedEnvelope, result);
    return result;
}

bool Messenger::sendBroadcast(const Envelope &envelope) {
    MessageSerializerFactory factory;
    auto serializer = factory.createSerializer(envelope.getType());
    std::string serializedEnvelope = serializer->serialize(envelope);
    bool result = this->sendBroadcast(serializedEnvelope);
    this->logSent("BROADCAST", envelope.getPayloadType(), serializedEnvelope, result);
    return result;
}

std::unique_ptr<Envelope> Messenger::receive() {
    MessageSerializerFactory factory;
    auto serializer = factory.createSerializer(MessageType::ENVELOPE);
    std::string sender = this->receiveString();
    std::string serializedEnvelope = this->receiveString();
    auto message = serializer->deserialize(serializedEnvelope);
    auto envelope = std::unique_ptr<Envelope>(dynamic_cast<Envelope *>(message.release()));
    envelope->setSender(sender);
    this->logReceived(sender != this->identity ? sender : "SELF", envelope->getPayloadType(), serializedEnvelope);
    this->notifySubscribers(*envelope);

    return envelope;
}

void Messenger::logReceived(const std::string &sender, MessageType type, const std::string &string) {
    std::stringstream ss;
    ss << "Received " << messageTypeToString(type) << " message(" << string << ") from " << sender;
    LoggerSingleton::getInstance()->log(ss.str());
}

const std::string &Messenger::getIdentity() const {
    return this->identity;
}

CallbackWrapper<Envelope> *Messenger::registerCallback(MessageType type, const std::function<void(const Envelope &)> &callback) {
    return this->callbackRepository.registerCallback(type, callback);
}

void Messenger::unregisterCallback(const CallbackWrapper<Envelope> &callback) {
    this->callbackRepository.unregisterCallback(callback);
}

void Messenger::notifySubscribers(const Envelope &envelope) {
    auto callbacks = this->callbackRepository.getCallbacks(envelope.getPayloadType());
    for (auto &&callback : callbacks) {
        callback(envelope);
    }
}

std::vector<std::string> Messenger::getPeers() const {
    std::vector<std::string> peers;
    for (auto &&config : this->peersConfig) {
        peers.emplace_back(config.first);
    }
    return peers;
}
