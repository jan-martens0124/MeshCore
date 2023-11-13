//
// Created by Jonas on 15/06/2022. Core algorithm mostly copied from an implementation by Wim Vancroonenburg
//

#include "GJK.h"
#include <array>
#include <unordered_set>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

std::optional<float>
GJK::computeDistanceSqr(const GJKConvexShape &convex_shape_A, const GJKConvexShape &convex_shape_B,
                        const glm::vec3 &initialDir) {

    glm::vec3 origin = { 0.,0.,0.};

    std::vector<SupportPoint> simplex;
    simplex.reserve(4);

    glm::vec3 vk = support(convex_shape_A, convex_shape_B, initialDir).point;
    long iteration = 0;
    do
    {
        glm::vec3 D = -vk;
        SupportPoint wk = support(convex_shape_A, convex_shape_B, D);

        if(glm::dot((wk.point-vk),D)/glm::dot(D,D) < GJK_EPSILON)
        {
            return glm::dot(D,D);
        }

        simplex.push_back(wk);
        std::optional<glm::vec3> updatedD = doSimplex(simplex, D);

        if(!updatedD)
        {
            return std::nullopt;
        }
        D = updatedD.value();
        vk = (-D);

        iteration++;
    } while (iteration < 1000);

    glm::vec3 diff = vk-origin;
    return glm::dot(diff, diff);
}

bool GJK::hasSeparation(const GJKConvexShape &convex_shape_A, const GJKConvexShape &convex_shape_B, const glm::vec3 &initialDir, float minimumSeparationDistanceSqr) {

    assert(minimumSeparationDistanceSqr >= 0.0);

    std::vector<SupportPoint> simplex;
    simplex.reserve(4);
    auto vk = support(convex_shape_A, convex_shape_B, -initialDir).point;

    long iteration = 0;
    do {
        // Compute the support for this direction
        glm::vec3 D = -vk;
        SupportPoint wk = support(convex_shape_A, convex_shape_B, D);

        // Test if the separation along this distance satisfies the minimum distance already (Optional, but should improve performance)
        auto DD = glm::dot(D, D);
        auto separationAlongD = -glm::dot(wk.point,D);
        auto separationAlongDSqr = separationAlongD * separationAlongD;
        if(separationAlongDSqr>=minimumSeparationDistanceSqr*DD && separationAlongD >= 0.0){ // Correct for D not being normalized by multiplying with DD and avoiding division this way
            return true;
        }

        // Stop if the difference between the values along the direction is too small
        if(glm::dot((wk.point-vk),D) < GJK_EPSILON*DD) {
            return DD >= minimumSeparationDistanceSqr;
        }

        // Add the new point to the simplex and compute the next direction based on this
        simplex.push_back(wk);
        std::optional<glm::dvec3> updatedD = doSimplex(simplex, D); // Find the closest point

        // Test if separation along the new direction is too small
        if(!updatedD.has_value() || glm::dot(updatedD.value(),updatedD.value()) < minimumSeparationDistanceSqr){
            return false;
        }

        vk = (-updatedD.value());

        iteration++;
    } while (iteration < 1000);

    return glm::dot(vk, vk) >= minimumSeparationDistanceSqr;
}

std::optional<std::pair<Vertex, Vertex>> GJK::computeClosestPoints(const GJKConvexShape &convex_shape_A, const GJKConvexShape &convex_shape_B, const glm::vec3 &initialDir) {

    std::vector<SupportPoint> simplex;
    simplex.reserve(4);
    auto vk = support(convex_shape_A, convex_shape_B, -initialDir).point;
    long iteration = 0;
    do {
        // Compute the support for this direction
        glm::vec3 D = -vk;
        auto wk = support(convex_shape_A, convex_shape_B, D);

        if(glm::dot((wk.point-vk),D)/glm::dot(D,D) < GJK_EPSILON){
            break;
        }

        // Add the new point to the simplex and compute the next direction based on this
        simplex.push_back(wk);
        std::optional<glm::vec3> updatedD = doSimplex(simplex, D); // Find the closest point

        if(!updatedD.has_value()){
            return std::nullopt;
        }

        vk = (-updatedD.value());

        iteration++;
    } while(iteration < 1000);

    return computeClosestPoints(simplex);
}

