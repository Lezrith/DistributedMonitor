#ifndef DISTRIBUTEDMONITOR_CALLBACK_H
#define DISTRIBUTEDMONITOR_CALLBACK_H

#include "Utils/Sole/sole.h"

template<typename U>
class CallbackWrapper {
public:
    explicit CallbackWrapper(std::function<void(const Envelope &)> callback) : callback(std::move(callback)), callbackUUID(sole::uuid4()) {}

    bool operator==(const CallbackWrapper &rhs) const {
        return callbackUUID == rhs.callbackUUID;
    }

    bool operator!=(const CallbackWrapper &rhs) const {
        return !(rhs == *this);
    }

    void operator()(const U &argument) const {
        this->callback(argument);
    }

private:
    std::function<void(const Envelope &)> callback;
    sole::uuid callbackUUID;
};


#endif //DISTRIBUTEDMONITOR_CALLBACK_H
