#include <string>
#include <sstream>
#include "Serializers/MessageSerializerFactory.h"
#include "Messages/Envelope.h"

class MessageSerializerFactory;

std::string EnvelopeSerializer::serialize(const Message &message) {
    auto envelope = dynamic_cast<const Envelope *>(&message);
    MessageSerializerFactory factory;
    auto serializer = factory.createSerializer(envelope->getPayloadType());
    auto payload = envelope->getPayload();

    std::string stringPayload = serializer->serialize(*payload);
    std::ostringstream oss;
    oss << envelope->getPayloadType() << ";" << stringPayload;
    return oss.str();
}

std::unique_ptr<Message> EnvelopeSerializer::deserialize(const std::string &string) {
    std::istringstream iss(string);
    std::string typeString;
    std::getline(iss, typeString, ';');
    auto payloadType = static_cast<MessageType>(std::stoi(typeString));

    MessageSerializerFactory factory;
    std::unique_ptr<MessageSerializer> serializer = factory.createSerializer(payloadType);

    std::string payload;
    iss >> payload;
    std::unique_ptr<Message> message = serializer->deserialize(payload);
    return std::make_unique<Envelope>(std::move(message));
}
