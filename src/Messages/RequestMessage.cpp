#include "RequestMessage.h"

int RequestMessage::getRequestNumber() const {
    return this->requestNumber;
}

RequestMessage::RequestMessage(sole::uuid mutexUUID, int requestNumber) : Message(MessageType::REQUEST), mutexUUID(mutexUUID), requestNumber(requestNumber) {

}

const sole::uuid &RequestMessage::getMutexUUID() const {
    return this->mutexUUID;
}
