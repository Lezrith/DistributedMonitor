#ifndef DISTRIBUTEDMONITOR_STRINGMESSAGE_H
#define DISTRIBUTEDMONITOR_STRINGMESSAGE_H


#include <string>
#include "Message.h"

class StringMessage : public Message {
public:
    explicit StringMessage(const std::string &text);

    const std::string &getText() const;

private:
    std::string text;
};


#endif //DISTRIBUTEDMONITOR_STRINGMESSAGE_H
