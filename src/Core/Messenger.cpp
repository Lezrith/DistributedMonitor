#include <Messages/Envelope.h>
#include "Core/Messenger.h"

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
    return result;
}

void Messenger::logSent(const std::string &name, MessageType type, const std::string &message, bool result) {
    if (result) {
        std::stringstream ss;
        ss << "Sent " << messageTypeToString(type) << " message(" << message << ") to " << (name != this->identity ? name : "SELF");
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
        LoggerSingleton::getInstance()->log("Ready for incoming message");
    }
}

bool Messenger::send(const std::string &name, const Envelope &envelope) {
    MessageSerializerFactory factory;
    auto serializer = factory.createSerializer(envelope.getType());
    std::string serializedEnvelope = serializer->serialize(envelope);
    this->onSent.notifySubscribers(envelope.getPayloadType(), envelope);
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
    this->logReceived(sender, envelope->getPayloadType(), serializedEnvelope);
    this->onReceive.notifySubscribers(envelope->getPayloadType(), *envelope);

    return envelope;
}

void Messenger::logReceived(const std::string &sender, MessageType type, const std::string &string) {
    std::stringstream ss;
    ss << "Received " << messageTypeToString(type) << " message(" << string << ") from " << (sender != this->identity ? sender : "SELF");
    LoggerSingleton::getInstance()->log(ss.str());
}

const std::string &Messenger::getIdentity() const {
    return this->identity;
}

std::vector<std::string> Messenger::getPeers() const {
    std::vector<std::string> peers;
    for (auto &&config : this->peersConfig) {
        peers.emplace_back(config.first);
    }
    return peers;
}

void Messenger::sendBroadcastWithACK(const Envelope &envelope, sole::uuid requestUUID) {
    std::mutex m;
    std::condition_variable cv;
    std::unique_lock<std::mutex> lock(m);
    unsigned long repliesNeeded = this->peersConfig.size() - 1;
    std::function<void(const Envelope &)> onACKReceived = [&repliesNeeded, &m, &cv, &requestUUID](const Envelope &e) {
        auto acknowledgeMessage = dynamic_cast<const AcknowledgeMessage *>(e.getPayload());
        if (acknowledgeMessage->getRequestUUID() == requestUUID) {
            std::lock_guard<std::mutex> guard(m);
            repliesNeeded--;
            if (repliesNeeded == 0) {
                cv.notify_one();
            }
        }
    };
    auto callbackHandler = this->onReceive.subscribe(MessageType::ACKNOWLEDGE, onACKReceived);
    this->sendBroadcast(envelope);
    while (repliesNeeded != 0) {
        cv.wait(lock);
    }
    this->onReceive.unsubscribe(*callbackHandler);
}
