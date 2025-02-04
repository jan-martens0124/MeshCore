//
// Created by Jonas Tollenaere on 23/01/2025.
//

#ifndef EXTENDEDMESHCORE_INDEXFACE_H
#define EXTENDEDMESHCORE_INDEXFACE_H

#include <vector>

struct IndexFace {
    const std::vector<size_t> vertexIndices;
    explicit IndexFace(std::vector<size_t> vertexIndices): vertexIndices(std::move(vertexIndices)) {}
};

#endif //EXTENDEDMESHCORE_INDEXFACE_H
