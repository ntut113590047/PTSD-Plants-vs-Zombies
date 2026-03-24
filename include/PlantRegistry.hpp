#ifndef PLANTREGISTRY_HPP
#define PLANTREGISTRY_HPP

#include <map>
#include <string>
#include <vector>
#include "PlantCard.hpp"

class PlantRegistry {
public:
    static PlantRegistry& GetInstance();

    PlantData GetPlantData(const std::string& plantName) const;

private:
    PlantRegistry();
    std::map<std::string, PlantData> m_PlantDataMap;
};

#endif