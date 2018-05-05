#ifndef DISTRIBUTEDMONITOR_STATEMESSAGE_H
#define DISTRIBUTEDMONITOR_STATEMESSAGE_H


#include "Utils/Sole/sole.h"
#include "Message.h"

class StateMessage : public Message {
public:
    StateMessage(const sole::uuid &monitorUUID, const std::string &serializedState, const sole::uuid &messageUUID);

    const sole::uuid &getMonitorUUID() const;

    const std::string &getSerializedState() const;

    const sole::uuid &getMessageUUID() const;

private:
    sole::uuid monitorUUID;
    std::string serializedState;
    sole::uuid messageUUID;
};


#endif //DISTRIBUTEDMONITOR_STATEMESSAGE_H
