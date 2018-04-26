#ifndef DISTRIBUTEDMONITOR_ENVELOPE_H
#define DISTRIBUTEDMONITOR_ENVELOPE_H


#include <memory>
#include "MessageType.h"
#include "Message.h"

class Envelope : public Message {
public:
    explicit Envelope(std::unique_ptr<Message> &message);

    MessageType getPayloadType() const;

    const Message *getPayload() const;

    const std::string &getSender() const;

    void setSender(const std::string &sender);

private:
    std::unique_ptr<Message> payload;
    MessageType payloadType;
    std::string sender;
};


#endif //DISTRIBUTEDMONITOR_ENVELOPE_H
