//
// Created by Jonas Tollenaere on 20/01/2025.
//

#include "Intersection.h"

namespace Intersection {

    std::optional<Line> intersect(const Plane& planeA, const Plane& planeB) {

        // Calculate the direction of the intersection line
        auto direction = glm::cross(planeA.getNormal(), planeB.getNormal());
        auto directionLength = glm::length(direction);

        // If the length is zero, the planes are parallel
        if(directionLength < 1e-6f){
            return std::nullopt;
        }

        float dA = planeA.getD();
        float dB = planeB.getD();

        float a1 = planeA.getNormal().x;
        float b1 = planeA.getNormal().y;
        float c1 = planeA.getNormal().z;
        float a2 = planeB.getNormal().x;
        float b2 = planeB.getNormal().y;
        float c2 = planeB.getNormal().z;

        float x, y, z;
        if (std::abs(direction.z) >= std::abs(direction.x) && std::abs(direction.z) >= std::abs(direction.y)) {
            z = 0;
            float tmp = 1 / (a1*b2 - a2 * b1);
            x = (b1 * dB - b2 * dA) * tmp;
            y = (a2 * dA - a1 * dB) * tmp;
        }
        else if (std::abs(direction.x) >= std::abs(direction.y) && std::abs(direction.x) >= std::abs(direction.z)) {
            x = 0;
            float tmp = 1 / (b1*c2 - b2 * c1);
            y = (c1 * dB - c2 * dA) * tmp;
            z = (b2 * dA - b1 * dB) * tmp;
        }
        else {
            assert(abs(direction.y) >= abs(direction.x) && abs(direction.y) >= abs(direction.z));
            y = 0;
            float tmp = 1 / (a1*c2 - a2 * c1);
            x = (c1 * dB - c2 * dA) * tmp;
            z = (a2 * dA - a1 * dB) * tmp;
        }

        auto pointOnLine = glm::vec3(x, y, z);

        // Return the resulting line
        assert(planeA.distance(pointOnLine) < 1e-5f);
        assert(planeB.distance(pointOnLine) < 1e-5f);
        return Line(pointOnLine, direction/directionLength);
    }

    std::optional<glm::vec3> intersect(const Plane& plane, const Line& line) {
        float denominator = glm::dot(plane.getNormal(), line.getDirection());

        // If the denominator is zero, the line is parallel to the plane
        if(glm::abs(denominator) < 1e-6f){
            return std::nullopt;
        }

        float t = - (glm::dot(plane.getNormal(), line.getPoint()) + plane.getD()) / denominator;
        return line.getPoint() + t * line.getDirection();
    }

    std::optional<glm::vec3> intersect(const Plane& planeA, const Plane& planeB, const Plane& planeC) {

        auto line = intersect(planeA, planeB);
        if(!line.has_value()){
            return std::nullopt;
        }

        return intersect(planeC, line.value());
    }
}