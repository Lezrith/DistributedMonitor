#ifndef DISTRIBUTEDMONITOR_DISTRIBUTEDMONITOR_H
#define DISTRIBUTEDMONITOR_DISTRIBUTEDMONITOR_H


#include <string>
#include <map>
#include <condition_variable>
#include <mutex>
#include <functional>
#include "Messenger.h"
#include "Messages/Envelope.h"
#include "Messages/RequestMessage.h"

class DistributedMutex {
public:
    explicit DistributedMutex(std::shared_ptr<Messenger> messenger, const sole::uuid &UUID);

    virtual ~DistributedMutex();

    void lock();

    void unlock();

private:
    sole::uuid UUID;
    std::shared_ptr<Messenger> messenger;
    std::map<std::string, int> requestNumbers; // RN
    std::map<std::string, int> grantedRequestNumbers; // LN
    std::deque<std::string> waitingNodes;
    std::mutex mutex;
    std::condition_variable waitingForPrivilege;
    bool isRequesting;
    bool hasPrivilege;
    std::string identity;
    CallbackWrapper<Envelope> onPrivilegeReceivedHandle;
    CallbackWrapper<Envelope> onRequestReceivedHandle;

    void onPrivilegeReceived(const Envelope &envelope);

    void onRequestReceived(const Envelope &envelope);

    bool isNodeWaiting(const std::string &identity);

    void updateWaitingQueue();

    void sendPrivilege(const std::string &nextHolderIdentity);

    void requestPrivilege();
};


#endif //DISTRIBUTEDMONITOR_DISTRIBUTEDMONITOR_H
