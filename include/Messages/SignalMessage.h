#ifndef DISTRIBUTEDMONITOR_SIGNALMESSAGE_H
#define DISTRIBUTEDMONITOR_SIGNALMESSAGE_H


#include "Utils/Sole/sole.h"
#include "Message.h"

class SignalMessage : public Message {
public:
    explicit SignalMessage(const sole::uuid &conditionalVariableUUID);

    const sole::uuid &getConditionalVariableUUID() const;

private:
    sole::uuid conditionalVariableUUID;
};


#endif //DISTRIBUTEDMONITOR_SIGNALMESSAGE_H
