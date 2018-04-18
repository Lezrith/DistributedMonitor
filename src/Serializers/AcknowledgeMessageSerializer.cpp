#include <Messages/AcknowledgeMessage.h>
#include "AcknowledgeMessageSerializer.h"

std::string AcknowledgeMessageSerializer::serialize(const Message &message) {
    auto &acknowledgeMessage = dynamic_cast<const AcknowledgeMessage &>(message);
    return acknowledgeMessage.getRequestUUID().str();
}

std::unique_ptr<Message> AcknowledgeMessageSerializer::deserialize(const std::string &string) {
    return std::make_unique<AcknowledgeMessage>(sole::rebuild(string));
}
