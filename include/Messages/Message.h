#ifndef DISTRIBUTEDMONITOR_MESSAGE_H
#define DISTRIBUTEDMONITOR_MESSAGE_H


#include "MessageType.h"

class Message {
public:
    explicit Message(MessageType type);

    MessageType getType() const;

    virtual ~Message() = default;

private:
    MessageType type;
};


#endif //DISTRIBUTEDMONITOR_MESSAGE_H
