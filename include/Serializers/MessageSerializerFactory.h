#ifndef DISTRIBUTEDMONITOR_MESSAGESERIALIZERFACTORY_H
#define DISTRIBUTEDMONITOR_MESSAGESERIALIZERFACTORY_H


#include <stdexcept>
#include <memory>
#include "Messages/MessageType.h"
#include "Serializers/StringMessageSerializer.h"
#include "Serializers/EnvelopeSerializer.h"
#include "Serializers/PrivilegeMessageSerializer.h"
#include "Serializers/RequestMessageSerializer.h"
#include "Serializers/AcknowledgeMessageSerializer.h"
#include "Serializers/WaitEndMessageSerializer.h"
#include "Serializers/WaitMessageSerializer.h"
#include "Serializers/SignalMessageSerializer.h"
#include "Serializers/StateMessageSerializer.h"
#include "Serializers/PoisonMessageSerializer.h"
#include "Messages/Message.h"
#include "MessageSerializer.h"

class MessageSerializerFactory {
public:
    std::unique_ptr<MessageSerializer> createSerializer(MessageType type);
};


#endif //DISTRIBUTEDMONITOR_MESSAGESERIALIZERFACTORY_H
