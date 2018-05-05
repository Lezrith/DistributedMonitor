#include "Messages/StateMessage.h"
#include "StateMessageSerializer.h"

std::string StateMessageSerializer::serialize(const Message &message) {
    auto &stateMessage = dynamic_cast<const StateMessage &>(message);
    std::stringstream ss;
    ss << stateMessage.getMonitorUUID() << ";" << stateMessage.getMessageUUID() << ";" << stateMessage.getSerializedState();
    return ss.str();
}

std::unique_ptr<Message> StateMessageSerializer::deserialize(const std::string &string) {
    std::stringstream ss(string);
    std::string monitorUUID;
    std::getline(ss, monitorUUID, ';');
    std::string messageUUID;
    std::getline(ss, messageUUID, ';');
    std::string serializedState;
    ss >> serializedState;
    return std::make_unique<StateMessage>(sole::rebuild(monitorUUID), serializedState, sole::rebuild(messageUUID));
}
