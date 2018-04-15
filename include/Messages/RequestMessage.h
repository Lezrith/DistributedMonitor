#ifndef DISTRIBUTEDMONITOR_REQUESTMESSAGE_H
#define DISTRIBUTEDMONITOR_REQUESTMESSAGE_H


#include "Utils/Sole/sole.h"
#include "Message.h"

class RequestMessage : public Message {
public:
    RequestMessage(sole::uuid mutexUUID, int requestNumber);

    int getRequestNumber() const;

    const sole::uuid &getMutexUUID() const;

private:
    sole::uuid mutexUUID;
    int requestNumber;
};


#endif //DISTRIBUTEDMONITOR_REQUESTMESSAGE_H
