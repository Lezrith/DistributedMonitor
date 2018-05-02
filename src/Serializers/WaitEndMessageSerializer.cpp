#include "WaitEndMessageSerializer.h"

std::string WaitEndMessageSerializer::serialize(const Message &message) {
    auto &waitEndMessage = dynamic_cast<const WaitEndMessage &>(message);
    std::stringstream ss;
    ss << waitEndMessage.getIdentity() << ";" << waitEndMessage.getConditionalVariableUUID() << ";" << waitEndMessage.getMessageUUID();
    return ss.str();
}

std::unique_ptr<Message> WaitEndMessageSerializer::deserialize(const std::string &string) {
    std::stringstream ss(string);
    std::string identity;
    std::getline(ss, identity, ';');
    std::string conditionalVariableUUID;
    std::getline(ss, conditionalVariableUUID, ';');
    std::string messageUUID;
    ss >> messageUUID;
    return std::make_unique<WaitEndMessage>(identity, sole::rebuild(conditionalVariableUUID), sole::rebuild(messageUUID));
}
