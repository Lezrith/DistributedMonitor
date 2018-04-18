#include "MessageSerializerFactory.h"

std::unique_ptr<MessageSerializer> MessageSerializerFactory::createSerializer(MessageType type) {
    switch (type) {
        case ENVELOPE:
            return std::make_unique<EnvelopeSerializer>();
        case STRING:
            return std::make_unique<StringMessageSerializer>();
        case PRIVILEGE:
            return std::make_unique<PrivilegeMessageSerializer>();
        case REQUEST:
            return std::make_unique<RequestMessageSerializer>();
        case ACKNOWLEDGE:
            return std::make_unique<AcknowledgeMessageSerializer>();
        case SIGNAL:
        case POISON:
        case REPLY:
        default:
            throw std::logic_error("Unknown message type" + type);
    }
}
