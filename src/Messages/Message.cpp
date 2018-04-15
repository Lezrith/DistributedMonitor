#include "Message.h"

Message::Message(MessageType type) : type(type) {}

MessageType Message::getType() const {
    return type;
}
