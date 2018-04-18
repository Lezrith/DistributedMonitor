#include "AcknowledgeMessage.h"

const sole::uuid &AcknowledgeMessage::getRequestUUID() const {
    return requestUUID;
}

AcknowledgeMessage::AcknowledgeMessage(const sole::uuid &requestUUID) : Message(MessageType::ACKNOWLEDGE), requestUUID(requestUUID) {}
