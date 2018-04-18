#ifndef DISTRIBUTEDMONITOR_ACKNOWLEDGEMESSAGE_H
#define DISTRIBUTEDMONITOR_ACKNOWLEDGEMESSAGE_H


#include "Utils/Sole/sole.h"
#include "Message.h"

class AcknowledgeMessage : public Message {
public:
    explicit AcknowledgeMessage(const sole::uuid &requestUUID);

    const sole::uuid &getRequestUUID() const;

private:
    sole::uuid requestUUID;
};


#endif //DISTRIBUTEDMONITOR_ACKNOWLEDGEMESSAGE_H
