#include "WaitMessage.h"

WaitMessage::WaitMessage(const sole::uuid &conditionalVariableUUID, const sole::uuid &messageUUID) : Message(MessageType::WAIT),
                                                                                                     conditionalVariableUUID(conditionalVariableUUID),
                                                                                                     messageUUID(messageUUID) {}

const sole::uuid &WaitMessage::getConditionalVariableUUID() const {
    return conditionalVariableUUID;
}

const sole::uuid &WaitMessage::getMessageUUID() const {
    return messageUUID;
}
