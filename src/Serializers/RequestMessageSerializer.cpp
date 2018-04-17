#include "RequestMessageSerializer.h"

std::string RequestMessageSerializer::serialize(const Message &message) {
    auto &requestMessage = dynamic_cast<const RequestMessage &>(message);
    std::ostringstream oss;
    oss << requestMessage.getMutexUUID() << ";";
    oss << std::to_string(requestMessage.getRequestNumber());
    return oss.str();
}

std::unique_ptr<Message> RequestMessageSerializer::deserialize(const std::string &string) {
    std::istringstream iss(string);
    std::string mutexUUID;
    std::getline(iss, mutexUUID, ';');
    std::string requestNumberString;
    iss >> requestNumberString;
    int requestNumber = std::stoi(requestNumberString);
    return std::make_unique<RequestMessage>(sole::rebuild(mutexUUID), requestNumber);
}
