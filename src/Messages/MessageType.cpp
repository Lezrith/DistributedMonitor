#include "Message.h"

std::string messageTypeToString(MessageType type) {
    switch (type) {
        case MessageType::ENVELOPE:
            return "Envelope";
        case MessageType::STRING:
            return "String";
        case MessageType::REQUEST:
            return "Request";
        case MessageType::REPLY:
            return "Reply";
        case MessageType::PRIVILEGE:
            return "Privilege";
        case MessageType::SIGNAL:
            return "Signal";
        case MessageType::POISON:
            return "Poison";
        case MessageType::ACKNOWLEDGE:
            return "Acknowledge";
        case MessageType::WAIT:
            return "Wait";
        case MessageType::WAIT_END:
            return "Wait_end";
        case MessageType::STATE:
            return "State";
        default:
            return "Unknown";
    }
}