//
// Created by Jonas on 27/01/2021.
//

#include "ModelSpaceMesh.h"

#include <utility>
#include <unordered_set>

ModelSpaceMesh::ModelSpaceMesh(std::vector<Vertex> vertices, std::vector<IndexTriangle> triangles):
vertices(std::move(vertices)),
triangles(std::move(triangles))
{}

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
