#include "MessageSerializerFactory.h"

std::unique_ptr<MessageSerializer> MessageSerializerFactory::createSerializer(MessageType type) {
    switch (type) {
        case STRING:
            return std::make_unique<StringMessageSerializer>();
        case REQUEST:
        case REPLY:
        case PRIVILEGE:
        case SIGNAL:
        default:
            throw std::logic_error("Unknown message type" + type);
    }
}
