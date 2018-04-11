#include "SafeSocket.h"
#include "Logger.h"

SafeSocket::SafeSocket(zmq::context_t &context, zmq::socket_type type) : SafeSocket(context, type, std::string("")) {

}

SafeSocket::SafeSocket(zmq::context_t &context, zmq::socket_type type, const std::string &identity) : socket(context, type), mutex() {
    int linger = 0;
    if (!identity.empty()) {
        this->socket.setsockopt(ZMQ_IDENTITY, identity.c_str(), identity.length());
    }
    this->socket.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
}

void SafeSocket::lock() {
    this->mutex.lock();
}

void SafeSocket::unlock() {
    this->mutex.unlock();
}

std::string SafeSocket::receive() {
    this->lock();
    std::string message = this->receiveUnsafe();
    this->unlock();
    return message;

}

bool SafeSocket::send(const std::string &string) {
    this->lock();

    bool result = this->sendUnsafe(string);

    this->unlock();
    return result;
}

void SafeSocket::bind(const std::string &address) {
    this->lock();
    this->socket.bind(address.c_str());
    this->unlock();
}

void SafeSocket::connect(const std::string &address) {
    this->lock();
    this->socket.connect(address.c_str());
    this->unlock();
}

bool SafeSocket::sendUnsafe(const std::string &string) {
    zmq::message_t message(string.size());
    memcpy(message.data(), string.data(), string.size());
    return this->socket.send(message);
}

std::string SafeSocket::receiveUnsafe() {
    zmq::message_t message;
    this->socket.recv(&message);
    return std::string(static_cast<char *>(message.data()), message.size());
}


