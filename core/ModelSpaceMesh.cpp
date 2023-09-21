//
// Created by Jonas on 27/01/2021.
//

#include "ModelSpaceMesh.h"

#include <utility>
#include <unordered_set>
#include <iostream>
#include "../factories/AABBFactory.h"
#include "../external/quickhull/QuickHull.hpp"

#define EPSILON 1e-4

ModelSpaceMesh::ModelSpaceMesh(std::vector<Vertex> vertices, std::vector<IndexTriangle> triangles):
vertices(std::move(vertices)),
triangles(std::move(triangles))
{
    assert(!this->vertices.empty());
}

const std::vector<Vertex>& ModelSpaceMesh::getVertices() const {
    return vertices;
}

const std::vector<IndexTriangle>& ModelSpaceMesh::getTriangles() const {
    return triangles;
}

const std::vector<IndexEdge>& ModelSpaceMesh::getEdges() const {
    if(!edges.has_value()){
        // Set up the hash and equals in a way that the order of vertexIndex0 and vertexIndex1 doesn't matter
        auto hash = [](const IndexEdge& edge) { return std::hash<unsigned int>()(edge.vertexIndex0 + edge.vertexIndex1); }; // Hashes should remain equal if vertices are swapped
        auto equal = [](const IndexEdge& edge1, const IndexEdge& edge2) {
            return (edge1.vertexIndex0 == edge2.vertexIndex0 && edge1.vertexIndex1 == edge2.vertexIndex1) ||
                   (edge1.vertexIndex1 == edge2.vertexIndex0 && edge1.vertexIndex0 == edge2.vertexIndex1); };
        std::unordered_set<IndexEdge, decltype(hash), decltype(equal)> edgeSet(8, hash, equal);
        for(IndexTriangle triangle: this->triangles){
            edgeSet.insert(IndexEdge{triangle.vertexIndex0, triangle.vertexIndex1});
            edgeSet.insert(IndexEdge{triangle.vertexIndex1, triangle.vertexIndex2});
            edgeSet.insert(IndexEdge{triangle.vertexIndex2, triangle.vertexIndex0});
        }

        this->edges = std::vector<IndexEdge>(edgeSet.begin(), edgeSet.end());
    }
    return edges.value();
}

std::vector<IndexEdge> ModelSpaceMesh::getSufficientIntersectionEdges() const {
    // Set up the hash and equals in a way that the order of vertexIndex0 and vertexIndex1 doesn't matter
    auto hash = [](const IndexEdge& edge) { return std::hash<unsigned int>()(edge.vertexIndex0 + edge.vertexIndex1); }; // Hashes should remain equal if vertices are swapped
    auto equal = [](const IndexEdge& edgeA, const IndexEdge& edgeB) {

        bool equal = (edgeA.vertexIndex0 == edgeB.vertexIndex0 && edgeA.vertexIndex1 == edgeB.vertexIndex1) ||
                     (edgeA.vertexIndex1 == edgeB.vertexIndex0 && edgeA.vertexIndex0 == edgeB.vertexIndex1);
        return equal;
    };
    std::unordered_set<IndexEdge, decltype(hash), decltype(equal)> edgeSet(8, hash, equal);
    std::unordered_set<IndexEdge, decltype(hash), decltype(equal)> preferNotUsed(8, hash, equal);
    for(IndexTriangle triangle: this->triangles){

        auto edge0 = IndexEdge{triangle.vertexIndex0, triangle.vertexIndex1};
        auto edge1 = IndexEdge{triangle.vertexIndex1, triangle.vertexIndex2};
        auto edge2 = IndexEdge{triangle.vertexIndex2, triangle.vertexIndex0};

        bool present0 = (edgeSet.find(edge0) != edgeSet.end());
        bool present1 = (edgeSet.find(edge1) != edgeSet.end());
        bool present2 = (edgeSet.find(edge2) != edgeSet.end());

        unsigned int nPresent = 0;
        if(present0) nPresent++;
        if(present1) nPresent++;
        if(present2) nPresent++;

        while(nPresent<2){
            if(!present0 && !(preferNotUsed.find(edge0) != preferNotUsed.end())){
                edgeSet.insert(edge0);
                present0=true;
                nPresent++;
            }
            else if(!present1 && !(preferNotUsed.find(edge1) != preferNotUsed.end())){
                edgeSet.insert(edge1);
                present1=true;
                nPresent++;
            }
            else if(!present2 && !(preferNotUsed.find(edge2) != preferNotUsed.end())){
                edgeSet.insert(edge2);
                present2=true;
                nPresent++;
            }
            else if(!present0){
                edgeSet.insert(edge0);
                present0=true;
                nPresent++;
            }
            else if(!present1){
                edgeSet.insert(edge1);
                present1=true;
                nPresent++;
            }
            else if(!present2){
                edgeSet.insert(edge2);
                present2=true;
                nPresent++;
            }
            else assert(false);

        }
        if(!present0) preferNotUsed.insert(edge0);
        if(!present1) preferNotUsed.insert(edge1);
        if(!present2) preferNotUsed.insert(edge2);
    }

    std::vector<IndexEdge> returnVector(edgeSet.begin(), edgeSet.end());

#if !NDEBUG
    for(IndexTriangle triangle: this->triangles){
        auto edge0 = IndexEdge{triangle.vertexIndex0, triangle.vertexIndex1};
        auto edge1 = IndexEdge{triangle.vertexIndex1, triangle.vertexIndex2};
        auto edge2 = IndexEdge{triangle.vertexIndex2, triangle.vertexIndex0};

        bool present0 = (edgeSet.find(edge0) != edgeSet.end());
        bool present1 = (edgeSet.find(edge1) != edgeSet.end());
        bool present2 = (edgeSet.find(edge2) != edgeSet.end());

        unsigned int nPresent = 0;
        if(present0) nPresent++;
        if(present1) nPresent++;
        if(present2) nPresent++;

        assert(nPresent>=2);
    }

    std::cout << std::endl;
    std::cout << "Generated sufficient set of edges" << std::endl;
    std::cout << "\tDefault number of edges:\t" << this->getEdges().size() << std::endl;
    std::cout << "\tSufficient number of edges:\t" << returnVector.size() << std::endl;
    std::cout << "\tSufficient on default ratio:\t" << float(returnVector.size())/this->getEdges().size() << std::endl;
    std::cout << "\tPercentage theoretical minimum:\t" << (float(returnVector.size())/this->getEdges().size())/(2.0f/3) << std::endl;
    std::cout << std::endl;

#endif
    return returnVector;
}

