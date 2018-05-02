#include "DistributedConditionalVariable.h"

DistributedConditionalVariable::DistributedConditionalVariable(const std::shared_ptr<Messenger> &messenger, const sole::uuid &UUID, const std::shared_ptr<DistributedMutex> &mutex)
        : messenger(messenger), UUID(UUID), distributedMutex(mutex) {
    this->onWaitReceivedHandle = this->messenger->onReceive.subscribe(MessageType::WAIT, [=](const Envelope &envelope) { this->onWaitReceived(envelope); });
    this->onSignalReceivedHandle = this->messenger->onReceive.subscribe(MessageType::SIGNAL, [=](const Envelope &envelope) { this->onSignalReceived(envelope); });
    this->onWaitEndReceivedHandle = this->messenger->onReceive.subscribe(MessageType::WAIT_END, [=](const Envelope &envelope) { this->onWaitEndReceived(envelope); });
}

DistributedConditionalVariable::~DistributedConditionalVariable() {
    this->messenger->onReceive.unsubscribe(*this->onWaitReceivedHandle);
    this->messenger->onReceive.unsubscribe(*this->onSignalReceivedHandle);
    this->messenger->onReceive.unsubscribe(*this->onWaitEndReceivedHandle);
}

void DistributedConditionalVariable::wait() {
    std::unique_lock<std::mutex> lock(this->mutex);
    this->sendWait();

    this->distributedMutex->unlock();
    LoggerSingleton::getInstance()->log("Waiting for signal");
    this->waitingForSignal.wait(lock);
    this->distributedMutex->lock();

    LoggerSingleton::getInstance()->log("Woke up");
}

void DistributedConditionalVariable::signal() {
    std::lock_guard<std::mutex> guard(this->mutex);

    if (!this->waitingNodes.empty()) {
        std::string nodeToSignal = this->waitingNodes.front();
        this->sendWaitEnd(nodeToSignal);
        std::unique_ptr<Message> signalMessage = std::make_unique<SignalMessage>(this->UUID);
        this->messenger->send(nodeToSignal, Envelope(signalMessage));
    }
}

void DistributedConditionalVariable::signalAll() {
    std::lock_guard<std::mutex> guard(this->mutex);

    if (!this->waitingNodes.empty()) {
        for (auto &&node : this->waitingNodes) {
            this->sendWaitEnd(node);
        }
        std::unique_ptr<Message> signalMessage = std::make_unique<SignalMessage>(this->UUID);
        for (auto &&node : this->waitingNodes) {
            this->messenger->send(node, Envelope(signalMessage));
        }
    }
}

void DistributedConditionalVariable::onWaitReceived(const Envelope &envelope) {
    const std::string &sender = envelope.getSender();
    if (sender != this->messenger->getIdentity()) {
        std::lock_guard<std::mutex> guard(this->mutex);
        if (envelope.getPayloadType() != MessageType::WAIT) {
            throw std::logic_error("Expected wait message but got message of type: " + envelope.getPayloadType());
        }
        auto waitMessage = dynamic_cast<const WaitMessage *>(envelope.getPayload());
        if (waitMessage->getConditionalVariableUUID() == this->UUID) {
            this->waitingNodes.push_back(sender);
        }
        std::unique_ptr<Message> ack = std::make_unique<AcknowledgeMessage>(waitMessage->getMessageUUID());
        this->messenger->send(sender, Envelope(ack));
    }
}

void DistributedConditionalVariable::onSignalReceived(const Envelope &envelope) {
    const std::string &sender = envelope.getSender();
    if (!this->isWaiting) {
        throw std::logic_error("Received signal while not waiting");
    }
    if (sender != this->messenger->getIdentity()) {
        std::lock_guard<std::mutex> guard(this->mutex);
        if (envelope.getPayloadType() != MessageType::SIGNAL) {
            throw std::logic_error("Expected signal message but got message of type: " + envelope.getPayloadType());
        }
        auto signalMessage = dynamic_cast<const SignalMessage *>(envelope.getPayload());
        if (signalMessage->getConditionalVariableUUID() == this->UUID) {
            this->waitingForSignal.notify_one();
        }
    }
}

void DistributedConditionalVariable::onWaitEndReceived(const Envelope &envelope) {
    const std::string &sender = envelope.getSender();
    if (sender != this->messenger->getIdentity()) {
        std::lock_guard<std::mutex> guard(this->mutex);
        if (envelope.getPayloadType() != MessageType::WAIT_END) {
            throw std::logic_error("Expected wait_end message but got message of type: " + envelope.getPayloadType());
        }
        auto waitEndMessage = dynamic_cast<const WaitEndMessage *>(envelope.getPayload());
        if (waitEndMessage->getConditionalVariableUUID() == this->UUID) {
            if (waitEndMessage->getIdentity() != this->messenger->getIdentity()) {
                this->removeWaitingNode(waitEndMessage->getIdentity());
            }
            std::unique_ptr<Message> ack = std::make_unique<AcknowledgeMessage>(waitEndMessage->getMessageUUID());
            this->messenger->send(sender, Envelope(ack));
        }
    }
}

void DistributedConditionalVariable::sendWait() {
    sole::uuid waitUUID = sole::uuid0();
    std::unique_ptr<Message> waitMessage = std::make_unique<WaitMessage>(this->UUID, waitUUID);
    this->messenger->sendBroadcastWithACK(Envelope(waitMessage), waitUUID);
    this->isWaiting = true;
}

void DistributedConditionalVariable::sendWaitEnd(std::string identity) {
    sole::uuid waitEndUUID = sole::uuid0();
    std::unique_ptr<Message> waitEndMessage = std::make_unique<WaitEndMessage>(identity, this->UUID, waitEndUUID);
    this->messenger->sendBroadcastWithACK(Envelope(waitEndMessage), waitEndUUID);
    this->removeWaitingNode(identity);
    this->isWaiting = false;
}

void DistributedConditionalVariable::removeWaitingNode(const std::string &identity) {
    auto iter = std::find(this->waitingNodes.begin(), this->waitingNodes.end(), identity);
    if (iter != this->waitingNodes.end()) {
        this->waitingNodes.erase(iter);
    } else {
        throw std::logic_error("Received wait_end but node is not in queue");
    }
}