std::pair<glm::dvec3, glm::dvec3> GJK::computeClosestPoints(const std::vector<GJK::SupportPoint>& simplex){
    glm::dvec3 closestPointA;
    glm::dvec3 closestPointB;

    switch (simplex.size()) {
        case 1:
            // Trivial case
            closestPointA = simplex[0].supportPointA;
            closestPointB = simplex[0].supportPointB;
            break;
        case 2: {

            // Find the point on the line segment simplex that is closest to the origin
            auto ab = simplex[1].point - simplex[0].point;

            // Convex composition: P(t) = a + t*(b-a)
//            float t = glm::dot(-ab, simplex[0].point)/glm::dot(ab, ab);
//            t = glm::clamp(t, 0.0, 1.0); // Catch degenerate cases but shouldn't happen
//            closestPointA.scaleAdd(1-t, simplex.get(0).supportPointA, closestPointA);
//            closestPointA.scaleAdd(t, simplex.get(1).supportPointA, closestPointA);
//
//            closestPointB.scaleAdd(1-t, simplex.get(0).supportPointB, closestPointB);
//            closestPointB.scaleAdd(t, simplex.get(1).supportPointB, closestPointB);

            auto l = simplex[1].point - simplex[0].point;

            float ldotl = glm::dot(l,l);
            float ldota = glm::dot(l, simplex[0].point);

            float lambda2 = -ldota/ldotl;
            float lambda1 = (ldotl+ldota)/ldotl;

            closestPointA = lambda1*simplex[0].supportPointA + lambda2*simplex[1].supportPointA;
            closestPointB = lambda1*simplex[0].supportPointB + lambda2*simplex[1].supportPointB;

            if (ldotl == 0) {
                closestPointA = simplex[0].supportPointA;
                closestPointB = simplex[0].supportPointB;
            }
            else if (lambda1 < 0) {
                closestPointA = simplex[1].supportPointA;
                closestPointB = simplex[1].supportPointB;
            } else if (lambda2 < 0) {
                closestPointA = simplex[0].supportPointA;
                closestPointB = simplex[0].supportPointB;
            }
        }
            break;
        case 3: {
            // We look for the barycentric coordinates of the closest point in the simplex to the origin
            // This is a convex combination of simplex points
            // Derived from ClosestPtPointTriangle as described in "Real-Time Collision Detection" by Christer Ericson
            auto ab = simplex[1].point - simplex[0].point;
            auto ac = simplex[2].point - simplex[0].point;

            // Check if origin lies in the vertex region outside A
            float d1 = -glm::dot(ab,simplex[0].point);
            float d2 = -glm::dot(ac,simplex[0].point);

            // Check if point lies in the vertex region outside B
            float d3 = -glm::dot(ab,simplex[1].point);
            float d4 = -glm::dot(ac,simplex[1].point);

            // Check if point lies in the vertex region outside C
            float d5 = -glm::dot(ab,simplex[2].point);
            float d6 = -glm::dot(ac,simplex[2].point);

            // The point lies in the face region. Compute the closest point through barycentric coordinates
            float va = d3*d6 - d5*d4;
            float vb = d5*d2 - d1*d6;
            float vc = d1*d4 - d3*d2;
            float denominator = 1.0f / (va + vb + vc);
            float u = va * denominator;
            float v = vb * denominator;
            float w = vc * denominator;

            assert(u >= 0.0 && u <= 1.0);
            assert(v >= 0.0 && v <= 1.0);
            assert(w >= 0.0 && w <= 1.0);

            closestPointA = u*simplex[0].supportPointA + v*simplex[1].supportPointA + w*simplex[2].supportPointA;
            closestPointB = u*simplex[0].supportPointB + v*simplex[1].supportPointB + w*simplex[2].supportPointB;
            break;
        }
        default:
            throw "Simplex size not supported";
    }
    return {closestPointA, closestPointB};
}

