#include "StringMessage.h"

StringMessage::StringMessage(const std::string &text) : Message(MessageType::STRING), text(text) {}

const std::string &StringMessage::getText() const {
    return text;
}
