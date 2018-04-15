#include "Envelope.h"

Envelope::Envelope(std::unique_ptr<Message> message) : Message(MessageType::ENVELOPE) {
    this->payloadType = message->getType();
    this->payload = std::move(message);
}

MessageType Envelope::getPayloadType() const {
    return this->payloadType;
}

const Message *Envelope::getPayload() const {
    return this->payload.get();
}

const std::string &Envelope::getSender() const {
    return sender;
}

void Envelope::setSender(const std::string &sender) {
    this->sender = sender;
}
