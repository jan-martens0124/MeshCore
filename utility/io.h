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
#include "../core/OBB.h"
#include "../core/Sphere.h"


static std::ostream &operator<<(std::ostream &os, const Vertex &v) {
    return os << "(" << v.x << "," << v.y << "," << v.z << ")";
}

static std::ostream & operator<<(std::ostream & os, const Ray& ray){
    return os << "Ray(Origin" << ray.origin << ", Direction " << ray.direction << ")";
}

static std::ostream & operator<<(std::ostream & os, const Sphere& sphere){
    return os << "Sphere(Center" << sphere.getCenter() << ", Radius " << sphere.getRadius() << ")";
}

static std::ostream& operator<<(std::ostream& o, const Transformation& transformation) {
    o << "\tScale: " << transformation.getScale() << "\n";
    o << "\tPosition: " << transformation.getPosition() << "\n";
    o << "\tRotation: quat(" << transformation.getRotation().w << "," << transformation.getRotation().x << "," << transformation.getRotation().y << "," << transformation.getRotation().z << ")\n";
    auto transform = transformation.getMatrix();
    o << "\t[" << transform.operator[](0).x << "," << transform.operator[](1).x << "," << transform.operator[](2).x << "," << transform.operator[](3).x << "\n\t"
                 << transform.operator[](0).y << "," << transform.operator[](1).y << "," << transform.operator[](2).y << "," << transform.operator[](3).y << "\n\t"
                 << transform.operator[](0).z << "," << transform.operator[](1).z << "," << transform.operator[](2).z << "," << transform.operator[](3).z << "\n\t"
                 << transform.operator[](0).w << "," << transform.operator[](1).w << "," << transform.operator[](2).w << "," << transform.operator[](3).w
    << "]\n";
    return o;
}

static std::ostream & operator<<(std::ostream & os, const AABB& aabb){
    return os << "AABB[min" << aabb.getMinimum() << ";max" << aabb.getMaximum() << "]";
}

static std::ostream & operator<<(std::ostream & os, const OBB& obb){
    return os << "OBB[AABB[" << obb.getAabb() << "]; Quaternion[" << obb.getRotation().x << "," << obb.getRotation().y << "," << obb.getRotation().z << "," << obb.getRotation().w << "]]";
}

static std::ostream& operator<<(std::ostream& o, const VertexTriangle& vertexTriangle) {
    return o << "VertexTriangle(" << vertexTriangle.vertices[0] << ", " << vertexTriangle.vertices[1] << ", " << vertexTriangle.vertices[2] << ")";
}

#endif //MESHCORE_IO_H
