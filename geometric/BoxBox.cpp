//
// Created by Jonas on 5/11/2021.
//

#include <array>
#include "Intersection.h"

namespace Intersection {

    bool intersect(const AABB& firstAABB, const AABB& secondAABB){
        return firstAABB.getMinimum().x <= secondAABB.getMaximum().x &&
               firstAABB.getMinimum().y <= secondAABB.getMaximum().y &&
               firstAABB.getMinimum().z <= secondAABB.getMaximum().z &&
               secondAABB.getMinimum().x <= firstAABB.getMaximum().x &&
               secondAABB.getMinimum().y <= firstAABB.getMaximum().y &&
               secondAABB.getMinimum().z <= firstAABB.getMaximum().z;
    }

    bool intersect(const AABB& firstAABB, const AABB& secondAABB, const glm::mat4 &firstToSecondTransformationMatrix, const glm::mat4 &secondToFirstTransformationMatrix){
// See sketches on separating axis theorem for AABB's
        std::array<Vertex, 8> otherCorners{};
        otherCorners[0] = secondToFirstTransformationMatrix * glm::vec4(secondAABB.getMinimum().x, secondAABB.getMinimum().y, secondAABB.getMinimum().z, 1);
        otherCorners[1] = secondToFirstTransformationMatrix * glm::vec4(secondAABB.getMaximum().x, secondAABB.getMinimum().y, secondAABB.getMinimum().z, 1);
        otherCorners[2] = secondToFirstTransformationMatrix * glm::vec4(secondAABB.getMinimum().x, secondAABB.getMaximum().y, secondAABB.getMinimum().z, 1);
        otherCorners[3] = secondToFirstTransformationMatrix * glm::vec4(secondAABB.getMaximum().x, secondAABB.getMaximum().y, secondAABB.getMinimum().z, 1);
        otherCorners[4] = secondToFirstTransformationMatrix * glm::vec4(secondAABB.getMinimum().x, secondAABB.getMinimum().y, secondAABB.getMaximum().z, 1);
        otherCorners[5] = secondToFirstTransformationMatrix * glm::vec4(secondAABB.getMaximum().x, secondAABB.getMinimum().y, secondAABB.getMaximum().z, 1);
        otherCorners[6] = secondToFirstTransformationMatrix * glm::vec4(secondAABB.getMinimum().x, secondAABB.getMaximum().y, secondAABB.getMaximum().z, 1);
        otherCorners[7] = secondToFirstTransformationMatrix * glm::vec4(secondAABB.getMaximum().x, secondAABB.getMaximum().y, secondAABB.getMaximum().z, 1);

        glm::bvec3 otherComponentWiseLarger(true);
        glm::bvec3 otherComponentWiseSmaller(true);
        for(const auto& corner: otherCorners){
            otherComponentWiseLarger &= glm::greaterThan(corner, firstAABB.getMaximum());
            otherComponentWiseSmaller &= glm::lessThan(corner, firstAABB.getMinimum());
        }
        if(glm::any(otherComponentWiseLarger) || glm::any(otherComponentWiseSmaller)){
            return false;
        }

        std::array<Vertex, 8> thisCorners{};
        thisCorners[0] = firstToSecondTransformationMatrix * glm::vec4(firstAABB.getMinimum().x, firstAABB.getMinimum().y, firstAABB.getMinimum().z, 1);
        thisCorners[1] = firstToSecondTransformationMatrix * glm::vec4(firstAABB.getMaximum().x, firstAABB.getMinimum().y, firstAABB.getMinimum().z, 1);
        thisCorners[2] = firstToSecondTransformationMatrix * glm::vec4(firstAABB.getMinimum().x, firstAABB.getMaximum().y, firstAABB.getMinimum().z, 1);
        thisCorners[3] = firstToSecondTransformationMatrix * glm::vec4(firstAABB.getMaximum().x, firstAABB.getMaximum().y, firstAABB.getMinimum().z, 1);
        thisCorners[4] = firstToSecondTransformationMatrix * glm::vec4(firstAABB.getMinimum().x, firstAABB.getMinimum().y, firstAABB.getMaximum().z, 1);
        thisCorners[5] = firstToSecondTransformationMatrix * glm::vec4(firstAABB.getMaximum().x, firstAABB.getMinimum().y, firstAABB.getMaximum().z, 1);
        thisCorners[6] = firstToSecondTransformationMatrix * glm::vec4(firstAABB.getMinimum().x, firstAABB.getMaximum().y, firstAABB.getMaximum().z, 1);
        thisCorners[7] = firstToSecondTransformationMatrix * glm::vec4(firstAABB.getMaximum().x, firstAABB.getMaximum().y, firstAABB.getMaximum().z, 1);

        glm::bvec3 thisComponentWiseLarger(true);
        glm::bvec3 thisComponentWiseSmaller(true);
        for(const auto& corner: thisCorners){
            thisComponentWiseLarger &= glm::greaterThan(corner, secondAABB.getMaximum());
            thisComponentWiseSmaller &= glm::lessThan(corner, secondAABB.getMinimum());
        }
        if(glm::any(otherComponentWiseLarger) || glm::any(otherComponentWiseSmaller)){
            return false;
        }
        return true;
    }
}