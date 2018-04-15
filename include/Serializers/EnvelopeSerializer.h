#ifndef DISTRIBUTEDMONITOR_ENVELOPESERIALIZER_H
#define DISTRIBUTEDMONITOR_ENVELOPESERIALIZER_H


#include <Messages/Message.h>
#include <string>
#include "MessageSerializer.h"

class EnvelopeSerializer : public MessageSerializer {
    std::string serialize(const Message &message) override;

    Message deserialize(const std::string &string) override;
};


#endif //DISTRIBUTEDMONITOR_ENVELOPESERIALIZER_H