std::optional<glm::vec3> GJK::doSimplex(std::vector<SupportPoint> &simplex, const glm::vec3 &dir) {

    glm::vec3 origin(0);
    glm::vec3 g;
    switch(simplex.size()){
        case 1:
            g = doSimplex1(simplex, origin, simplex[0]);
            break;
        case 2:
            g = doSimplex2(simplex, origin, simplex[0], simplex[1]);
            break;
        case 3:
            g = doSimplex3(simplex, origin, simplex[0], simplex[1], simplex[2]);
            break;
        case 4:
            g = doSimplex4(simplex, origin, simplex[0], simplex[1], simplex[2], simplex[3]);
            break;
        default:
            throw std::runtime_error("Should not occur!");
    }

    if(glm::distance(g,origin) < GJK_EPSILON)
    {
        return std::nullopt;
    }
    else
    {
        return -g;
    }

}

glm::vec3 GJK::doSimplex1(std::vector<SupportPoint> &simplex, const glm::vec3 &origin, const SupportPoint &a) {
    return a.point;
}

glm::vec3 GJK::doSimplex2(std::vector<SupportPoint> &simplex, const glm::vec3 &origin, const SupportPoint &a, const SupportPoint &b) {

    glm::vec3 ab = b.point - a.point;
    glm::vec3 ao = origin - a.point;

    glm::vec3 q;
    float t = glm::dot(ao, ab);

    if(t <= 0.0)
    {
        q = a.point;
        simplex.erase(simplex.begin()+1);
    } else
    {
        float denom = glm::dot(ab, ab);
        if( t >= denom)
        {
            q = b.point;
            simplex.erase(simplex.begin());
        } else
        {
            t = t / denom;
            q = t * ab + a.point;
        }
    }

    return q;
}

float scalarProd(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
    return glm::dot(a, glm::cross(b, c));
}

glm::vec3 GJK::doSimplex3(std::vector<SupportPoint> &simplex, const glm::vec3 &o, const SupportPoint &a, const SupportPoint &b, const SupportPoint &c) {

    glm::vec3 ac, ca, ab, ba, bc, cb, ao, bo, co;
    ac = c.point-a.point;
    ca = -ac;
    ab = b.point-a.point;
    ba = -ab;
    bc = c.point - b.point;
    cb = -bc;
    ao = o - a.point;
    bo = o - b.point;
    co = o - c.point;

    float snom = glm::dot(ao, ab), sdenom = glm::dot(bo, ba);
    float tnom = glm::dot(ao, ac), tdenom = glm::dot(co, ca);

    glm::vec3 q;
    if(snom <= 0.0 && tnom <= 0.0)
    {
        q = a.point;
        simplex.erase(simplex.begin()+2);
        simplex.erase(simplex.begin()+1);

        return q;
    }

    float unom = glm::dot(bo,bc), udenom = glm::dot(co,cb);

    if (sdenom <= 0.0 && unom <= 0.0) {
        q = b.point;
        simplex.erase(simplex.begin()+2);
        simplex.erase(simplex.begin());

        return q;

    }
    if (tdenom <= 0.0 && udenom <= 0.0) {
        q = c.point;
        simplex.erase(simplex.begin()+1);
        simplex.erase(simplex.begin());

        return q;
    }

    glm::vec3 n = glm::cross(ab, ac);
    float vc = scalarProd(n, -ao, -bo);

    if (vc <= 0.0 && snom >= 0.0 && sdenom >= 0.0) {
        simplex.erase(simplex.begin()+2);
        q = (ab * snom / (snom + sdenom)) + a.point;

        return q;

    }

    float va = scalarProd(n, -bo, -co);
    if (va <= 0.0 && unom >= 0.0 && udenom >= 0.0) {
        simplex.erase(simplex.begin());
        q = (bc*unom / (unom + udenom)) + b.point;

        return q;

    }

    float vb = scalarProd(n, -co, -ao);
    if (vb <= 0.0 && tnom >= 0.0 && tdenom >= 0.0) {
        simplex.erase(simplex.begin()+1);
        q = (ac*tnom / (tnom + tdenom)) + a.point;

        return q;

    }

    float u = va / (va + vb + vc);
    float v = vb / (va + vb + vc);
    float w = 1 - u - v;

    q = u*a.point + v*b.point + w*c.point;

    return q;
}

