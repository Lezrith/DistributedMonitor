#include "Demo/IntVectorSerializer.h"

std::string IntVectorSerializer::serialize(const std::vector<int> &object) {
    std::stringstream ss;
    for (auto &&item : object) {
        ss << item << ";";
    }
    return ss.str();
}

std::unique_ptr<std::vector<int>> IntVectorSerializer::deserialize(const std::string &string) {
    auto result = std::make_unique<std::vector<int>>();
    std::stringstream ss(string);
    std::string number;
    while (getline(ss, number, ';')) {
        result->emplace_back(std::stoi(number));
    }
    return result;
}
