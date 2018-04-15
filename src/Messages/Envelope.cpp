#include "Envelope.h"

const Message *Envelope::getPayload() const {
    return this->payload;
}

Envelope::Envelope(Message *message) : Message(MessageType::ENVELOPE), payload(message) {
    this->payloadType = message->getType();
}

MessageType Envelope::getPayloadType() const {
    return this->payloadType;
}
