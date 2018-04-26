#include "SignalMessageSerializer.h"

std::string SignalMessageSerializer::serialize(const Message &message) {
    auto &signalMessage = dynamic_cast<const SignalMessage &>(message);
    return signalMessage.getConditionalVariableUUID().str();
}

std::unique_ptr<Message> SignalMessageSerializer::deserialize(const std::string &string) {
    return std::make_unique<SignalMessage>(sole::rebuild(string));
}
