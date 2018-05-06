#ifndef _DISTRIBUTEDMONITOR_DISTRIBUTEDMONITOR_H
#define _DISTRIBUTEDMONITOR_DISTRIBUTEDMONITOR_H


#include <memory>
#include <type_traits>
#include <functional>
#include "Messages/StateMessage.h"
#include "Utils/Sole/sole.h"
#include "DistributedConditionalVariable.h"
#include "DistributedMutex.h"
#include "Serializers/Serializer.h"
#include "Messenger.h"

using namespace std::placeholders;

template<typename State, typename StateSerializer>
class DistributedMonitor {
    static_assert(std::is_base_of<Serializer<State>, StateSerializer>(), "StateSerializer should derive from Serializer");
public:

    DistributedMonitor(std::shared_ptr<Messenger> messenger, sole::uuid monitorUUID) : messenger(messenger), UUID(monitorUUID),
                                                                                       state(std::make_shared<State>()),
                                                                                       onStateReceivedHandle(messenger->onReceive.subscribe(MessageType::STATE,
                                                                                                                                            std::bind(
                                                                                                                                                    &DistributedMonitor::onStateReceived,
                                                                                                                                                    this, _1))) {
        sole::uuid mutexUUID(monitorUUID);
        mutexUUID.ab++;
        sole::uuid cvUUID(monitorUUID);
        cvUUID.ab--;
        this->distributedMutex = std::make_shared<DistributedMutex>(messenger, mutexUUID);
        this->distributedConditionalVariable = std::make_unique<DistributedConditionalVariable>(messenger, cvUUID, this->distributedMutex);
    }

    template<typename Args>
    DistributedMonitor(std::shared_ptr<Messenger> messenger, sole::uuid monitorUUID, Args &&args) : messenger(messenger), UUID(monitorUUID),
                                                                                                    state(std::make_shared<State>(std::forward<Args>(args))),
                                                                                                    onStateReceivedHandle(messenger->onReceive.subscribe(MessageType::STATE,
                                                                                                                                                         [=](const Envelope &envelope) {
                                                                                                                                                             this->onStateReceived(
                                                                                                                                                                     envelope);
                                                                                                                                                         })) {
        sole::uuid mutexUUID(monitorUUID);
        mutexUUID.ab++;
        sole::uuid cvUUID(monitorUUID);
        cvUUID.ab--;
        this->distributedMutex = std::make_shared<DistributedMutex>(messenger, mutexUUID);
        this->distributedConditionalVariable = std::make_unique<DistributedConditionalVariable>(messenger, cvUUID, this->distributedMutex);
    }

    virtual ~DistributedMonitor() {
        if (this->messenger != nullptr) {
            this->messenger->onReceive.unsubscribe(this->onStateReceivedHandle);
        }
    }

    const std::shared_ptr<State> &getStateUnsafe() const {
        return state;
    }


    struct MonitorHelper {
    public:
        explicit MonitorHelper(DistributedMonitor *monitor) : monitor(monitor) {
            monitor->distributedMutex->lock();
        }

        virtual ~MonitorHelper() {
            monitor->sendState();
            monitor->distributedMutex->unlock();
        }

        std::shared_ptr<State> operator->() {
            return monitor->state;
        };

        void wait() {
            monitor->distributedConditionalVariable->wait();
        }

        void signal() {
            monitor->distributedConditionalVariable->signal();
        }

        void signalAll() {
            monitor->distributedConditionalVariable->signalAll();
        }

    private:
        DistributedMonitor *monitor;
    };

    MonitorHelper operator->() {
        return MonitorHelper(this);
    }

    MonitorHelper manuallyLock() {
        return MonitorHelper(this);
    }

private:
    std::shared_ptr<Messenger> messenger;
    sole::uuid UUID;
    std::shared_ptr<State> state;
    std::mutex mutex;
    std::shared_ptr<DistributedMutex> distributedMutex{};
    std::unique_ptr<DistributedConditionalVariable> distributedConditionalVariable;
    CallbackWrapper<Envelope> onStateReceivedHandle;

    void sendState() {
        std::lock_guard<std::mutex> guard(this->mutex);
        StateSerializer serializer;
        std::string serializedState = serializer.serialize(*this->state);
        auto messageUUID = sole::uuid0();
        std::unique_ptr<Message> stateMessage = std::make_unique<StateMessage>(this->UUID, serializedState, messageUUID);
        this->messenger->sendBroadcastWithACK(Envelope(stateMessage), messageUUID);
        LoggerSingleton::getInstance()->log(DEBUG, "Sent state update to peers");
    }

    void onStateReceived(const Envelope &envelope) {
        std::string sender = envelope.getSender();
        if (sender != this->messenger->getIdentity()) {
            std::lock_guard<std::mutex> guard(this->mutex);
            if (envelope.getPayloadType() != MessageType::STATE) {
                throw std::logic_error("Expected state message but got message of type: " + envelope.getPayloadType());
            }
            auto stateMessage = dynamic_cast<const StateMessage *>(envelope.getPayload());
            if (stateMessage->getMonitorUUID() == this->UUID) {
                StateSerializer serializer;
                this->state = serializer.deserialize(stateMessage->getSerializedState());
                LoggerSingleton::getInstance()->log(DEBUG, "Received state update from peer");
                std::unique_ptr<Message> ack = std::make_unique<AcknowledgeMessage>(stateMessage->getMessageUUID());
                this->messenger->send(sender, Envelope(ack));
            }
        }
    }
};


#endif //_DISTRIBUTEDMONITOR_DISTRIBUTEDMONITOR_H
