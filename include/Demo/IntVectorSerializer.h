#ifndef DISTRIBUTEDMONITOR_INTVECTORSERIALIZER_H
#define DISTRIBUTEDMONITOR_INTVECTORSERIALIZER_H

#include <vector>
#include <sstream>
#include "Serializers/Serializer.h"

class IntVectorSerializer : public Serializer<std::vector<int>> {
public:
    std::string serialize(const std::vector<int> &object) override;

    std::unique_ptr<std::vector<int>> deserialize(const std::string &string) override;
};


#endif //DISTRIBUTEDMONITOR_INTVECTORSERIALIZER_H
