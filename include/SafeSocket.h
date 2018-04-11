#ifndef DISTRIBUTEDMONITOR_SAFESOCKET_H
#define DISTRIBUTEDMONITOR_SAFESOCKET_H

#include <mutex>
#include <zmq.hpp>
#include <sys/types.h>
#include <unistd.h>

class SafeSocket {
public:
    SafeSocket(zmq::context_t &context, zmq::socket_type type);

    SafeSocket(zmq::context_t &context, zmq::socket_type type, const std::string &identity);

    void lock();

    void unlock();

    std::string receive();

    std::string receiveUnsafe();

    bool send(const std::string &string);

    bool sendUnsafe(const std::string &string);

    void bind(const std::string &address);

    void connect(const std::string &address);

private:
    zmq::socket_t socket;
    std::mutex mutex;
};


#endif //DISTRIBUTEDMONITOR_SAFESOCKET_H
