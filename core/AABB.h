//
// Created by Jonas on 7/10/2021.
//

#ifndef MESHCORE_AABB_H
#define MESHCORE_AABB_H


#include <vector>
#include "Vertex.h"
#include "Ray.h"

class VertexTriangle;

/** Axis Aligned Bounding Box **/
class AABB {
private:
    glm::vec3 minimum;
    glm::vec3 maximum;
public:
    explicit AABB(const std::vector<Vertex>& vertices);
    explicit AABB(const VertexTriangle& vertexTriangle);
    explicit AABB(const std::vector<VertexTriangle>& vertexTriangles);
    AABB(glm::vec3 minimum, glm::vec3 maximum);
    [[nodiscard]] glm::vec3 getMinimum() const;
    [[nodiscard]] glm::vec3 getMaximum() const;
    [[nodiscard]] glm::vec3 getCenter() const;
    [[nodiscard]] glm::vec3 getHalf() const;
};

std::ostream & operator<<(std::ostream & os, const AABB& aabb);

#endif //MESHCORE_AABB_H
