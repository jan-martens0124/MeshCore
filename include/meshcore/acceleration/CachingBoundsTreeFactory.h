//
// Created by tolle on 4/02/2023.
//

#ifndef MESHCORE_CACHINGBOUNDSTREEFACTORY_H
#define MESHCORE_CACHINGBOUNDSTREEFACTORY_H

#include <unordered_map>
#include <mutex>
#include <memory>
#include <functional>
#include <shared_mutex>
#include "AbstractBoundsTree.h"

template<class Tree>
class CachingBoundsTreeFactory {
public:
    static std::shared_ptr<Tree> getBoundsTree(const std::shared_ptr<ModelSpaceMesh> &modelSpaceMesh) {

        static std::shared_mutex cacheMapMutex;
        static std::unordered_map<std::shared_ptr<ModelSpaceMesh>, std::shared_ptr<Tree>> cacheMap; // Maybe a weak pointer would be nicer here, but gives hash problems as key value

        // 1. Check if cached already
        {
            std::shared_lock<std::shared_mutex> lock(cacheMapMutex);
            const auto cacheIterator = cacheMap.find(modelSpaceMesh);
            if (cacheIterator != cacheMap.end()) {
                return cacheIterator->second;
            }
        }

        // 2. Create a new tree
        {
            auto tree = std::make_shared<Tree>(modelSpaceMesh);
            std::unique_lock<std::shared_mutex> unique_lock(cacheMapMutex);
            const auto cacheIterator = cacheMap.find(modelSpaceMesh); // Check again if already added by another thread in the meantime
            if (cacheIterator != cacheMap.end()) {
                tree = cacheIterator->second; // Don't overwrite if already added by another thread in the meantime
            }
            else {
                cacheMap[modelSpaceMesh] = tree;
            }
            return tree;
        }
    };
};

#endif //MESHCORE_CACHINGBOUNDSTREEFACTORY_H
