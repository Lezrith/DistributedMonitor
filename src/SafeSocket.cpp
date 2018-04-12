#include "SafeSocket.h"

SafeSocket::SafeSocket(zmq::context_t &context, zmq::socket_type type, int linger) : SafeSocket(context, type, std::string(""), linger) {

}

SafeSocket::SafeSocket(zmq::context_t &context, zmq::socket_type type, const std::string &identity, int linger) : socket(context, type), mutex() {
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
    std::lock_guard<std::mutex> guard(this->mutex);
    std::string message = this->receiveUnsafe();
    return message;

}

bool SafeSocket::send(const std::string &string) {
    std::lock_guard<std::mutex> guard(this->mutex);
    bool result = this->sendUnsafe(string);
    return result;
}

void SafeSocket::bind(const std::string &address) {
    std::lock_guard<std::mutex> guard(this->mutex);
    this->socket.bind(address.c_str());
}

void SafeSocket::connect(const std::string &address) {
    std::lock_guard<std::mutex> guard(this->mutex);
    this->socket.connect(address.c_str());
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


