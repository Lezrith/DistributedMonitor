#include "Messages/PoisonMessage.h"
#include "PoisonMessageSerializer.h"

std::string PoisonMessageSerializer::serialize(const Message &message) {
    auto poisonMessage = dynamic_cast<const PoisonMessage &>(message);
    return poisonMessage.getReason();
}

std::unique_ptr<Message> PoisonMessageSerializer::deserialize(const std::string &string) {
    return std::make_unique<PoisonMessage>(string);
}
