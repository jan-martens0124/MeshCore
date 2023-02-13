//
// Created by Jonas on 25/08/2022.
//

#ifndef OPTIXMESHCORE_ABSTRACTBOUNDSTREE_H
#define OPTIXMESHCORE_ABSTRACTBOUNDSTREE_H

#include "../core/Vertex.h"
#include "../core/Ray.h"
#include "../core/VertexTriangle.h"
#include "../utility/hash.h"
#include "../geometric/Intersection.h"
#include "../geometric/Distance.h"
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <array>
#include <memory>
#include <unordered_set>
#include <map>

/** Abstract template class for regular bounding volume tree hierarchies
 *
 * Bounds can be represented by AABBs, OBBs, Spheres, ... as long as the required Intersection and getDistanceSquaredTo functions are implemented.
 *
 * **/

template <class Bounds, unsigned int Degree, bool UniqueTriangleAssignment=false>
class AbstractBoundsTree {

protected:
    Bounds bounds;
    std::vector<VertexTriangle> triangles;
    std::array<std::shared_ptr<AbstractBoundsTree<Bounds, Degree, UniqueTriangleAssignment>>, Degree> children;
    const unsigned int depth;
    bool split = false;
    bool empty = true;

    AbstractBoundsTree(const Bounds &bounds, unsigned int depth): bounds(bounds), split(false), depth(depth), empty(true) {}

public:
    virtual void splitTopDown(unsigned int maxDepth, unsigned int maxTrianglesPerNode) = 0;

    [[nodiscard]] bool intersectsTriangle(const VertexTriangle& vertexTriangle) const;
    [[nodiscard]] bool intersectsRay(const Ray& ray) const;

    [[nodiscard]] Vertex getClosestPoint(const Vertex &vertex) const;
    [[nodiscard]] const VertexTriangle* getClosestTriangle(const Vertex& vertex) const;
    [[nodiscard]] float getShortestDistanceSquared(const Vertex& vertex) const;
    [[nodiscard]] bool hasMinimumDistance(const Vertex& vertex, float minimumDistanceSquared) const;

    [[nodiscard]] Vertex getClosestPoint(const VertexTriangle &triangle) const;
    [[nodiscard]] const VertexTriangle* getClosestTriangle(const VertexTriangle &triangle) const;
    [[nodiscard]] float getShortestDistanceSquared(const VertexTriangle &triangle) const;
    [[nodiscard]] bool hasMinimumDistance(const VertexTriangle &triangle, float minimumDistanceSquared) const;

    [[nodiscard]] unsigned int getNumberOfRayIntersections(const Ray& ray) const;
    [[nodiscard]] std::vector<VertexTriangle> getIntersectingTriangles(const Ray& ray) const;
    [[nodiscard]] std::vector<VertexTriangle> getIntersectingTriangles(const VertexTriangle& triangle) const;

private:
    // More efficient versions of the function are possible if each triangle is uniquely assigned to a single node
    void getIntersectingTriangles(const Ray& ray, std::vector<VertexTriangle>& result) const;
    void getIntersectingTriangles(const Ray& ray, std::unordered_set<VertexTriangle>& result) const;
    void getIntersectingTriangles(const VertexTriangle& triangle, std::vector<VertexTriangle>& result) const;
    void getIntersectingTriangles(const VertexTriangle& triangle, std::unordered_set<VertexTriangle>& result) const;

public:
    struct ClosestTriangleQueryResult{
        const VertexTriangle* closestTriangle = nullptr;
        Vertex closestVertex{};
        float lowerDistanceBoundSquared = std::numeric_limits<float>::max();
    };
    void queryClosestTriangle(const Vertex& vertex, ClosestTriangleQueryResult* result) const;
    void queryClosestTriangle(const VertexTriangle& triangle, ClosestTriangleQueryResult* result) const;

public:

