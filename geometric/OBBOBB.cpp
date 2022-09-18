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

        // First search for a separating axis that is aligned with first OBB
        {
            Quaternion secondToFirstRotation = firstOBB.getRotation().getInverse() * secondOBB.getRotation();
            glm::bvec3 secondComponentWiseGreater(true); // Stays true if the second OBB is separated on the corresponding axis (x, y or z) by being greater
            glm::bvec3 secondComponentWiseSmaller(true); // Stays true if the second OBB is separated on the corresponding axis (x, y or z) by being smaller

            // Iterate over all the corner points of the second OBB (checking these, is enough)
            Vertex extremePoints[] = {secondAABB.getMinimum(), secondAABB.getMaximum()};
            for(int xi=0; xi<=1; xi++){
                for(int yi=0; yi<=1; yi++){
                    for(int zi=0; zi<=1; zi++){

                        // Rotate the corner point of the second OBB to the first OBBs coordinate system
                        Vertex rotatedCorner = secondToFirstRotation.rotateVertex({extremePoints[xi].x, extremePoints[yi].y, extremePoints[zi].z});

                        // Check if the corner point is separated on each axis (x, y or z)
                        secondComponentWiseGreater &= glm::greaterThan(rotatedCorner, firstAABB.getMaximum());
                        secondComponentWiseSmaller &= glm::lessThan(rotatedCorner, firstAABB.getMinimum());

//                        if(!(glm::any(secondComponentWiseGreater) || glm::any(secondComponentWiseSmaller))){ // TODO test if this actually improves performance, OBBs can be separated most of the time
//                            goto endFirstNestedLoop; // If the second OBB is not separated on any axis, we can stop the search
//                        }
                    }
                }
            }
            if(glm::any(secondComponentWiseGreater) || glm::any(secondComponentWiseSmaller)){
                return false;
            }
        }
//        endFirstNestedLoop:

        // Then search for a separating axis that is aligned with the second OBB
        {
            Quaternion firstToSecondRotation = secondOBB.getRotation().getInverse() * firstOBB.getRotation();
            glm::bvec3 firstComponentWiseGreater(true); // Stays true if the first OBB is separated on the corresponding axis (x, y or z) by being greater
            glm::bvec3 firstComponentWiseSmaller(true); // Stays true if the first OBB is separated on the corresponding axis (x, y or z) by being smaller

            // Iterate over all the corner points of the second OBB (checking these, is enough)
            Vertex extremePoints[] = {firstAABB.getMinimum(), firstAABB.getMaximum()};
            for(int xi=0; xi<=1; xi++) {
                for (int yi = 0; yi <= 1; yi++) {
                    for (int zi = 0; zi <= 1; zi++) {

                        Vertex rotatedCorner = firstToSecondRotation.rotateVertex({extremePoints[xi].x, extremePoints[yi].y, extremePoints[zi].z});
                        firstComponentWiseGreater &= glm::greaterThan(rotatedCorner, secondAABB.getMaximum());
                        firstComponentWiseSmaller &= glm::lessThan(rotatedCorner, secondAABB.getMinimum());

//                        if(!(glm::any(firstComponentWiseGreater) || glm::any(firstComponentWiseSmaller))){ // TODO test if this actually improves performance, OBBs can be separated most of the time
//                            goto endSecondNestedLoop;
//                        }
                    }
                }
            }
            if(glm::any(firstComponentWiseGreater) || glm::any(firstComponentWiseSmaller)){
                return false;
            }
        }
//        endSecondNestedLoop:
        return true;
    }
}