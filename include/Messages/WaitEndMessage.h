#ifndef DISTRIBUTEDMONITOR_WAITENDMESSAGE_H
#define DISTRIBUTEDMONITOR_WAITENDMESSAGE_H


#include "Utils/Sole/sole.h"
#include "Message.h"

class WaitEndMessage : public Message {
public:
    WaitEndMessage(const sole::uuid &conditionalVariableUUID, const sole::uuid &messageUUID);

    const sole::uuid &getConditionalVariableUUID() const;

    const sole::uuid &getMessageUUID() const;

private:
    sole::uuid conditionalVariableUUID;
    sole::uuid messageUUID;
};


#endif //DISTRIBUTEDMONITOR_WAITENDMESSAGE_H
