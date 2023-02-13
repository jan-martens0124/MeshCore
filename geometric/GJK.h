//
// File created by Jonas on 15/06/2022. Core algorithm mostly copied from an implementation by Wim Vancroonenburg
//

#ifndef OPTIXMESHCORE_GJK_H
#define OPTIXMESHCORE_GJK_H

#include <set>
#include "../core/AABB.h"
#include "../core/OBB.h"
#include "../core/WorldSpaceMesh.h"
#include "../core/VertexTriangle.h"
#include "../core/Sphere.h"

#define GJK_EPSILON 0.00001f
#define GJK_STEEPEST_DESCENT true

class GJKConvexShape {
public:
    [[nodiscard]] virtual glm::vec3 computeSupport(const glm::vec3& direction) const = 0;
};

class GJK {
public:
    static std::optional<float> computeDistanceSqr(const GJKConvexShape &convex_shape_A, const GJKConvexShape &convex_shape_B, const glm::vec3& initialDir);

private:
    static glm::vec3 support(const GJKConvexShape& convex_shape_A, const GJKConvexShape& convex_shape_B, const glm::vec3& D);

    static std::optional<glm::vec3> doSimplex(std::vector<glm::vec3>& simplex, const glm::vec3& dir);

    static glm::vec3 doSimplex1(std::vector<glm::vec3>& simplex, const glm::vec3& origin, const glm::vec3& a);

    static glm::vec3 doSimplex2(std::vector<glm::vec3>& simplex, const glm::vec3& origin, const glm::vec3& a, const glm::vec3& b);

    static glm::vec3 doSimplex3(std::vector<glm::vec3>& simplex, const glm::vec3& origin, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);

    static glm::vec3 doSimplex4(std::vector<glm::vec3>& simplex, const glm::vec3& origin, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d);
};

class GJKVertex: public GJKConvexShape {
    Vertex vertex;
public:
    explicit GJKVertex(const Vertex& vertex);
    [[nodiscard]] glm::vec3 computeSupport(const glm::vec3 &direction) const override;
};

class GJKAABB: public GJKConvexShape{
    Vertex center;
    glm::vec3 half;
public:
    explicit GJKAABB(const AABB& aabb);
    [[nodiscard]] glm::vec3 computeSupport(const glm::vec3 &direction) const override;
};

class GJKOBB: public GJKConvexShape {
    Quaternion rotation;
    GJKAABB gjkaabb;

public:
    explicit GJKOBB(const OBB& obb);
    [[nodiscard]] glm::vec3 computeSupport(const glm::vec3 &direction) const override;
};

class GJKSphere: public GJKConvexShape {
    Vertex center;
    float radius;
public:
    explicit GJKSphere(const Sphere& sphere);
    [[nodiscard]] glm::vec3 computeSupport(const glm::vec3 &direction) const override;
};

class GJKMesh: public GJKConvexShape {
    std::shared_ptr<WorldSpaceMesh> worldSpaceMesh;
    std::vector<std::set<unsigned int>> connectedVertexIndices;

    mutable unsigned int startVertexIndex; // Subsequent calls to compute the support will most likely profit from locality
public:
    explicit GJKMesh(const std::shared_ptr<WorldSpaceMesh>& worldSpaceMesh);
    [[nodiscard]] glm::vec3 computeSupport(const glm::vec3 &direction) const override;
};

// TODO GJK for VertexTriangle

#endif //OPTIXMESHCORE_GJK_H
