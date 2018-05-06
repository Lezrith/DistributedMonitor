#ifndef DISTRIBUTEDMONITOR_MONITORDEMO_H
#define DISTRIBUTEDMONITOR_MONITORDEMO_H

#include <string>
#include <map>
#include <fstream>
#include <zmq.hpp>
#include "Core/DistributedMonitor.h"
#include "Demo/IntVectorSerializer.h"
#include "Core/Messenger.h"

void monitorDemo(const std::string &configFile, const std::string identity);

#endif //DISTRIBUTEDMONITOR_MONITORDEMO_H
