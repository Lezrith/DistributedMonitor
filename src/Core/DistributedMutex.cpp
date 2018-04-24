#include "Core/DistributedMutex.h"

DistributedMutex::DistributedMutex(Messenger &messenger, const sole::uuid &UUID) : UUID(UUID), messenger(messenger), isRequesting(false), identity(messenger.getIdentity()) {
    std::vector<std::string> peers = messenger.getPeers();
    this->hasPrivilege = this->identity == peers.front();
    for (auto &&peer : peers) {
        this->requestNumbers[peer] = -1;
        this->grantedRequestNumbers[peer] = -1;
    }

    this->onPrivilegeReceivedHandle = messenger.registerCallback(MessageType::PRIVILEGE, [=](const Envelope &envelope) { this->onPrivilegeReceived(envelope); });
    this->onRequestReceivedHandle = messenger.registerCallback(MessageType::REQUEST, [=](const Envelope &envelope) { this->onRequestReceived(envelope); });
}

DistributedMutex::~DistributedMutex() {
    if (this->onPrivilegeReceivedHandle != nullptr) {
        this->messenger.unregisterCallback(*this->onPrivilegeReceivedHandle);
        this->messenger.unregisterCallback(*this->onRequestReceivedHandle);
    }
}

void DistributedMutex::lock() {
    std::unique_lock<std::mutex> lock(this->mutex);
    this->isRequesting = true;
    if (!this->hasPrivilege) {
        this->RequestPrivilege();
        this->waitingForPrivilege.wait(lock);
        this->hasPrivilege = true;
        auto iter = std::find(this->waitingNodes.begin(), this->waitingNodes.end(), this->identity);
        if (iter != this->waitingNodes.end()) {
            this->waitingNodes.erase(iter);
        }
    }
}

void DistributedMutex::RequestPrivilege() {
    int requestNumber = ++this->requestNumbers[this->identity];
    auto request = std::make_unique<RequestMessage>(this->UUID, requestNumber);
    Envelope envelope(move(request));
    messenger.sendBroadcast(envelope);
}

void DistributedMutex::unlock() {
    std::lock_guard<std::mutex> guard(this->mutex);
    this->grantedRequestNumbers[this->identity] = this->requestNumbers[this->identity];
    this->updateWaitingQueue();
    if (!this->waitingNodes.empty()) {
        this->SendPrivilege(this->waitingNodes.front());
    }

    this->isRequesting = false;
}

void DistributedMutex::SendPrivilege(const std::string &nextHolderIdentity) {
    this->hasPrivilege = false;
    auto privilegeMessage = std::make_unique<PrivilegeMessage>(this->UUID, this->waitingNodes, this->requestNumbers);
    Envelope envelope(move(privilegeMessage));
    messenger.send(nextHolderIdentity, envelope);
}

void DistributedMutex::updateWaitingQueue() {
    for (auto &&number : this->requestNumbers) {
        std::string identity = number.first;
        int requestNumber = number.second;
        if (identity != this->identity && !isNodeWaiting(identity) && requestNumber == grantedRequestNumbers[identity] + 1) {
            this->waitingNodes.push_back(identity);
        }
    }
}

bool DistributedMutex::isNodeWaiting(const std::string &identity) {
    return std::find(this->waitingNodes.begin(), this->waitingNodes.end(), identity) != std::end(this->waitingNodes);
}

void DistributedMutex::onPrivilegeReceived(const Envelope &envelope) {
    std::lock_guard<std::mutex> guard(this->mutex);
    if (envelope.getPayloadType() != MessageType::PRIVILEGE) {
        throw std::logic_error("Expected privilege message but got message of type: " + envelope.getPayloadType());
    }
    auto privilegeMessage = dynamic_cast<const PrivilegeMessage *>(envelope.getPayload());
    if (privilegeMessage->getmutexUUID() == this->UUID) {
        this->grantedRequestNumbers = privilegeMessage->getRequestNumbers();
        this->requestNumbers = privilegeMessage->getRequestNumbers();
        this->waitingNodes = privilegeMessage->getWaitingNodes();
        this->waitingForPrivilege.notify_one();
    }
}

void DistributedMutex::onRequestReceived(const Envelope &envelope) {
    std::lock_guard<std::mutex> guard(this->mutex);
    if (envelope.getPayloadType() != MessageType::REQUEST) {
        throw std::logic_error("Expected request message but got message of type: " + envelope.getPayloadType());
    }
    auto requestMessage = dynamic_cast<const RequestMessage *>(envelope.getPayload());
    const std::string &askingNode = envelope.getSender();
    if (this->identity != askingNode && requestMessage->getMutexUUID() == this->UUID) {
        int requestNumber = std::max(this->requestNumbers[askingNode], requestMessage->getRequestNumber());
        this->requestNumbers[askingNode] = requestNumber;
        if (this->hasPrivilege && !this->isRequesting && requestNumber == (this->grantedRequestNumbers[askingNode] + 1)) {
            this->SendPrivilege(askingNode);
        }
    }
}
