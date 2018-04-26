#include "WaitMessageSerializer.h"

std::string WaitMessageSerializer::serialize(const Message &message) {
    auto &waitMessage = dynamic_cast<const WaitMessage &>(message);
    std::stringstream ss;
    ss << waitMessage.getConditionalVariableUUID() << ";" << waitMessage.getMessageUUID();
    return ss.str();
}

std::unique_ptr<Message> WaitMessageSerializer::deserialize(const std::string &string) {
    std::stringstream ss(string);
    std::string conditionalVariableUUID;
    std::getline(ss, conditionalVariableUUID, ';');
    std::string messageUUID;
    ss >> messageUUID;
    return std::make_unique<WaitMessage>(sole::rebuild(conditionalVariableUUID), sole::rebuild(messageUUID));
}