bool pointOutsideOfPlane(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d) {
    glm::vec3 ap = p - a;
    glm::vec3 ad = d - a;
    glm::vec3 ac = c - a;
    glm::vec3 ab = b - a;

    glm::vec3 abac = glm::cross(ab, ac);

    float signp = glm::dot(ap, abac);
    float signd = glm::dot(ad, abac);

    return signp * signd < GJK_EPSILON;
}

glm::vec3 GJK::doSimplex4(std::vector<SupportPoint> &simplex, const glm::vec3 &origin, const SupportPoint &a, const SupportPoint &b, const SupportPoint &c, const SupportPoint &d) {
    glm::vec3 closestP = origin;
    float distanceSqr = std::numeric_limits<float>::infinity();
    int closestTriangle = -1;

    std::array<std::vector<SupportPoint>, 4> simplexCopies = { simplex, simplex, simplex, simplex };

    if (pointOutsideOfPlane(origin, a.point, b.point, c.point, d.point)) {
        simplexCopies[0].erase(simplexCopies[0].begin()+3);
        glm::vec3 q = doSimplex3(simplexCopies[0], origin, a, b, c);

        float distSqr = glm::distance2(origin, q);
        if (distSqr < distanceSqr) {
            closestP = q;
            distanceSqr = distSqr;
            closestTriangle = 0;
        }
    }

    if (pointOutsideOfPlane(origin, a.point, b.point, d.point, c.point)) {
        simplexCopies[1].erase(simplexCopies[1].begin()+2);
        glm::vec3 q = doSimplex3(simplexCopies[1], origin, a, b, d);

        float distSqr = glm::distance2(origin,q);
        if (distSqr < distanceSqr) {
            closestP = q;
            distanceSqr = distSqr;
            closestTriangle = 1;
        }
    }

    if (pointOutsideOfPlane(origin, a.point, c.point, d.point, b.point)) {
        simplexCopies[2].erase(simplexCopies[2].begin()+1);
        glm::vec3 q = doSimplex3(simplexCopies[2], origin, a, c, d);

        float distSqr = glm::distance2(origin,q);
        if (distSqr < distanceSqr) {
            closestP = q;
            distanceSqr = distSqr;
            closestTriangle = 2;
        }
    }

    if (pointOutsideOfPlane(origin, b.point, c.point, d.point, a.point)) {
        simplexCopies[3].erase(simplexCopies[3].begin());
        glm::vec3 q = doSimplex3(simplexCopies[3], origin, b, c, d);

        float distSqr = glm::distance2(origin,q);
        if (distSqr < distanceSqr) {
            closestP = q;
            distanceSqr = distSqr;
            closestTriangle = 3;
        }
    }

    if (closestTriangle != -1) {
        simplex.clear();
        simplex = simplexCopies[closestTriangle];
    }

    return closestP;
}

GJK::SupportPoint GJK::support(const GJKConvexShape& convex_shape_A, const GJKConvexShape& convex_shape_B, const glm::vec3& D) {
    auto pa = convex_shape_A.computeSupport(D);
    auto pb = convex_shape_B.computeSupport(-D);
    return {pa, pb};
}

glm::vec3 GJKAABB::computeSupport(const glm::vec3 &direction) const {
    return this->center + this->half * glm::sign(direction);
}

