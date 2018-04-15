#ifndef DISTRIBUTEDMONITOR_PRIVILIGEMESSAGE_H
#define DISTRIBUTEDMONITOR_PRIVILIGEMESSAGE_H


#include <string>
#include <queue>
#include <map>
#include "Utils/Sole/sole.h"
#include "Message.h"

class PrivilegeMessage : public Message {
public:
    PrivilegeMessage(const std::deque<std::string> &waitingNodes, const std::map<std::string, int> &requestNumbers);

    PrivilegeMessage(const sole::uuid &mutexUUID, const std::deque<std::string> &waitingNodes, const std::map<std::string, int> &requestNumbers);

    const std::deque<std::string> &getWaitingNodes() const;

    const std::map<std::string, int> &getRequestNumbers() const;

    const sole::uuid &getmutexUUID() const;

private:
    std::deque<std::string> waitingNodes;
    std::map<std::string, int> requestNumbers;
    sole::uuid mutexUUID;
};


#endif //DISTRIBUTEDMONITOR_PRIVILIGEMESSAGE_H
