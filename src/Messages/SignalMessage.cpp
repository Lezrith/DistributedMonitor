#include "SignalMessage.h"

SignalMessage::SignalMessage(const sole::uuid &conditionalVariableUUID) : Message(MessageType::SIGNAL),
                                                                          conditionalVariableUUID(conditionalVariableUUID) {}

const sole::uuid &SignalMessage::getConditionalVariableUUID() const {
    return conditionalVariableUUID;
}
