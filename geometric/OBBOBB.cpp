//
// Created by Jonas on 15/03/2022.
//

#include "Intersection.h"

namespace Intersection {

    bool intersect(const OBB& obb, const VertexTriangle& vertexTriangle){
        return intersect(obb.getAabb(), vertexTriangle.getRotated(obb.getRotation().getInverse()));
    }

    bool intersect(const OBB& obb, const Ray& ray){
        return intersect(obb.getAabb(), ray.getRotated(obb.getRotation().getInverse()));
    }

    bool intersect(const OBB& firstOBB, const OBB& secondOBB){

        // See sketches on separating axis theorem for AABB's
        const auto& firstAABB = firstOBB.getAabb();
        const auto& secondAABB = secondOBB.getAabb();

        // First search for a separating axis aligned with first OBB
        {
//            glm::mat4 secondToFirstTransformationMatrix = secondOBB.getRotation().getMatrix() * firstOBB.getRotation().getInverseMatrix();
            Quaternion secondToFirstRotation = secondOBB.getRotation() * firstOBB.getRotation().getInverse();
            glm::bvec3 secondComponentWiseGreater(true); // Stays true if the second OBB is separated (greater) on the corresponding axis (x, y or z)
            glm::bvec3 secondComponentWiseSmaller(true); // Stays true if the second OBB is separated (smaller) on the corresponding axis (x, y or z)

            // Iterate over all the corner points of the second OBB (checking these is enough)
            Vertex extremePoints[] = {secondAABB.getMinimum(), secondAABB.getMaximum()};
            for(int xi=0; xi<=1; xi++){
                for(int yi=0; yi<=1; yi++){
                    for(int zi=0; zi<=1; zi++){

                        Vertex corner = secondToFirstRotation * extremePoints[xi];

                        secondComponentWiseGreater &= glm::greaterThan(corner, firstAABB.getMaximum());
                        secondComponentWiseSmaller &= glm::lessThan(corner, firstAABB.getMinimum());

                        if(!(glm::any(secondComponentWiseGreater) || glm::any(secondComponentWiseSmaller))){
                            goto endFirstNestedLoop;
                        }
                    }
                }
            }
            if(glm::any(secondComponentWiseGreater) || glm::any(secondComponentWiseSmaller)){
                return false;
            }
        }
        endFirstNestedLoop:

        // Then search for a separating axis aligned with the second OBB
        {
            // TODO verify if the quaternion approach reaches the same result
//            glm::mat4 firstToSecondTransformationMatrix = firstOBB.getRotation().getMatrix() * secondOBB.getRotation().getInverseMatrix();
//
            Quaternion firstToSecondRotation = firstOBB.getRotation() * secondOBB.getRotation().getInverse();
            glm::bvec3 firstComponentWiseGreater(true); // Stays true if the first OBB is separated (greater) on the corresponding axis (x, y or z)
            glm::bvec3 firstComponentWiseSmaller(true); // Stays true if the first OBB is separated (smaller) on the corresponding axis (x, y or z)

            // Iterate over all the corner points of the second OBB (checking these is enough)
            Vertex extremePoints[] = {firstAABB.getMinimum(), firstAABB.getMaximum()};
            for(int xi=0; xi<=1; xi++) {
                for (int yi = 0; yi <= 1; yi++) {
                    for (int zi = 0; zi <= 1; zi++) {

                        Vertex corner = firstToSecondRotation * extremePoints[xi];
                        firstComponentWiseGreater &= glm::greaterThan(corner, secondAABB.getMaximum());
                        firstComponentWiseSmaller &= glm::lessThan(corner, secondAABB.getMinimum());

                        if(!(glm::any(firstComponentWiseGreater) || glm::any(firstComponentWiseSmaller))){
                            goto endSecondNestedLoop;
                        }
                    }
                }
            }
            if(glm::any(firstComponentWiseGreater) || glm::any(firstComponentWiseSmaller)){
                return false;
            }
        }
        endSecondNestedLoop:
        return true;
    }
}