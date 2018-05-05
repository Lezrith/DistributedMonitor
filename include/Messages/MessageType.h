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
    WAIT_END,
    SIGNAL,
    POISON,
    ACKNOWLEDGE,
    STATE,
};

std::string messageTypeToString(MessageType type);

#endif //DISTRIBUTEDMONITOR_MESSAGETYPE_H
