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

template <class Bounds, unsigned int Degree> class AbstractBoundsTree {
protected:
    Bounds bounds;
    bool split = false;
    bool empty = true;
    unsigned int depth = 0;
    std::vector<VertexTriangle> triangles;
    std::array<std::shared_ptr<AbstractBoundsTree<Bounds, Degree>>, Degree> children;

private:
    void getClosestTriangle(const Vertex& vertex, const VertexTriangle** result, float* lowerDistanceBoundSquared) const;

public:
    AbstractBoundsTree(const Bounds &bounds, unsigned int depth): bounds(bounds), split(false), depth(depth), empty(true) {}
    virtual void splitTopDown(unsigned int maxDepth, unsigned int maxTrianglesPerNode) = 0;
//    template <unsigned int OtherDegree> [[nodiscard]] bool intersectsBoundsTree(const glm::mat4 &otherToThisTransformationMatrix, const Bounds<OtherDegree>& other, const glm::mat4 &thisToOtherTransformationMatrix) const; // TODO this one should be implemented for AABB tree specifically

    [[nodiscard]] bool intersectsTriangle(const VertexTriangle& vertexTriangle) const;
    [[nodiscard]] bool intersectsRay(const Ray& ray) const;
    [[nodiscard]] virtual unsigned int getNumberOfRayIntersections(const Ray& ray) const;
    [[nodiscard]] virtual std::unordered_set<VertexTriangle> getIntersectingTriangles(const Ray& ray) const;
    [[nodiscard]] virtual std::unordered_set<VertexTriangle> getIntersectingTriangles(const VertexTriangle& triangle) const;
    [[nodiscard]] const VertexTriangle* getClosestTriangle(const Vertex& vertex) const;
    [[nodiscard]] Vertex getClosestPoint(const Vertex &vertex) const;

    [[nodiscard]] const Bounds &getBounds() const;
    [[nodiscard]] bool isSplit() const;
    [[nodiscard]] bool isEmpty() const;
    [[nodiscard]] unsigned int getDepth() const;
    [[nodiscard]] const std::vector<VertexTriangle> &getTriangles() const;
    const std::array<std::shared_ptr<AbstractBoundsTree<Bounds, Degree>>, Degree> &getChildren() const;

//    static_assert(std::is_function<class Bounds::getClosestPoint>());
};

template <class Bounds, unsigned int Degree>
const Bounds &AbstractBoundsTree<Bounds, Degree>::getBounds() const {
    return bounds;
}

template <class Bounds, unsigned int Degree>
bool AbstractBoundsTree<Bounds, Degree>::isSplit() const {
    return split;
}

template <class Bounds, unsigned int Degree>
bool AbstractBoundsTree<Bounds, Degree>::isEmpty() const {
    return empty;
}

template <class Bounds, unsigned int Degree>
unsigned int AbstractBoundsTree<Bounds, Degree>::getDepth() const {
    return depth;
}

template <class Bounds, unsigned int Degree>
const std::vector<VertexTriangle> &AbstractBoundsTree<Bounds, Degree>::getTriangles() const {
    return triangles;
}

template <class Bounds, unsigned int Degree>
const std::array<std::shared_ptr<AbstractBoundsTree<Bounds, Degree>>, Degree> &AbstractBoundsTree<Bounds, Degree>::getChildren() const {
    return children;
}

template <class Bounds, unsigned int Degree>
bool AbstractBoundsTree<Bounds, Degree>::intersectsTriangle(const VertexTriangle &vertexTriangle) const{
    if(Intersection::intersect(this->bounds, vertexTriangle)){
        if(split){
            assert(triangles.empty());
            return std::any_of(children.begin(), children.end(), [vertexTriangle](const auto& child) {return child->intersectsTriangle(vertexTriangle);});
        }
        else{
            return std::any_of(triangles.begin(), triangles.end(), [vertexTriangle](const auto& triangle) {
                return Intersection::intersect(triangle, vertexTriangle);
            });
        }
    }
    else return false;
}

template <class Bounds, unsigned int Degree>
bool AbstractBoundsTree<Bounds, Degree>::intersectsRay(const Ray &ray) const {
    if(Intersection::intersect(this->bounds, ray)){
        if(split){
            assert(triangles.empty());
            return std::any_of(children.begin(), children.end(), [ray](const auto& child) {return child->intersectsRay(ray);});
        }
        else{
            return std::any_of(triangles.begin(), triangles.end(), [ray](const auto& triangle) {
                return Intersection::intersect(ray, triangle);
            });
        }
    }
    else return false;
}

template <class Bounds, unsigned int Degree>
unsigned int AbstractBoundsTree<Bounds, Degree>::getNumberOfRayIntersections(const Ray &ray) const {
    return this->getIntersectingTriangles(ray).size();
//    // Code below is more efficient but this is only correct if each triangle is only present in a single node on each level, which we can't know
//    if(Intersection::intersect(this->bounds, ray)){
//        unsigned int intersectingTriangles = 0u;
//        if(split){
//            assert(triangles.empty()); // A split node shouldn't contain any triangles
//            for(const auto& child: children){
//                auto childIntersectingTriangles = child->getNumberOfRayIntersections(ray);
//                intersectingTriangles += childIntersectingTriangles;
//            }
//        }
//        else{
//            for(const auto& triangle: triangles){
//                if(Intersection::intersect(ray, triangle)){
//                    intersectingTriangles++;
//                }
//            }
//        }
//        return intersectingTriangles;
//    }
//    else{
//        return 0u;
//    }
}