GJKAABB::GJKAABB(const AABB &aabb): center(aabb.getCenter()), half(aabb.getHalf()) {}

glm::vec3 GJKVertex::computeSupport(const glm::vec3 &direction) const {
    return this->vertex;
}

GJKVertex::GJKVertex(const Vertex &vertex): vertex(vertex) {}

GJKOBB::GJKOBB(const OBB &obb): rotation(obb.getRotation()), gjkaabb(obb.getAabb()) {}

glm::vec3 GJKOBB::computeSupport(const glm::vec3 &direction) const {
    auto aabbSpaceDirection = rotation.inverseRotateVertex(direction);
    auto aabbSpaceSupport = gjkaabb.computeSupport(aabbSpaceDirection);
    return rotation.rotateVertex(aabbSpaceSupport);
}

GJKMesh::GJKMesh(const WorldSpaceMesh* worldSpaceMesh): worldSpaceMesh(worldSpaceMesh), startVertexIndex(0) {
    assert(worldSpaceMesh->getModelSpaceMesh()->isConvex() && "Should only construct a GJKMesh if we know this mesh is actually convex!");
}

glm::vec3 GJKMesh::computeSupport(const glm::vec3 &direction) const {

    auto& transformation = worldSpaceMesh->getModelTransformation();
    auto& connectedVertexIndices = worldSpaceMesh->getModelSpaceMesh()->getConnectedVertexIndices();
    auto& vertices = worldSpaceMesh->getModelSpaceMesh()->getVertices();

    auto modelSpaceDirection = transformation.inverseTransformVector(direction);

    auto currentVertexIndex = startVertexIndex;
    auto currentSupport = glm::dot(vertices.at(startVertexIndex), modelSpaceDirection);

    std::set<unsigned int> visitedVertexIndices;
    visitedVertexIndices.insert(startVertexIndex);

    for (int i = 0; i < vertices.size(); ++i){ // We will visit at most all points, otherwise something is wrong

        unsigned int nextVertexIndex = -1u;
        float nextSupport = currentSupport;

        for(const auto& connectedVertexIndex: connectedVertexIndices.at(currentVertexIndex)){

            // Skip if we already visited this vertex
            if(visitedVertexIndices.find(connectedVertexIndex) != visitedVertexIndices.end()){
                continue;
            }

            auto connectedVertex = vertices.at(connectedVertexIndex);
            auto connectedSupport = glm::dot(connectedVertex, modelSpaceDirection);


#if !GJK_STEEPEST_DESCENT
            // Hill Climbing
            // Continue with the first connected vertex that has better support
            if(connectedSupport >= currentSupport){
                nextVertexIndex = connectedVertexIndex;
                nextSupport = connectedSupport;
                break;
            }
# else
            // Steepest descent
            // Continue with the connected vertex that has the best support
            if(connectedSupport >= nextSupport){
                nextVertexIndex = connectedVertexIndex;
                nextSupport = connectedSupport;
            }
#endif
        }
        if(nextVertexIndex == -1u){
            // No better support found, return current vertex
            break;
        }

        currentVertexIndex = nextVertexIndex;
        currentSupport = nextSupport;
        visitedVertexIndices.insert(currentVertexIndex);
    }

#ifndef NDEBUG
    // Assert there is no vertex that has better support (However, this might happen for "broken" meshes, computing the convex hull can be a remedy here)
    for (const auto &vertex: vertices){
        assert(glm::dot(vertex, modelSpaceDirection) <= currentSupport);
    }
#endif

    this->startVertexIndex = currentVertexIndex;
    return worldSpaceMesh->getModelTransformation().transformVertex(vertices.at(currentVertexIndex));
}

glm::vec3 GJKSphere::computeSupport(const glm::vec3 &direction) const {
    return center + glm::normalize(direction) * radius;
}

GJKSphere::GJKSphere(const Sphere &sphere): radius(sphere.getRadius()), center(sphere.getCenter()) {}