std::vector<IndexTriangle> computeQuickHullExternal(const std::vector<Vertex>& vertices){

    quickhull::QuickHull<float> qh;
    std::vector<quickhull::Vector3<float>> qhVertices;

    qhVertices.reserve(vertices.size());
    for(Vertex vertex: vertices){
        qhVertices.emplace_back(vertex.x, vertex.y, vertex.z);
    }

    auto hull = qh.getConvexHull(qhVertices, false, true);

    const auto& indexBuffer = hull.getIndexBuffer();

    std::vector<IndexTriangle> triangles;
    for(unsigned int i=0; i<indexBuffer.size(); i+=3){
        triangles.emplace_back(IndexTriangle{(unsigned int)(indexBuffer[i]), (unsigned int)(indexBuffer[i+1]), (unsigned int)(indexBuffer[i+2])});
    }

    return triangles;
}

std::optional<std::shared_ptr<ModelSpaceMesh>> ModelSpaceMesh::getConvexHull() const {

    // Return the value if already calculated before
    if(convexHull.has_value()){

        // If previously no mesh was found, return empty optional
        if(convexHull.value() == nullptr) return std::nullopt;
        return convexHull.value();
    }

    // Get the triangles that should be part of the convex hull
    const auto indexTriangles = computeQuickHullExternal(this->vertices);

    if(indexTriangles.empty()){
        convexHull = nullptr;
        return std::nullopt;
    }

    // Create a new modelSpaceMesh that only contains vertices that are part of the outer hull
    std::vector<Vertex> hullVertices;
    std::vector<IndexTriangle> hullTriangles;

    // Helper function to keep track of the vertices that should be present in the final result
    std::vector<unsigned int> vertexMapping(this->vertices.size(), -1); // Element at position i contains the index of vertex i in the hullVertices vector, -1 if not present in hullVertices
    auto addVertexIfNotPresent = [&](unsigned int index){
        if(vertexMapping.at(index)==-1){
            vertexMapping.at(index) = hullVertices.size();
            hullVertices.emplace_back(this->vertices.at(index));
        }
    };

    for (const auto &indexTriangle : indexTriangles){
        // Put the vertices in the hullVertices vector if not present yet
        addVertexIfNotPresent(indexTriangle.vertexIndex0);
        addVertexIfNotPresent(indexTriangle.vertexIndex1);
        addVertexIfNotPresent(indexTriangle.vertexIndex2);

        unsigned int newIndex0 = vertexMapping.at(indexTriangle.vertexIndex0);
        unsigned int newIndex1 = vertexMapping.at(indexTriangle.vertexIndex1);
        unsigned int newIndex2 = vertexMapping.at(indexTriangle.vertexIndex2);
        hullTriangles.emplace_back(IndexTriangle{newIndex0, newIndex1, newIndex2});
    }

    assert(!convexHull.has_value() && "This shouldn't be calculated again if a value was present already");
    auto hull = std::make_shared<ModelSpaceMesh>(hullVertices, hullTriangles);
    convexHull = hull;
    convexHull.value()->setName("Convex hull of " + this->getName());
    return hull;
}

