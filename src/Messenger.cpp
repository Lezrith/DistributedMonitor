#include "Messenger.h"

Messenger::Messenger(zmq::context_t &context, std::pair<std::string, std::string> &selfConfig, std::map<std::string, std::string> &peers) {
    this->inSocket = new SafeSocket(context, zmq::socket_type::router);
    selfConfig.second = std::regex_replace(selfConfig.second, std::regex(R"(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})"), "*");
    this->inSocket->bind(selfConfig.second);
    LoggerSingleton::getInstance()->log("Bound inSocket to " + selfConfig.second);
    for (auto &&peer : peers) {
        auto *socket = new SafeSocket(context, zmq::socket_type::dealer, selfConfig.first);
        socket->connect(peer.second);
        LoggerSingleton::getInstance()->log("Connected outSocket to " + peer.second);
        this->outSockets.insert(std::pair<std::string, SafeSocket *>(peer.first, socket));
    }
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
    LoggerSingleton::getInstance()->log("Sending to " + name);
    return socket->send(string);
}

std::string Messenger::receive() {
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
        LoggerSingleton::getInstance()->log("Sending to " + socket.first);
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
        std::string from = this->receive();
        std::string message = this->receive();
        if (message == "kill") {
            shouldStop = true;
        }
        LoggerSingleton::getInstance()->log(message.append(" from ").append(from));
    }
}
