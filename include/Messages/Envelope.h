#ifndef DISTRIBUTEDMONITOR_ENVELOPE_H
#define DISTRIBUTEDMONITOR_ENVELOPE_H


#include <memory>
#include "MessageType.h"
#include "Message.h"

class Envelope : public Message {
public:
    explicit Envelope(Message *message);

    MessageType getPayloadType() const;

    const Message *getPayload() const;

private:
    Message *payload;
    MessageType payloadType;
};


#endif //DISTRIBUTEDMONITOR_ENVELOPE_H
