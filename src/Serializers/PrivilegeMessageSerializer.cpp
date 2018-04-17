#include "PrivilegeMessageSerializer.h"

std::string PrivilegeMessageSerializer::serialize(const Message &message) {
    auto privilegeMessage = dynamic_cast<const PrivilegeMessage &>(message);
    std::ostringstream oss;
    oss << privilegeMessage.getmutexUUID() << ";";
    std::deque<std::string> waitingNodes = privilegeMessage.getWaitingNodes();
    for (auto &&node :waitingNodes) {
        oss << node;
        if (&node != &waitingNodes.back()) {
            oss << ",";
        }
    }
    oss << ";";
    std::map<std::string, int> requestNumbers = privilegeMessage.getRequestNumbers();
    for (auto &&kv : requestNumbers) {
        oss << "(" << kv.first << "," << kv.second << ")";
    }
    return oss.str();
}

std::unique_ptr<Message> PrivilegeMessageSerializer::deserialize(const std::string &string) {
    std::stringstream ss(string);
    std::string UUIDString;
    std::getline(ss, UUIDString, ';');

    std::string waitingNodesString;
    std::getline(ss, waitingNodesString, ';');
    std::deque<std::string> waitingNodes = deserializeWaitingNodes(waitingNodesString);

    std::string requestNumbersString;
    ss >> requestNumbersString;
    std::map<std::string, int> requestNumbers = deserializeRequestNumbers(requestNumbersString);

    return std::make_unique<PrivilegeMessage>(sole::rebuild(UUIDString), waitingNodes, requestNumbers);
}

std::deque<std::string> PrivilegeMessageSerializer::deserializeWaitingNodes(std::string &string) const {
    std::deque<std::string> waitingNodes;
    std::stringstream ss(string);
    std::string nodeIdentity;
    while (getline(ss, nodeIdentity, ',')) {
        waitingNodes.push_back(nodeIdentity);
    }
    return waitingNodes;
}

std::map<std::string, int> PrivilegeMessageSerializer::deserializeRequestNumbers(std::string &string) const {
    std::map<std::string, int> requestNumbers;
    std::regex regex(R"(\((\w+),(-?\d+)\))");
    std::smatch match;
    std::string requestNumbersString = string;
    while (regex_search(requestNumbersString, match, regex)) {
        if (match.size() > 2) {
            std::string identity = match[1];
            int number = stoi(match[2]);
            requestNumbers[identity] = number;
        }
        requestNumbersString = match.suffix();
    }
    return requestNumbers;
}