    [[nodiscard]] const Bounds &getBounds() const;
    [[nodiscard]] bool isSplit() const;
    [[nodiscard]] bool isEmpty() const;
    [[nodiscard]] const std::vector<VertexTriangle> &getTriangles() const;
    [[nodiscard]] const std::array<std::shared_ptr<AbstractBoundsTree<Bounds, Degree, UniqueTriangleAssignment>>, Degree> &getChildren() const;
};

template <class Bounds, unsigned int Degree, bool UniqueTriangleAssignment>
const Bounds &AbstractBoundsTree<Bounds, Degree, UniqueTriangleAssignment>::getBounds() const {
    return bounds;
}

template <class Bounds, unsigned int Degree, bool UniqueTriangleAssignment>
bool AbstractBoundsTree<Bounds, Degree, UniqueTriangleAssignment>::isSplit() const {
    return split;
}

template <class Bounds, unsigned int Degree, bool UniqueTriangleAssignment>
bool AbstractBoundsTree<Bounds, Degree, UniqueTriangleAssignment>::isEmpty() const {
    return empty;
}

template <class Bounds, unsigned int Degree, bool UniqueTriangleAssignment>
const std::vector<VertexTriangle> &AbstractBoundsTree<Bounds, Degree, UniqueTriangleAssignment>::getTriangles() const {
    return triangles;
}

template <class Bounds, unsigned int Degree, bool UniqueTriangleAssignment>
const std::array<std::shared_ptr<AbstractBoundsTree<Bounds, Degree, UniqueTriangleAssignment>>, Degree> &AbstractBoundsTree<Bounds, Degree, UniqueTriangleAssignment>::getChildren() const {
    return children;
}

template <class Bounds, unsigned int Degree, bool UniqueTriangleAssignment>
bool AbstractBoundsTree<Bounds, Degree, UniqueTriangleAssignment>::intersectsTriangle(const VertexTriangle &vertexTriangle) const{
    if(Intersection::intersect(this->bounds, vertexTriangle)){
        for (const auto &child: children){
            if(child && child->intersectsTriangle(vertexTriangle)){
                return true;
            }
        }
        for(const auto& triangle: triangles){
            if(Intersection::intersect(triangle, vertexTriangle)){
                return true;
            }
        }
    }
    return false;
}

template <class Bounds, unsigned int Degree, bool UniqueTriangleAssignment>
bool AbstractBoundsTree<Bounds, Degree, UniqueTriangleAssignment>::intersectsRay(const Ray &ray) const {
    if(Intersection::intersect(this->bounds, ray)){
        for (const auto &child: children){
            if(child && child->intersectsTriangle(ray)){
                return true;
            }
        }
        for(const auto& triangle: triangles){
            if(Intersection::intersect(ray, triangle)){
                return true;
            }
        }
    }
    return false;
}

template <class Bounds, unsigned int Degree, bool UniqueTriangleAssignment>
unsigned int AbstractBoundsTree<Bounds, Degree, UniqueTriangleAssignment>::getNumberOfRayIntersections(const Ray &ray) const {

    if(!UniqueTriangleAssignment){
        std::unordered_set<VertexTriangle> result;
        this->getIntersectingTriangles(ray, result);
        return result.size();
    }

    // Code below is more efficient but this is only correct if each triangle is uniquely assigned to a single node on each level
    if(Intersection::intersect(this->bounds, ray)){
        unsigned int intersectingTriangles = 0u;
        for(const auto& child: children){
            if(child){
                auto childIntersectingTriangles = child->getNumberOfRayIntersections(ray);
                intersectingTriangles += childIntersectingTriangles;
            }
        }
        for(const auto& triangle: triangles){
            if(Intersection::intersect(ray, triangle)){
                intersectingTriangles++;
            }
        }
        return intersectingTriangles;
    }
    else{
        return 0u;
    }
}

