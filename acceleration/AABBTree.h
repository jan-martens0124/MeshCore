//
// Created by Jonas on 15/03/2021.
//

#ifndef MESHCORE_AABBTREE_H
#define MESHCORE_AABBTREE_H

#include "../core/Vertex.h"
#include "../core/Ray.h"
#include "../core/AABB.h"
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

/** Abstract template class for regular AABB tree hierarchies **/
template <unsigned int Degree> class AABBTree {
protected:
    AABB bounds;
    bool split = false;
    bool empty = true;
    unsigned int depth = 0;
    std::vector<VertexTriangle> triangles;
    std::array<std::shared_ptr<AABBTree<Degree>>, Degree> children;
public:
    virtual void splitTopDown(unsigned int maxDepth, unsigned int maxTrianglesPerNode) = 0;
private:
    void getClosestTriangle(const Vertex& vertex, const VertexTriangle* result, float* lowerDistanceBoundSquared) const;

public:
//    AABBTree();
    AABBTree(const AABB &aabb, unsigned int depth): bounds(aabb), split(false), depth(depth), empty(true) {}
    template <unsigned int OtherDegree> [[nodiscard]] bool intersectsAABBTree(const glm::mat4 &otherToThisTransformationMatrix, const AABBTree<OtherDegree>& other, const glm::mat4 &thisToOtherTransformationMatrix) const;
    [[nodiscard]] bool intersectsTriangle(const VertexTriangle& vertexTriangle) const;
    [[nodiscard]] bool intersectsRay(const Ray& ray) const;
    [[nodiscard]] virtual unsigned int getNumberOfRayIntersections(const Ray& ray) const;
    [[nodiscard]] virtual std::unordered_set<VertexTriangle> getIntersectingTriangles(const Ray& ray) const;
    [[nodiscard]] virtual std::unordered_set<VertexTriangle> getIntersectingTriangles(const VertexTriangle& triangle) const;
    [[nodiscard]] VertexTriangle* getClosestTriangle(const Vertex& vertex) const;

    [[nodiscard]] const AABB &getBounds() const;
    [[nodiscard]] bool isSplit() const;
    [[nodiscard]] bool isEmpty() const;
    [[nodiscard]] unsigned int getDepth() const;
    [[nodiscard]] const std::vector<VertexTriangle> &getTriangles() const;
    const std::array<std::shared_ptr<AABBTree<Degree>>, Degree> &getChildren() const;
};

template <unsigned int Degree>
template <unsigned int OtherDegree>
bool AABBTree<Degree>::intersectsAABBTree(const glm::mat4 &otherToThisTransformationMatrix, const AABBTree<OtherDegree> &other,
                                          const glm::mat4 &thisToOtherTransformationMatrix) const {

    if(this->empty || other.isEmpty()){
        assert((!this->split && this->triangles.empty()) || (!other.isSplit() && other.getTriangles().empty()));
        // One of the trees is empty
        return false;
    }

    if(!Intersection::intersect(this->bounds, other.bounds, thisToOtherTransformationMatrix,
                                otherToThisTransformationMatrix)){
        // The bounding boxes do not intersect each other
        return false;
    }

    else if(this->split && other.isSplit()){

///      Go deeper in each tree
//        for(const auto& thisChild: this->children){
//            for(const auto& otherChild: other.getChildren()){
//                if(thisChild->intersectsAABBTree(otherToThisTransformationMatrix, *otherChild, thisToOtherTransformationMatrix)){
//                    return true;
//                }
//            }
//        }

///         Alternatively only go deeper in 1 tree (sometimes faster, sometimes not)
//        for(const auto& thisChild: this->children){
//            if(other.intersectsAABBTree(thisToOtherTransformation, *thisChild, otherToThisTransformation)){
//                return true;
//            }
//        }

        for(const auto& otherChild: other.children){
            if(this->intersectsAABBTree(otherToThisTransformationMatrix, *otherChild, thisToOtherTransformationMatrix)){
                return true;
            }
        }



        return false;
    }
    else if(this->split){
        assert(!other.isSplit());
        for(const auto& thisChild: this->children){
            if(thisChild->intersectsAABBTree(otherToThisTransformationMatrix, other, thisToOtherTransformationMatrix)){
                return true;
            }
        }
        return false;
    }
    else if(other.isSplit()){
        assert(!this->split);
        for(const auto& otherChild: other.getChildren()){
            if(this->intersectsAABBTree(otherToThisTransformationMatrix, *otherChild, thisToOtherTransformationMatrix)){
                return true;
            }
        }
        return false;
    }
    else {
        assert(!this->split && !other.isSplit());
        for(const auto& thisTriangle: this->triangles){
            for(const auto& otherTriangle: other.getTriangles()){
                if(Intersection::intersect(otherTriangle,
                                           thisTriangle.getTransformed(thisToOtherTransformationMatrix))){
//                    std::cout << "Intersection between AABBTrees: " << std::endl;
//                    std::cout << "\t" << Vertex(thisToOtherTransformation * glm::vec4(thisTriangle.vertex0,1)) << std::endl;
//                    std::cout << "\t" << Vertex(thisToOtherTransformation * glm::vec4(thisTriangle.vertex1,1)) << std::endl;
//                    std::cout << "\t" << Vertex(thisToOtherTransformation * glm::vec4(thisTriangle.vertex2,1)) << std::endl;
//                    std::cout << "\t" << /* otherToThisTransformation *  glm::vec4( */ otherTriangle.vertex0 /*, 1)*/ << std::endl;
//                    std::cout << "\t" << /* otherToThisTransformation *  glm::vec4( */ otherTriangle.vertex1 /*, 1)*/ << std::endl;
//                    std::cout << "\t" << /* otherToThisTransformation *  glm::vec4( */ otherTriangle.vertex2 /*, 1)*/ << std::endl;
                    return true;
                }
            }
        }
        return false;
    }
}