template <class Bounds, unsigned int Degree>
std::unordered_set<VertexTriangle> AbstractBoundsTree<Bounds, Degree>::getIntersectingTriangles(const Ray &ray) const {
    if(Intersection::intersect(this->bounds, ray)){
        std::unordered_set<VertexTriangle> intersectingTriangles;
        if(split){
            assert(triangles.empty()); // A split node shouldn't contain any triangles
            for(const auto& child: children){
                auto childIntersectingTriangles = child->getIntersectingTriangles(ray);
                intersectingTriangles.insert(childIntersectingTriangles.begin(), childIntersectingTriangles.end());
            }
        }
        else{
            for(const auto& triangle: triangles){
                if(Intersection::intersect(ray, triangle)){
                    intersectingTriangles.insert(triangle);
                }
            }
        }
        return intersectingTriangles;
    }
    else{
        return {};
    }
}

template <class Bounds, unsigned int Degree>
std::unordered_set<VertexTriangle> AbstractBoundsTree<Bounds, Degree>::getIntersectingTriangles(const VertexTriangle &triangle) const {
    if(Intersection::intersect(this->bounds, triangle)){
        std::unordered_set<VertexTriangle> intersectingTriangles;
        if(split){
            assert(triangles.empty()); // A split node shouldn't contain any triangles
            for(const auto& child: children){
                auto childIntersectingTriangles = child->getIntersectingTriangles(triangle);
                intersectingTriangles.insert(childIntersectingTriangles.begin(), childIntersectingTriangles.end());
            }
        }
        else{
            for(const auto& triangle: triangles){
                if(Intersection::intersect(triangle, triangle)){
                    intersectingTriangles.insert(triangle);
                }
            }
        }
        return intersectingTriangles;
    }
    else{
        return {};
    }
}

template <class Bounds, unsigned int Degree>
const VertexTriangle* AbstractBoundsTree<Bounds, Degree>::getClosestTriangle(const Vertex &vertex) const{
    const VertexTriangle* closestTriangle = nullptr;
    const VertexTriangle** result = &closestTriangle;
    auto* lowerDistanceBoundSquared = new float(std::numeric_limits<float>::max());
    this->getClosestTriangle(vertex, result, lowerDistanceBoundSquared);
    assert(*result);
    return *result;
}

template <class Bounds, unsigned int Degree>
Vertex AbstractBoundsTree<Bounds, Degree>::getClosestPoint(const Vertex &vertex) const{
    const VertexTriangle* closestTriangle = nullptr;
    const VertexTriangle** result = &closestTriangle;
    auto* lowerDistanceBoundSquared = new float(std::numeric_limits<float>::max());
    this->getClosestTriangle(vertex, result, lowerDistanceBoundSquared);
    assert(*result);
    return (*result)->getClosestPoint(vertex);
}

template <class Bounds, unsigned int Degree>
void AbstractBoundsTree<Bounds, Degree>::getClosestTriangle(const Vertex &vertex, const VertexTriangle** result, float* lowerDistanceBoundSquared) const {
    if(split){
        // We calculate the closest distance for each child first, as we will calculate them all anyway
        std::array<float, Degree> squaredDistances;
        std::array<unsigned int, Degree> indices;
        for(unsigned int childIndex = 0; childIndex < Degree; childIndex++){

            // Calculate minimal distance to the bounding box
            auto child = children[childIndex];
            squaredDistances[childIndex] = child->bounds.getDistanceSquaredTo(vertex);
            indices[childIndex] = childIndex; // Sorted later
        }

        // We can then sort to visit closer distances first and pruning more work
        // This is much faster than not storing the distances and then sorting them
        std::sort(indices.begin(), indices.end(), [squaredDistances](const auto& indexA, const auto& indexB){
            return squaredDistances[indexA] < squaredDistances[indexB];
        });

        // Visit only children that can lead to improvement
        for (const auto &childIndex : indices){
            if(squaredDistances[childIndex] < *lowerDistanceBoundSquared){
                children[childIndex]->getClosestTriangle(vertex, result, lowerDistanceBoundSquared);
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
            if(distanceSquared < *lowerDistanceBoundSquared){ // If multiple triangles are equally close, the first one will be returned
                *result = &triangle;
                *lowerDistanceBoundSquared = distanceSquared;
            }
        }
    }
}

template<unsigned int Degree>
struct AbstractBoundsTrees{
    typedef AbstractBoundsTree<AABB, Degree> AABBTree;
    typedef AbstractBoundsTree<OBB, Degree> OBBTree;
    typedef AbstractBoundsTree<Sphere, Degree> SphereTree;
};

#endif //OPTIXMESHCORE_ABSTRACTBOUNDSTREE_H
