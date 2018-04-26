#include "WaitEndMessage.h"

WaitEndMessage::WaitEndMessage(const sole::uuid &conditionalVariableUUID, const sole::uuid &messageUUID) : Message(MessageType::WAIT_END),
                                                                                                           conditionalVariableUUID(conditionalVariableUUID),
                                                                                                           messageUUID(messageUUID) {}

const sole::uuid &WaitEndMessage::getConditionalVariableUUID() const {
    return conditionalVariableUUID;
}

const sole::uuid &WaitEndMessage::getMessageUUID() const {
    return messageUUID;
}
