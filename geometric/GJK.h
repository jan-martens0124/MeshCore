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
    static bool hasSeparation(const GJKConvexShape &convex_shape_A, const GJKConvexShape &convex_shape_B, const glm::vec3& initialDir, float minimumSeparationDistanceSqr=0.0f);
    static std::optional<std::pair<Vertex,Vertex>> computeClosestPoints(const GJKConvexShape &convex_shape_A, const GJKConvexShape &convex_shape_B, const glm::vec3& initialDir);

private:

    class SupportPoint {
    public:
        SupportPoint(glm::vec3 supportPointA, glm::vec3 supportPointB) : supportPointA(supportPointA), supportPointB(supportPointB), point(supportPointA - supportPointB) {}
        glm::vec3 supportPointA;
        glm::vec3 supportPointB;
        glm::vec3 point;
    };

    static SupportPoint support(const GJKConvexShape& convex_shape_A, const GJKConvexShape& convex_shape_B, const glm::vec3& D);
    static std::optional<glm::vec3> doSimplex(std::vector<SupportPoint>& simplex, const glm::vec3& dir);

    static glm::vec3 doSimplex1(std::vector<SupportPoint>& simplex, const glm::vec3& origin, const SupportPoint& a);
    static glm::vec3 doSimplex2(std::vector<SupportPoint>& simplex, const glm::vec3& origin, const SupportPoint& a, const SupportPoint& b);
    static glm::vec3 doSimplex3(std::vector<SupportPoint>& simplex, const glm::vec3& origin, const SupportPoint& a, const SupportPoint& b, const SupportPoint& c);
    static glm::vec3 doSimplex4(std::vector<SupportPoint>& simplex, const glm::vec3& origin, const SupportPoint& a, const SupportPoint& b, const SupportPoint& c, const SupportPoint& d);

    static std::pair<glm::dvec3, glm::dvec3> computeClosestPoints(const std::vector<GJK::SupportPoint>& simplex);
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
    const WorldSpaceMesh* worldSpaceMesh; // This was originally a shared_ptr, but we want to avoid copying these pointers too much as they are atomic operations
    mutable unsigned int startVertexIndex; // Subsequent calls to compute the support will most likely profit from locality
public:
    explicit GJKMesh(const WorldSpaceMesh* worldSpaceMesh);
    [[nodiscard]] glm::vec3 computeSupport(const glm::vec3 &direction) const override;
};

//class GJKVertexTriangle: public GJKConvexShape {
//    VertexTriangle vertexTriangle;
//public:
//    explicit GJKVertexTriangle(const VertexTriangle& vertexTriangle);
//    [[nodiscard]] glm::vec3 computeSupport(const glm::vec3 &direction) const override;
//};

#endif //OPTIXMESHCORE_GJK_H
