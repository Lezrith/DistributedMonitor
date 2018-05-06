#include "main.h"

int main(int argc, char *argv[]) {
    try {
        if (argc < 3) {
            LoggerSingleton::getInstance()->log(NORMAL, "Not enough params. USAGE: DistributedMonitor <configFile> <nodeIdentity>");
            return -1;
        }
        if (argc >= 4 && std::string(argv[3]) == "DEBUG") {
            LoggerSingleton::getInstance()->setLogLevel(DEBUG);
        }
        std::string configPath = argv[1];
        std::string identity = argv[2];
        monitorDemo(configPath, identity);
    } catch (const std::exception &ex) {
        LoggerSingleton::getInstance()->log(DEBUG, ex.what());
    }
    return 0;
}