template <class Bounds, unsigned int Degree, bool UniqueTriangleAssignment>
std::vector<VertexTriangle> AbstractBoundsTree<Bounds, Degree, UniqueTriangleAssignment>::getIntersectingTriangles(const Ray &ray) const {
    if(UniqueTriangleAssignment){
        std::vector<VertexTriangle> result;
        this->getIntersectingTriangles(ray, result);
        return result;
    }
    else{
        std::unordered_set<VertexTriangle> result;
        this->getIntersectingTriangles(ray, result);
        return {result.begin(), result.end()};
    }
}

template<class Bounds, unsigned int Degree, bool UniqueTriangleAssignment>
void AbstractBoundsTree<Bounds, Degree, UniqueTriangleAssignment>::getIntersectingTriangles(const Ray &ray, std::vector<VertexTriangle> &result) const {
    if(Intersection::intersect(this->bounds, ray)){
        if(split){
            assert(triangles.empty()); // A split node shouldn't contain any triangles
            for(const auto& child: children){
                child->getIntersectingTriangles(ray, result);
            }
        }
        else{
            for(const auto& triangle: triangles){
                if(Intersection::intersect(ray, triangle)){
                    result.emplace_back(triangle);
                }
            }
        }
    }
}


template<class Bounds, unsigned int Degree, bool UniqueTriangleAssignment>
void AbstractBoundsTree<Bounds, Degree, UniqueTriangleAssignment>::getIntersectingTriangles(const Ray &ray, std::unordered_set<VertexTriangle> &result) const {
    if(Intersection::intersect(this->bounds, ray)){
        if(split){
            assert(triangles.empty()); // A split node shouldn't contain any triangles
            for(const auto& child: children){
                child->getIntersectingTriangles(ray, result);
            }
        }
        else{
            for(const auto& triangle: triangles){
                if(Intersection::intersect(ray, triangle)){
                    result.insert(triangle);
                }
            }
        }
    }
}

template<class Bounds, unsigned int Degree, bool UniqueTriangleAssignment>
void AbstractBoundsTree<Bounds, Degree, UniqueTriangleAssignment>::getIntersectingTriangles(const VertexTriangle &triangle, std::vector<VertexTriangle> &result) const {
    if(Intersection::intersect(this->bounds, triangle)){
        if(split){
            assert(triangles.empty()); // A split node shouldn't contain any triangles
            for(const auto& child: children){
                child->getIntersectingTriangles(triangle, result);
            }
        }
        else{
            for(const auto& nodeTriangle: triangles){
                if(Intersection::intersect(triangle, nodeTriangle)){
                    result.emplace_back(nodeTriangle);
                }
            }
        }
    }
}

template<class Bounds, unsigned int Degree, bool UniqueTriangleAssignment>
void AbstractBoundsTree<Bounds, Degree, UniqueTriangleAssignment>::getIntersectingTriangles(const VertexTriangle &triangle, std::unordered_set<VertexTriangle> &result) const {
    if(Intersection::intersect(this->bounds, triangle)){
        if(split){
            assert(triangles.empty()); // A split node shouldn't contain any triangles
            for(const auto& child: children){
                child->getIntersectingTriangles(triangle, result);
            }
        }
        else{
            for(const auto& nodeTriangle: triangles){
                if(Intersection::intersect(triangle, nodeTriangle)){
                    result.insert(nodeTriangle);
                }
            }
        }
    }
}

template <class Bounds, unsigned int Degree, bool UniqueTriangleAssignment>
std::vector<VertexTriangle> AbstractBoundsTree<Bounds, Degree, UniqueTriangleAssignment>::getIntersectingTriangles(const VertexTriangle &triangle) const {
    if(UniqueTriangleAssignment){
        std::vector<VertexTriangle> result;
        this->getIntersectingTriangles(triangle, result);
        return result;
    }
    else{
        std::unordered_set<VertexTriangle> result;
        this->getIntersectingTriangles(triangle, result);
        return {result.begin(), result.end()};
    }
}

