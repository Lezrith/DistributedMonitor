#include "DistributedConditionalVariable.h"

DistributedConditionalVariable::DistributedConditionalVariable(const std::shared_ptr<Messenger> &messenger, const sole::uuid &UUID, const std::shared_ptr<DistributedMutex> &mutex)
        : messenger(messenger), UUID(UUID), distributedMutex(mutex) {
    this->onWaitReceivedHandle = this->messenger->onReceive.subscribe(MessageType::WAIT, [=](const Envelope &envelope) { this->onWaitReceived(envelope); });
    this->onSignalReceivedHandle = this->messenger->onReceive.subscribe(MessageType::SIGNAL, [=](const Envelope &envelope) { this->onSignalReceived(envelope); });
}

DistributedConditionalVariable::~DistributedConditionalVariable() {
    this->messenger->onReceive.unsubscribe(*this->onWaitReceivedHandle);
    this->messenger->onReceive.unsubscribe(*this->onSignalReceivedHandle);
}

void DistributedConditionalVariable::wait() {
    // TODO
}

void DistributedConditionalVariable::signal() {
    // TODO
}

void DistributedConditionalVariable::signalAll() {
    // TODO
}

void DistributedConditionalVariable::onWaitReceived(const Envelope &envelope) {
    std::lock_guard<std::mutex> guard(this->mutex);
    if (envelope.getPayloadType() != MessageType::WAIT) {
        throw std::logic_error("Expected wait message but got message of type: " + envelope.getPayloadType());
    }
    auto waitMessage = dynamic_cast<const WaitMessage *>(envelope.getPayload());
    const std::string &sender = envelope.getSender();
    if (waitMessage->getConditionalVariableUUID() == this->UUID) {
        this->waitingNodes.push_back(sender);
    }
    std::unique_ptr<Message> ack = std::make_unique<AcknowledgeMessage>(waitMessage->getMessageUUID());
    this->messenger->send(sender, Envelope(ack));
}

void DistributedConditionalVariable::onSignalReceived(const Envelope &envelope) {
    std::lock_guard<std::mutex> guard(this->mutex);
    if (envelope.getPayloadType() != MessageType::SIGNAL) {
        throw std::logic_error("Expected signal message but got message of type: " + envelope.getPayloadType());
    }
    auto signalMessage = dynamic_cast<const SignalMessage *>(envelope.getPayload());
    const std::string &sender = envelope.getSender();
    if (signalMessage->getConditionalVariableUUID() == this->UUID) {
        this->waitingForSignal.notify_one();
    }
    auto iter = std::find(this->waitingNodes.begin(), this->waitingNodes.end(), this->messenger->getIdentity());
    this->waitingNodes.erase(iter);
    std::unique_ptr<Message> ack = std::make_unique<AcknowledgeMessage>(signalMessage->getMessageUUID());
    this->messenger->send(sender, Envelope(ack));
}


