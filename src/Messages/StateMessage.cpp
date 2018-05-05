#include "StateMessage.h"

StateMessage::StateMessage(const sole::uuid &monitorUUID, const std::string &serializedState, const sole::uuid &messageUUID) : Message(MessageType::STATE),
                                                                                                                               monitorUUID(monitorUUID),
                                                                                                                               serializedState(serializedState),
                                                                                                                               messageUUID(messageUUID) {}

const sole::uuid &StateMessage::getMonitorUUID() const {
    return monitorUUID;
}

const std::string &StateMessage::getSerializedState() const {
    return serializedState;
}

const sole::uuid &StateMessage::getMessageUUID() const {
    return messageUUID;
}
