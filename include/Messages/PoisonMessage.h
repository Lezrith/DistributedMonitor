#ifndef DISTRIBUTEDMONITOR_POISONMESSAGE_H
#define DISTRIBUTEDMONITOR_POISONMESSAGE_H


#include <string>
#include "Message.h"

class PoisonMessage : public Message {
public:
    PoisonMessage();

    explicit PoisonMessage(const std::string &reason);

    const std::string &getReason() const;

private:
    std::string reason;
};


#endif //DISTRIBUTEDMONITOR_POISONMESSAGE_H
