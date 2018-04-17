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
        default:
            return "Unknown";
    }
}