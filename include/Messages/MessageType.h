#ifndef DISTRIBUTEDMONITOR_MESSAGETYPE_H
#define DISTRIBUTEDMONITOR_MESSAGETYPE_H

#include <string>

enum MessageType {
    ENVELOPE,
    STRING,
    REQUEST,
    REPLY,
    PRIVILEGE,
    WAIT,
    SIGNAL,
    POISON,
    ACKNOWLEDGE,
};

std::string messageTypeToString(MessageType type);

#endif //DISTRIBUTEDMONITOR_MESSAGETYPE_H
