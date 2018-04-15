#include "StringMessageSerializer.h"

std::string StringMessageSerializer::serialize(const Message &message) {
    auto &stringMessage = dynamic_cast<const StringMessage &>(message);
    return stringMessage.getText();
}

Message StringMessageSerializer::deserialize(const std::string &string) {
    auto message = StringMessage(string);
    return dynamic_cast<Message &>(message);
}
