#ifndef DISTRIBUTEDMONITOR_CALLBACKREPOSITORY_H
#define DISTRIBUTEDMONITOR_CALLBACKREPOSITORY_H


#include <map>
#include <algorithm>
#include <mutex>
#include <vector>
#include "Messages/Envelope.h"
#include "Utils/Sole/sole.h"
#include "Messages/Message.h"
#include "Messages/MessageType.h"
#include "CallbackWrapper.h"

template<typename T, typename U>
class CallbackRepository {
    typedef std::vector<CallbackWrapper<U>> CallbackVector;
public:
    CallbackWrapper<U> *subscribe(T key, const std::function<void(const U &)> &callback) {
        std::lock_guard<std::mutex> guard(this->mutex);
        auto iter = this->callbackVectors.find(key);
        if (iter == this->callbackVectors.end()) {
            this->callbackVectors.insert(std::pair<T, CallbackVector>(key, CallbackVector()));
            iter = this->callbackVectors.find(key);
        }
        CallbackWrapper<U> wrapper(callback);
        iter->second.push_back(wrapper);
        return &(*iter->second.end());
    }

    void unsubscribe(const CallbackWrapper<U> &callback) {
        std::lock_guard<std::mutex> guard(this->mutex);
        for (auto &&kv : callbackVectors) {
            CallbackVector vector = kv.second;
            vector.erase(std::remove(vector.begin(), vector.end(), callback), vector.end());
        }
    }

    void notifySubscribers(T key, const U &event) {
        auto iter = this->callbackVectors.find(key);
        if (iter != this->callbackVectors.end()) {
            auto callbacks = iter->second;
            for (auto &&callback : callbacks) {
                callback(event);
            }
        }
    }

    std::vector<CallbackWrapper<U>> getCallbacks(T key) {
        std::lock_guard<std::mutex> guard(this->mutex);
        auto iter = this->callbackVectors.find(key);
        if (iter != this->callbackVectors.end()) {
            return iter->second;
        } else {
            return std::vector<CallbackWrapper<U>>();
        }
    }

private:
    std::mutex mutex;
    std::map<T, CallbackVector> callbackVectors;
};


#endif //DISTRIBUTEDMONITOR_CALLBACKREPOSITORY_H
