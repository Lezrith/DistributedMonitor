#ifndef DISTRIBUTEDMONITOR_PRIVILEGEMESSAGESERIALIZER_H
#define DISTRIBUTEDMONITOR_PRIVILEGEMESSAGESERIALIZER_H


#include <string>
#include <memory>
#include <sstream>
#include <queue>
#include <regex>
#include "Messages/PrivilegeMessage.h"
#include "Messages/Message.h"
#include "MessageSerializer.h"

class PrivilegeMessageSerializer : public MessageSerializer {
public:
    std::string serialize(const Message &message) override;

    std::unique_ptr<Message> deserialize(const std::string &string) override;

    std::map<std::string, int> deserializeRequestNumbers(std::string &string) const;

    std::deque<std::string> deserializeWaitingNodes(std::string &string) const;
};


#endif //DISTRIBUTEDMONITOR_PRIVILEGEMESSAGESERIALIZER_H