template<class Bounds, unsigned int Degree, bool UniqueTriangleAssignment>
void AbstractBoundsTree<Bounds, Degree, UniqueTriangleAssignment>::queryClosestTriangle(const VertexTriangle &triangle, ClosestTriangleQueryResult *result) const {
    if(split){
        // We calculate the closest distance for each child first, as we will calculate them all anyway
        std::array<float, Degree> squaredDistances;
        std::array<unsigned int, Degree> indices;
        for(unsigned int childIndex = 0; childIndex < Degree; childIndex++){

            // Calculate minimal distance to the bounding box
            auto child = children[childIndex];
            squaredDistances[childIndex] = child ? Distance::distanceSquared(child->bounds, triangle.bounds) : std::numeric_limits<float>::max(); // TODO change to bounds.getDistanceSquaredTo(triangle) in the future
            indices[childIndex] = childIndex; // Sorted later
        }

        // We can then sort to visit closer distances first and pruning more work
        // This is much faster than not storing the distances and then sorting them
        std::sort(indices.begin(), indices.end(), [squaredDistances](const auto& indexA, const auto& indexB){
            return squaredDistances[indexA] < squaredDistances[indexB];
        });

        // Visit only children that can lead to improvement
        for (const auto &childIndex : indices){
            if(squaredDistances[childIndex] < result->lowerDistanceBoundSquared){
                children[childIndex]->queryClosestTriangle(triangle, result);
            }
            else{
                break; // Because of the sorting, the next children shouldn't be visited either
            }
        }
    }
    else{
        for (auto &otherTriangle : triangles){
            Vertex closestPointTriangle, closestPointOtherTriangle;
            auto distanceSquared = Distance::distanceSquared(triangle, otherTriangle, &closestPointTriangle, &closestPointOtherTriangle);
            if(distanceSquared < result->lowerDistanceBoundSquared){ // If multiple triangles are equally close, the first one will be returned
                result->closestTriangle = &otherTriangle;
                result->lowerDistanceBoundSquared = distanceSquared;
                result->closestVertex = closestPointOtherTriangle;
            }
        }
    }
}

template <class Bounds, unsigned int Degree, bool UniqueTriangleAssignment>
void AbstractBoundsTree<Bounds, Degree, UniqueTriangleAssignment>::queryClosestTriangle(const Vertex& vertex, ClosestTriangleQueryResult* result) const {
    if(split){
        // We calculate the closest distance for each child first, as we will calculate them all anyway
        std::array<float, Degree> squaredDistances;
        std::array<unsigned int, Degree> indices;
        for(unsigned int childIndex = 0; childIndex < Degree; childIndex++){

            // Calculate minimal distance to the bounding box
            auto child = children[childIndex];
            squaredDistances[childIndex] = child ? child->bounds.getDistanceSquaredTo(vertex) : std::numeric_limits<float>::max();
            indices[childIndex] = childIndex; // Sorted later
        }

        // We can then sort to visit closer distances first and pruning more work
        // This is much faster than not storing the distances and then sorting them
        std::sort(indices.begin(), indices.end(), [squaredDistances](const auto& indexA, const auto& indexB){
            return squaredDistances[indexA] < squaredDistances[indexB];
        });

        // Visit only children that can lead to improvement
        for (const auto &childIndex : indices){
            if(squaredDistances[childIndex] < result->lowerDistanceBoundSquared){
                children[childIndex]->queryClosestTriangle(vertex, result);
                if(result->lowerDistanceBoundSquared <= 0.0){
                    return;
                }
            }
            else{
                break; // Because of the sorting, the next children won't be visited either
            }
        }
    }
    else{
        for (auto &triangle : triangles){
            auto closestPoint = triangle.getClosestPoint(vertex);
            auto delta = closestPoint - vertex;
            auto distanceSquared = glm::dot(delta, delta);
            if(distanceSquared < result->lowerDistanceBoundSquared){ // If multiple triangles are equally close, the first one will be returned
                result->closestTriangle = &triangle;
                result->lowerDistanceBoundSquared = distanceSquared;
                result->closestVertex = closestPoint;
                if(distanceSquared <= 0.0){
                    return;
                }
            }
        }
    }
}

