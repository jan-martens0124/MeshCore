//
// Created by Jonas on 15/02/2022.
//

#ifndef MESHCORE_IO_H
#define MESHCORE_IO_H

#include <ostream>
#include "../core/Vertex.h"
#include "../core/Ray.h"
#include "../core/Transformation.h"
#include "../core/AABB.h"
#include "../core/VertexTriangle.h"


static std::ostream &operator<<(std::ostream &os, const Vertex &v) {
    return os << "(" << v.x << "," << v.y << "," << v.z << ")";
}

static std::ostream & operator<<(std::ostream & os, const Ray& ray){
    return os << "Ray(Origin" << ray.origin << ", Direction" << ray.direction << ")";
}

static std::ostream& operator<<(std::ostream& o, const Transformation& transformation) {
    o << "\tScale: " << transformation.getScale() << "\n";
    o << "\tPosition: " << transformation.getPosition() << "\n";
    o << "\tYaw: " << transformation.getYaw() << "\n";
    o << "\tPitch: " << transformation.getPitch() << "\n";
    o << "\tRoll: " << transformation.getRoll() << "\n";
    glm::mat4 transform = transformation.getMatrix();
    o << "\n\t[" << transform.operator[](0) << "\n\t " << transform.operator[](1) << "\n\t " << transform.operator[](2) << "\n\t " << transform.operator[](3) << ']';
    return o;
}

static std::ostream & operator<<(std::ostream & os, const AABB& aabb){
    return os << "AABB[min" << aabb.getMinimum() << ";max" << aabb.getMaximum() << "]";
}

static std::ostream& operator<<(std::ostream& o, const VertexTriangle& vertexTriangle) {
    return o << "VertexTriangle(" << vertexTriangle.vertex0 << ", " << vertexTriangle.vertex1 << ", " << vertexTriangle.vertex2 << ")";
}

#endif //MESHCORE_IO_H
