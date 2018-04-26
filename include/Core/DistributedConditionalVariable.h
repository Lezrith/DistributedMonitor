#ifndef DISTRIBUTEDMONITOR_DISTRIBUTEDCONDITIONALVARIABLE_H
#define DISTRIBUTEDMONITOR_DISTRIBUTEDCONDITIONALVARIABLE_H


#include <string>
#include <deque>
#include <memory>
#include "Utils/Sole/sole.h"
#include "DistributedMutex.h"
#include "Messages/WaitMessage.h"
#include "Messages/SignalMessage.h"

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
    std::shared_ptr<DistributedMutex> distributedMutex;
    std::mutex mutex;
    std::condition_variable waitingForSignal;
    CallbackWrapper<Envelope> *onWaitReceivedHandle;
    CallbackWrapper<Envelope> *onSignalReceivedHandle;
    CallbackWrapper<Envelope> *onWaitEndReceivedHandle;

    void onWaitReceived(const Envelope &envelope);

    void onSignalReceived(const Envelope &envelope);

    void onWaitEndReceived(const Envelope &envelope);
};


#endif //DISTRIBUTEDMONITOR_DISTRIBUTEDCONDITIONALVARIABLE_H
