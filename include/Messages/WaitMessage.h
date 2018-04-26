#ifndef DISTRIBUTEDMONITOR_WAITMESSAGE_H
#define DISTRIBUTEDMONITOR_WAITMESSAGE_H


#include "Utils/Sole/sole.h"
#include "Message.h"

class WaitMessage : public Message {
public:

    WaitMessage(const sole::uuid &conditionalVariableUUID, const sole::uuid &messageUUID);

    const sole::uuid &getConditionalVariableUUID() const;

    const sole::uuid &getMessageUUID() const;

private:
    sole::uuid conditionalVariableUUID;
    sole::uuid messageUUID;
};


#endif //DISTRIBUTEDMONITOR_WAITMESSAGE_H
