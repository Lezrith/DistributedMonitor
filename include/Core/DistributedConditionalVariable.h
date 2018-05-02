#ifndef DISTRIBUTEDMONITOR_DISTRIBUTEDCONDITIONALVARIABLE_H
#define DISTRIBUTEDMONITOR_DISTRIBUTEDCONDITIONALVARIABLE_H


#include <string>
#include <deque>
#include <memory>
#include <atomic>
#include "Utils/Sole/sole.h"
#include "DistributedMutex.h"
#include "Messages/WaitMessage.h"
#include "Messages/SignalMessage.h"
#include "Messages/WaitEndMessage.h"

class DistributedConditionalVariable {
public:
    DistributedConditionalVariable(const std::shared_ptr<Messenger> &messenger, const sole::uuid &UUID, const std::shared_ptr<DistributedMutex> &distributedMutex);

    void wait();

    void signal();

    void signalAll();

    virtual ~DistributedConditionalVariable();

private:
    std::shared_ptr<Messenger> messenger;
    sole::uuid UUID;
    std::deque<std::string> waitingNodes;
    std::atomic<bool> isWaiting;
    std::shared_ptr<DistributedMutex> distributedMutex;
    std::mutex mutex;
    std::condition_variable waitingForSignal;
    CallbackWrapper<Envelope> *onWaitReceivedHandle;
    CallbackWrapper<Envelope> *onSignalReceivedHandle;
    CallbackWrapper<Envelope> *onWaitEndReceivedHandle;

    void onWaitReceived(const Envelope &envelope);

    void onSignalReceived(const Envelope &envelope);

    void onWaitEndReceived(const Envelope &envelope);

    void sendWait();

    void sendWaitEnd(std::string identity);

    void removeWaitingNode(const std::string &identity);
};


#endif //DISTRIBUTEDMONITOR_DISTRIBUTEDCONDITIONALVARIABLE_H
