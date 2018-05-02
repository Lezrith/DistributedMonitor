#include "WaitEndMessage.h"

WaitEndMessage::WaitEndMessage(const std::string &identity, const sole::uuid &conditionalVariableUUID, const sole::uuid &messageUUID) : Message(MessageType::WAIT_END),
                                                                                                                                        identity(identity),
                                                                                                                                        conditionalVariableUUID(
                                                                                                                                                conditionalVariableUUID),
                                                                                                                                        messageUUID(messageUUID) {}

const sole::uuid &WaitEndMessage::getConditionalVariableUUID() const {
    return conditionalVariableUUID;
}

const sole::uuid &WaitEndMessage::getMessageUUID() const {
    return messageUUID;
}

const std::string &WaitEndMessage::getIdentity() const {
    return identity;
}
