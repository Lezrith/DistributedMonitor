#include "PoisonMessage.h"

PoisonMessage::PoisonMessage() : PoisonMessage("") {}

PoisonMessage::PoisonMessage(const std::string &reason) : Message(MessageType::POISON), reason(reason) {}

const std::string &PoisonMessage::getReason() const {
    return reason;
}
