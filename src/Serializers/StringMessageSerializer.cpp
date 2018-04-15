#include "StringMessageSerializer.h"

std::string StringMessageSerializer::serialize(const Message &message) {
    auto &stringMessage = dynamic_cast<const StringMessage &>(message);
    return stringMessage.getText();
}

std::unique_ptr<Message> StringMessageSerializer::deserialize(const std::string &string) {
    return std::make_unique<StringMessage>(string);
}