template <unsigned int Degree>
bool AABBTree<Degree>::intersectsTriangle(const VertexTriangle &vertexTriangle) const{
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

template <unsigned int Degree>
bool AABBTree<Degree>::intersectsRay(const Ray &ray) const {
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

template<unsigned int Degree>
unsigned int AABBTree<Degree>::getNumberOfRayIntersections(const Ray &ray) const {
    return this->getIntersectingTriangles(ray).size();
}

template<unsigned int Degree>
const AABB &AABBTree<Degree>::getBounds() const {
    return bounds;
}

template<unsigned int Degree>
bool AABBTree<Degree>::isSplit() const {
    return split;
}

template<unsigned int Degree>
bool AABBTree<Degree>::isEmpty() const {
    return empty;
}

template<unsigned int Degree>
unsigned int AABBTree<Degree>::getDepth() const {
    return depth;
}

template<unsigned int Degree>
const std::vector<VertexTriangle> &AABBTree<Degree>::getTriangles() const {
    return triangles;
}

template<unsigned int Degree>
const std::array<std::shared_ptr<AABBTree<Degree>>, Degree> &AABBTree<Degree>::getChildren() const {
    return children;
}

template<unsigned int Degree>
std::unordered_set<VertexTriangle> AABBTree<Degree>::getIntersectingTriangles(const VertexTriangle &triangle) const {
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

template<unsigned int Degree>
std::unordered_set<VertexTriangle> AABBTree<Degree>::getIntersectingTriangles(const Ray &ray) const {
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

template<unsigned int Degree>
VertexTriangle* AABBTree<Degree>::getClosestTriangle(const Vertex &vertex) const{
    VertexTriangle* result = nullptr;
    auto* lowerDistanceBound = new float(std::numeric_limits<float>::max());
    this->getClosestTriangle(vertex, result, lowerDistanceBound);
    return result;
}

template<unsigned int Degree>
void AABBTree<Degree>::getClosestTriangle(const Vertex &vertex, const VertexTriangle* result, float* lowerDistanceBoundSquared) const {
    if(split){

        // We calculate the closest distance for each child first, as we will calculate them all anyway
        std::array<float, Degree> distances;
        std::array<unsigned int, Degree> indices;
        for(unsigned int childIndex = 0; childIndex < Degree; childIndex++){

            // Calculate minimal distance to the bounding box
            const AABBTree<Degree>& child = children[childIndex];
            auto closestPoint = child.bounds.getClosestPoint(vertex);
            auto delta = vertex - closestPoint;
            auto dSquared = glm::dot(delta, delta);

            distances[childIndex] = dSquared;

            indices[childIndex] = childIndex; // Sorted later
        }

        // We can then sort to visit closer distances first and pruning more work
        std::sort(indices.begin(), indices.end(), [distances](const auto& indexA, const auto& indexB){
            return distances[indexA] < distances[indexB];
        });

        // Visit only children that can lead to improvement
        for (const auto &childIndex : indices){
            if(distances[childIndex] < *lowerDistanceBoundSquared){
                children[childIndex]->getClosestTriangle(vertex, result, lowerDistanceBoundSquared);
            }
            else{
                break; // Because of the sorting, the next children won't be visited either
            }
        }

    }
    else{
        for (const auto &triangle : triangles){
            auto closestPoint = triangle.getClosestPoint(vertex);
            auto delta = closestPoint - vertex;
            auto distanceSquared = glm::dot(delta, delta);
            if(distanceSquared < *lowerDistanceBoundSquared){
                result = &triangle;
                *lowerDistanceBoundSquared = distanceSquared;
            }
        }
    }
}


#endif //MESHCORE_AABBTREE_H
