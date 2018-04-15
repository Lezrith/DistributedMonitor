#include "PrivilegeMessage.h"

PrivilegeMessage::PrivilegeMessage(const sole::uuid &mutexUUID, const std::deque<std::string> &waitingNodes, const std::map<std::string, int> &requestNumbers)
        : Message(MessageType::PRIVILEGE), waitingNodes(waitingNodes), requestNumbers(requestNumbers), mutexUUID(mutexUUID) {}

PrivilegeMessage::PrivilegeMessage(const std::deque<std::string> &waitingNodes, const std::map<std::string, int> &requestNumbers) : PrivilegeMessage(sole::uuid0(),
                                                                                                                                                     waitingNodes,
                                                                                                                                                     requestNumbers) {}

const std::deque<std::string> &PrivilegeMessage::getWaitingNodes() const {
    return this->waitingNodes;
}

const std::map<std::string, int> &PrivilegeMessage::getRequestNumbers() const {
    return this->requestNumbers;
}

const sole::uuid &PrivilegeMessage::getmutexUUID() const {
    return this->mutexUUID;
}