template <class Bounds, unsigned int Degree, bool UniqueTriangleAssignment>
bool AbstractBoundsTree<Bounds, Degree, UniqueTriangleAssignment>::hasMinimumDistance(const Vertex& vertex, float minimumDistanceSquared) const{
    ClosestTriangleQueryResult queryResult;
    queryResult.lowerDistanceBoundSquared = minimumDistanceSquared;
    queryClosestTriangle(vertex, &queryResult);
    return queryResult.closestTriangle; // True if the result is set, meaning a triangle was found < minimumDistanceSquared
}

template <class Bounds, unsigned int Degree, bool UniqueTriangleAssignment>
float AbstractBoundsTree<Bounds, Degree, UniqueTriangleAssignment>::getShortestDistanceSquared(const Vertex &vertex) const{
    ClosestTriangleQueryResult queryResult;
    queryClosestTriangle(vertex, &queryResult);
    assert(queryResult.closestTriangle); // Assert the result is set
    return queryResult.lowerDistanceBoundSquared;
}

template <class Bounds, unsigned int Degree, bool UniqueTriangleAssignment>
const VertexTriangle* AbstractBoundsTree<Bounds, Degree, UniqueTriangleAssignment>::getClosestTriangle(const Vertex &vertex) const{
    ClosestTriangleQueryResult queryResult;
    queryClosestTriangle(vertex, &queryResult);
    assert(queryResult.closestTriangle); // Assert the result is set
    return queryResult.closestTriangle;
}

template <class Bounds, unsigned int Degree, bool UniqueTriangleAssignment>
Vertex AbstractBoundsTree<Bounds, Degree, UniqueTriangleAssignment>::getClosestPoint(const Vertex &vertex) const{
    ClosestTriangleQueryResult queryResult;
    queryClosestTriangle(vertex, &queryResult);
    assert(queryResult.closestTriangle); // Assert the result is set
    return queryResult.closestVertex;
}


template<class Bounds, unsigned int Degree, bool UniqueTriangleAssignment>
bool AbstractBoundsTree<Bounds, Degree, UniqueTriangleAssignment>::hasMinimumDistance(const VertexTriangle &triangle, float minimumDistanceSquared) const {
    ClosestTriangleQueryResult queryResult;
    queryResult.lowerDistanceBoundSquared = minimumDistanceSquared;
    queryClosestTriangle(triangle, &queryResult);
    return queryResult.closestTriangle; // True if the result is set, meaning a triangle was found < minimumDistanceSquared
}

template<class Bounds, unsigned int Degree, bool UniqueTriangleAssignment>
float AbstractBoundsTree<Bounds, Degree, UniqueTriangleAssignment>::getShortestDistanceSquared(const VertexTriangle &triangle) const {
    ClosestTriangleQueryResult queryResult;
    queryClosestTriangle(triangle, &queryResult);
    assert(queryResult.closestTriangle); // Assert the result is set
    return queryResult.lowerDistanceBoundSquared;
}

template<class Bounds, unsigned int Degree, bool UniqueTriangleAssignment>
const VertexTriangle* AbstractBoundsTree<Bounds, Degree, UniqueTriangleAssignment>::getClosestTriangle(const VertexTriangle &triangle) const {
    ClosestTriangleQueryResult queryResult;
    queryClosestTriangle(triangle, &queryResult);
    assert(queryResult.closestTriangle); // Assert the result is set
    return queryResult.closestTriangle;
}

template<class Bounds, unsigned int Degree, bool UniqueTriangleAssignment>
Vertex AbstractBoundsTree<Bounds, Degree, UniqueTriangleAssignment>::getClosestPoint(const VertexTriangle &triangle) const {
    ClosestTriangleQueryResult queryResult;
    queryClosestTriangle(triangle, &queryResult);
    assert(queryResult.closestTriangle); // Assert the result is set
    return queryResult.closestVertex;
}

#endif //OPTIXMESHCORE_ABSTRACTBOUNDSTREE_H
