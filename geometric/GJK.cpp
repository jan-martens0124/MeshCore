//
// Created by Jonas on 15/06/2022. Core algorithm mostly copied from an implementation by Wim Vancroonenburg
//

#include "GJK.h"
#include <array>
#include <unordered_set>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

std::optional<double>
GJK::computeDistanceSqr(const GJKConvexShape &convex_shape_A, const GJKConvexShape &convex_shape_B,
                        const glm::vec3 &initialDir) {

    glm::vec3 origin = { 0.,0.,0.};

    std::vector<glm::vec3> simplex;
    simplex.reserve(4);

    glm::vec3 vk = support(convex_shape_A, convex_shape_B, initialDir);
    long iteration = 0;
    do
    {
        glm::vec3 D = -vk;
        glm::vec3 wk = support(convex_shape_A, convex_shape_B, D);

        if(glm::dot((wk-vk),D)/glm::dot(D,D) < GJK_EPSILON)
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


std::optional<glm::vec3> GJK::doSimplex(std::vector<glm::vec3> &simplex, const glm::vec3 &dir) {

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

glm::vec3 GJK::doSimplex1(std::vector<glm::vec3> &simplex, const glm::vec3 &origin, const glm::vec3 &a) {
    return a;
}

glm::vec3 GJK::doSimplex2(std::vector<glm::vec3> &simplex, const glm::vec3 &origin, const glm::vec3 &a,
                               const glm::vec3 &b) {

    glm::vec3 ab = b - a;
    glm::vec3 ao = origin - a;

    glm::vec3 q;
    float t = glm::dot(ao, ab);

    if(t <= 0.0)
    {
        q = a;
        simplex.erase(simplex.begin()+1);
    } else
    {
        float denom = glm::dot(ab, ab);
        if( t >= denom)
        {
            q = b;
            simplex.erase(simplex.begin());
        } else
        {
            t = t / denom;
            q = t * ab + a;
        }
    }

    return q;
}

float scalarProd(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
{
    return glm::dot(a, glm::cross(b, c));
}

glm::vec3 GJK::doSimplex3(std::vector<glm::vec3> &simplex, const glm::vec3 &o, const glm::vec3 &a,
                               const glm::vec3 &b, const glm::vec3 &c) {

    glm::vec3 ac, ca, ab, ba, bc, cb, ao, bo, co;
    ac = c-a;
    ca = -ac;
    ab = b-a;
    ba = -ab;
    bc = c - b;
    cb = -bc;
    ao = o - a;
    bo = o - b;
    co = o - c;

    float snom = glm::dot(ao, ab), sdenom = glm::dot(bo, ba);
    float tnom = glm::dot(ao, ac), tdenom = glm::dot(co, ca);

    glm::vec3 q;
    if(snom <= 0.0 && tnom <= 0.0)
    {
        q = a;
        simplex.erase(simplex.begin()+2);
        simplex.erase(simplex.begin()+1);

        return q;
    }

    float unom = glm::dot(bo,bc), udenom = glm::dot(co,cb);

    if (sdenom <= 0.0 && unom <= 0.0) {
        q = b;
        simplex.erase(simplex.begin()+2);
        simplex.erase(simplex.begin());

        return q;

    }
    if (tdenom <= 0.0 && udenom <= 0.0) {
        q = c;
        simplex.erase(simplex.begin()+1);
        simplex.erase(simplex.begin());

        return q;
    }

    glm::vec3 n = glm::cross(ab, ac);
    float vc = scalarProd(n, -ao, -bo);

    if (vc <= 0.0 && snom >= 0.0 && sdenom >= 0.0) {
        simplex.erase(simplex.begin()+2);
        q = (ab * snom / (snom + sdenom)) + a;

        return q;

    }

    float va = scalarProd(n, -bo, -co);
    if (va <= 0.0 && unom >= 0.0 && udenom >= 0.0) {
        simplex.erase(simplex.begin());
        q = (bc*unom / (unom + udenom)) + b;

        return q;

    }

    float vb = scalarProd(n, -co, -ao);
    if (vb <= 0.0 && tnom >= 0.0 && tdenom >= 0.0) {
        simplex.erase(simplex.begin()+1);
        q = (ac*tnom / (tnom + tdenom))+ a;

        return q;

    }


    float u = va / (va + vb + vc);
    float v = vb / (va + vb + vc);
    float w = 1 - u - v;

    q = u*a + v*b + w*c;

    return q;
}

bool pointOutsideOfPlane(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d)
{
    glm::vec3 ap = p - a;
    glm::vec3 ad = d - a;
    glm::vec3 ac = c - a;
    glm::vec3 ab = b - a;

    glm::vec3 abac = glm::cross(ab, ac);

    float signp = glm::dot(ap, abac);
    float signd = glm::dot(ad, abac);

    return signp * signd < GJK_EPSILON; // TODO this is changed to GJK_EPSILON instead of 0.0, but seems to solve differences between rotating cubes as mesh vs OBB
}

glm::vec3 GJK::doSimplex4(std::vector<glm::vec3> &simplex, const glm::vec3 &origin, const glm::vec3 &a,
                               const glm::vec3 &b, const glm::vec3 &c, const glm::vec3 &d) {
    glm::vec3 closestP = origin;
    float distanceSqr = std::numeric_limits<float>::infinity();
    int closestTriangle = -1;

    std::array<std::vector<glm::vec3>, 4> simplexCopies = { simplex, simplex, simplex, simplex };

    if (pointOutsideOfPlane(origin, a, b, c, d)) {
        simplexCopies[0].erase(simplexCopies[0].begin()+3);
        glm::vec3 q = doSimplex3(simplexCopies[0], origin, a, b, c);

        float distSqr = glm::distance2(origin, q);
        if (distSqr < distanceSqr) {
            closestP = q;
            distanceSqr = distSqr;
            closestTriangle = 0;
        }
    }

    if (pointOutsideOfPlane(origin, a, b, d, c)) {
        simplexCopies[1].erase(simplexCopies[1].begin()+2);
        glm::vec3 q = doSimplex3(simplexCopies[1], origin, a, b, d);

        float distSqr = glm::distance2(origin,q);
        if (distSqr < distanceSqr) {
            closestP = q;
            distanceSqr = distSqr;
            closestTriangle = 1;
        }
    }

    if (pointOutsideOfPlane(origin, a, c, d, b)) {
        simplexCopies[2].erase(simplexCopies[2].begin()+1);
        glm::vec3 q = doSimplex3(simplexCopies[2], origin, a, c, d);

        float distSqr = glm::distance2(origin,q);
        if (distSqr < distanceSqr) {
            closestP = q;
            distanceSqr = distSqr;
            closestTriangle = 2;
        }
    }

    if (pointOutsideOfPlane(origin, b, c, d, a)) {
        simplexCopies[3].erase(simplexCopies[3].begin());
        glm::vec3 q = doSimplex3(simplexCopies[3], origin, b, c, d);

        float distSqr = glm::distance2(origin,q);
        if (distSqr < distanceSqr) {
            closestP = q;
            distanceSqr = distSqr;
            closestTriangle = 3;
        }
    }

    if (closestTriangle != -1)
    {
        simplex.clear();
        simplex = simplexCopies[closestTriangle];
    }

    return closestP;
}

glm::vec3 GJK::support(const GJKConvexShape& convex_shape_A, const GJKConvexShape& convex_shape_B, const glm::vec3& D) {
    glm::vec3 pa = convex_shape_A.computeSupport(D);
    glm::vec3 pb = convex_shape_B.computeSupport(-D);
    return pa-pb;
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

GJKMesh::GJKMesh(const std::shared_ptr<WorldSpaceMesh> &worldSpaceMesh): worldSpaceMesh(worldSpaceMesh), startVertexIndex(0) {

    assert(worldSpaceMesh->getModelSpaceMesh()->isConvex());

    connectedVertexIndices.resize(worldSpaceMesh->getModelSpaceMesh()->getVertices().size());
    for (const auto &indexTriangle: worldSpaceMesh->getModelSpaceMesh()->getTriangles()){
        connectedVertexIndices.at(indexTriangle.vertexIndex0).insert(indexTriangle.vertexIndex1);
        connectedVertexIndices.at(indexTriangle.vertexIndex0).insert(indexTriangle.vertexIndex2);
        connectedVertexIndices.at(indexTriangle.vertexIndex1).insert(indexTriangle.vertexIndex2);
        connectedVertexIndices.at(indexTriangle.vertexIndex1).insert(indexTriangle.vertexIndex0);
        connectedVertexIndices.at(indexTriangle.vertexIndex2).insert(indexTriangle.vertexIndex0);
        connectedVertexIndices.at(indexTriangle.vertexIndex2).insert(indexTriangle.vertexIndex1);
    }
}

glm::vec3 GJKMesh::computeSupport(const glm::vec3 &direction) const {

    auto& transformation = worldSpaceMesh->getModelTransformation();
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
    this->startVertexIndex = currentVertexIndex;
    return worldSpaceMesh->getModelTransformation().transformVertex(vertices.at(currentVertexIndex));
}

glm::vec3 GJKSphere::computeSupport(const glm::vec3 &direction) const {
    return center + glm::normalize(direction) * radius;
}

GJKSphere::GJKSphere(const Sphere &sphere): radius(sphere.getRadius()), center(sphere.getCenter()) {}