const std::string &ModelSpaceMesh::getName() const {
    return name;
}

void ModelSpaceMesh::setName(const std::string &newName) {
    ModelSpaceMesh::name = newName;
}

float ModelSpaceMesh::getVolume() const {
    if(!volume.has_value()){
        computeVolumeAndCentroid();
    }
    return volume.value();
}

bool ModelSpaceMesh::isConvex() const{

    // Return the value if already calculated before
    if(!convex.has_value()){
        computeConvexity();
    }

    return convex.value();
}

void ModelSpaceMesh::computeConvexity() const {
    // Calculate the convexity
    convex = true;
    for (const auto &indexTriangle: this->triangles){

        Vertex vertex0 = this->vertices.at(indexTriangle.vertexIndex0);
        Vertex vertex1 = this->vertices.at(indexTriangle.vertexIndex1);
        Vertex vertex2 = this->vertices.at(indexTriangle.vertexIndex2);

        VertexTriangle triangle({vertex0, vertex1, vertex2});

        for (const auto &vertex : this->vertices){

            // The dot product between the triangle's normal and the vector from the triangle to the vertex should be negative
            auto normal = glm::normalize(triangle.normal);
            auto delta = glm::normalize(vertex - vertex0);
            auto dot = glm::dot(normal, delta);

            if(dot > EPSILON){
                convex = false;
                return;
            }
        }
    }
}

void ModelSpaceMesh::computeSurfaceAreaAndCentroid() const {
    auto meshSurfaceArea = 0.0f;
    auto centroid = Vertex(0.0f, 0.0f, 0.0f);
    for (const auto &indexTriangle: this->triangles){

        const auto v0 = this->vertices.at(indexTriangle.vertexIndex0);
        const auto v1 = this->vertices.at(indexTriangle.vertexIndex1);
        const auto v2 = this->vertices.at(indexTriangle.vertexIndex2);

        auto edge0(v1-v0);
        auto edge1(v2-v1);
        auto triangleArea = glm::length(glm::cross(edge0, edge1)); // 2.0f * the area of the triangle
        auto center = (v0 + v1 + v2); // 3.0f * the center of the triangle

        centroid += center * triangleArea;
        meshSurfaceArea += glm::length(glm::cross(edge0, edge1));
    }
    surfaceArea = meshSurfaceArea / 2.0f;
    surfaceCentroid = centroid / (meshSurfaceArea*3.0f);
    assert(getBounds().containsPoint(surfaceCentroid.value()) && "The surface centroid should be inside the bounding box");
}

Vertex ModelSpaceMesh::getVolumeCentroid() const {
    if(!volumeCentroid.has_value()){
        computeVolumeAndCentroid();
    }
    return volumeCentroid.value();
}

Vertex ModelSpaceMesh::getSurfaceCentroid() const {
    if(!surfaceCentroid.has_value()){
        computeSurfaceAreaAndCentroid();
    }
    return surfaceCentroid.value();
}

const AABB &ModelSpaceMesh::getBounds() const {
    if(!this->bounds.has_value()){
        this->bounds = AABBFactory::createAABB(this->vertices);
    }
    return this->bounds.value();
}

float ModelSpaceMesh::getSurfaceArea() const {
    if(!surfaceArea.has_value()){
        computeSurfaceAreaAndCentroid();
    }
    return surfaceArea.value();
}

void ModelSpaceMesh::computeVolumeAndCentroid() const {
    auto meshVolume = 0.0f;
    auto centroid = Vertex(0.0f, 0.0f, 0.0f);

    for (const auto &triangle: triangles){
        auto v0 = vertices.at(triangle.vertexIndex0);
        auto v1 = vertices.at(triangle.vertexIndex1);
        auto v2 = vertices.at(triangle.vertexIndex2);

        auto center = (v0 + v1 + v2);         // 4.0f * the center of tetrahedron resembled by triangle and origin
        auto _volume = dot(v0, cross(v1, v2));  // 6.0f * the signed volume of the

        meshVolume += _volume;
        centroid += center * _volume;
    }
    volume = meshVolume/6.0f;
    assert(volume.value()/this->getBounds().getVolume() <= (1.0 + 1e-6)  && "Volume of mesh should be smaller than volume of its bounding box");
    volumeCentroid = centroid / (meshVolume*4.0f);
    assert(getBounds().containsPoint(volumeCentroid.value()) && "Volume centroid should be inside the bounding box of the mesh");
}

