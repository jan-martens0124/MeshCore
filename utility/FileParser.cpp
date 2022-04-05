//
// Created by Jonas on 16/11/2020.
//

#include "FileParser.h"
#include "../mapbox/earcut.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <unordered_set>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <array>
#include <cstring>

std::mutex FileParser::cacheMapMutex{};

[[maybe_unused]] std::vector<std::shared_ptr<ModelSpaceMesh>> FileParser::parseFolder(const std::string &folderPath) {

    std::vector<std::shared_ptr<ModelSpaceMesh>> modelSpaceMeshes;

    for(const auto& file: std::filesystem::directory_iterator(folderPath)){
        modelSpaceMeshes.emplace_back(FileParser::parseFile(file.path().string()));
    }
    return modelSpaceMeshes;
}

std::shared_ptr<ModelSpaceMesh> FileParser::parseFile(const std::string &filePath) {

    std::locale::global(std::locale("en_US.UTF-8"));

    std::string extension = filePath.substr(filePath.find_last_of('.') + 1);

    if(!std::filesystem::exists(filePath)){
        std::cout << "Warning: File " << filePath << " does not exist!" << std::endl;
        return std::make_shared<ModelSpaceMesh>(ModelSpaceMesh(std::vector<Vertex>(), std::vector<IndexTriangle>()));
    }

    cacheMapMutex.lock();
    std::unordered_map<std::string, std::shared_ptr<ModelSpaceMesh>> meshCacheMap{};
    const auto cacheIterator = meshCacheMap.find(filePath);
    cacheMapMutex.unlock();
    if(cacheIterator != meshCacheMap.end()){
        std::cout << "Fileparser cache map hit for file " << filePath << std::endl;
        return cacheIterator->second;
    }
    else{
        std::shared_ptr<ModelSpaceMesh> returnModelSpaceMesh;
        if (extension ==  "stl") returnModelSpaceMesh = std::make_shared<ModelSpaceMesh>(parseFileSTL(filePath));
        else if( extension == "obj") returnModelSpaceMesh = std::make_shared<ModelSpaceMesh>(parseFileOBJ(filePath));
        else{
            // Return empty mesh if file extension not supported
            std::cout << "Warning: Extension ." << extension << " of file " << filePath << " not supported!" << std::endl;
            return std::make_shared<ModelSpaceMesh>(ModelSpaceMesh(std::vector<Vertex>(), std::vector<IndexTriangle>()));
        }
        cacheMapMutex.lock();
        meshCacheMap[filePath] = returnModelSpaceMesh;
        cacheMapMutex.unlock();
        return returnModelSpaceMesh;
    }
}

ModelSpaceMesh FileParser::parseFileOBJ(const std::string &filePath) {

    std::ifstream stream(filePath);
    std::vector<Vertex> vertices;
    std::vector<IndexTriangle> triangles;

    std::string line;
    while(getline(stream, line)){
        auto typeLength = line.find_first_of(' ');
        if(typeLength != std::string::npos){
            std::string type = line.substr(0, typeLength);
            std::string content = line.substr(typeLength + 1);

            if(type == "v"){
                auto whitespace0 = content.find_first_of(' ');
                auto whitespace1 = content.find_last_of(' ');
                float value0 = std::stof(content.substr(0, whitespace0));
                float value1 = std::stof(content.substr(whitespace0 + 1, whitespace1 - whitespace0 - 1));
                float value2 = std::stof(content.substr(whitespace1 + 1));
                vertices.emplace_back(Vertex(value0, value1, value2));
            }
            else if (type == "f"){

                std::vector<unsigned int> indices;
                auto whitespace = content.find_first_of(' ');
                while(content.find_first_of(' ')!=std::string::npos){
                    auto string = content.substr(0, whitespace);
                    indices.emplace_back(stoul(string) - 1);
                    content = content.substr(whitespace + 1);
                    whitespace = content.find_first_of(' ');
                }
                indices.emplace_back(stoul(content) - 1);

                for(const IndexTriangle& triangle: triangulate(vertices, indices)){
                    triangles.emplace_back(triangle);
                }

            }
            else if (type == "vp") {
                std::cout << "vp strings in .obj files not supported" << std::endl;
            }
            else if (type == "l"){
                std::cout << "l strings in .obj files not supported" << std::endl;
            }
        }
    }
//    return {vertices, triangles};

    // Remove unused vertices O(V) in time, O(V) in memory
    std::vector<Vertex> finalVertices;
    std::vector<IndexTriangle> finalTriangles;
    std::vector<unsigned int> vertexMapping(vertices.size(), -1); // Element at position i contains the index of vertex i in the hullVertices vector, -1 if not present in hullVertices
    auto addVertexIfNotPresent = [&](unsigned int index){
        if(vertexMapping.at(index)==-1){
            vertexMapping.at(index) = finalVertices.size();
            finalVertices.emplace_back(vertices.at(index));
        }
    };

    for (const auto &indexTriangle : triangles){
        // Put the vertices in the hullVertices vector if not present yet
        addVertexIfNotPresent(indexTriangle.vertexIndex0);
        addVertexIfNotPresent(indexTriangle.vertexIndex1);
        addVertexIfNotPresent(indexTriangle.vertexIndex2);

        unsigned int newIndex0 = vertexMapping.at(indexTriangle.vertexIndex0);
        unsigned int newIndex1 = vertexMapping.at(indexTriangle.vertexIndex1);
        unsigned int newIndex2 = vertexMapping.at(indexTriangle.vertexIndex2);
        finalTriangles.emplace_back(IndexTriangle{newIndex0, newIndex1, newIndex2});
    }

    return {finalVertices, finalTriangles};
}

Vertex readASCIISTLVertexLine(std::ifstream& stream){
    std::string line;
    getline(stream, line);
    assert(line.find("vertex") != std::string::npos);
    auto vertexIndex = line.find("vertex");
    line = line.substr(vertexIndex + 6); // Remove leading vertex word

    while(line.find_first_of(' ') == 0) line = line.substr(1); // Remove leading whitespace
    auto floatIndex = line.find_first_of(' ');
    float x = std::stof(line.substr(0, floatIndex));
    line = line.substr(floatIndex);

    while(line.find_first_of(' ') == 0) line = line.substr(1); // Remove leading whitespace
    floatIndex = line.find_first_of(' ');
    float y = std::stof(line.substr(0, floatIndex));
    line = line.substr(floatIndex);

    while(line.find_first_of(' ') == 0) line = line.substr(1); // Remove leading whitespace
    floatIndex = line.find_first_of(' ');
    float z = std::stof(line.substr(0, floatIndex));

    return {x,y,z};
}

ModelSpaceMesh FileParser::parseFileSTL(const std::string &filePath) {

    std::ifstream stream(filePath);
    std::string line;
    getline(stream, line);
    if(line.find("solid")==std::string::npos){
        stream.close();
        return FileParser::parseFileBinarySTL(filePath);
    }

    std::unordered_set<Vertex> vertexSet;
    std::vector<Vertex> vertices;
    std::vector<IndexTriangle> triangles;
    while(getline(stream, line)){
        auto firstLength = line.find("facet normal");
        if(firstLength != std::string::npos){

            // Begin parsing polygon
            std::vector<Vertex> facetVertices;
            getline(stream, line);
            assert(line.find("outer loop") != std::string::npos);

            std::vector<unsigned int> indices;
            for(int i=0; i<3; i++){
                Vertex vertex = readASCIISTLVertexLine(stream);

                // We try to find if the vertex already exists, as we don't want to duplicate it
                if(vertexSet.find(vertex)!=vertexSet.end()){
                    auto position = std::find(vertices.rbegin(), vertices.rend(), vertex); // Searching the list backwards significantly speeds up the search
                    auto index = (unsigned int)(vertices.size() - std::distance(vertices.rbegin(), position) - 1);
                    assert(index < vertices.size());
                    assert(index >= 0);
                    indices.emplace_back(index);
                }
                else{
                    indices.emplace_back((unsigned int)vertices.size());
                    vertices.emplace_back(vertex);
                    vertexSet.insert(vertex);
                }
            }
            assert(indices.size()==3);
            triangles.emplace_back(IndexTriangle{indices[0], indices[1], indices[2]});

            // Finish parsing polygon
            getline(stream, line);
            assert(line.find("endloop") != std::string::npos);
            getline(stream, line);
            assert(line.find("endfacet") != std::string::npos);
        }
    }
    return {vertices, triangles};
}

float readBinaryFloatLittleEndian(std::ifstream& stream){
    char buffer[4];
    float f;
    stream.read(buffer, 4);
    assert(sizeof(float)==4);
    std::memcpy(&f, buffer, 4);

    return f;
}

Vertex readBinaryVertexLittleEndian(std::ifstream& stream){
    float x = readBinaryFloatLittleEndian(stream);
    float y = readBinaryFloatLittleEndian(stream);
    float z = readBinaryFloatLittleEndian(stream);
    return {x,y,z};
}

unsigned int readBinaryUnsignedIntegerLittleEndian(std::ifstream& stream){
    char buffer[4];
    unsigned int i;
    stream.read(buffer, 4);
    assert(sizeof(unsigned int)==4);
    std::memcpy(&i, buffer, 4);
    return i;
}

ModelSpaceMesh FileParser::parseFileBinarySTL(const std::string &filePath) {

    std::ifstream stream(filePath, std::ios::binary);

    char header[80];
    stream.read(header, 80);

    unsigned int numTriangles = readBinaryUnsignedIntegerLittleEndian(stream);

    std::unordered_set<Vertex> vertexSet;
    std::vector<Vertex> vertices;
    std::vector<IndexTriangle> triangles;
    triangles.reserve(numTriangles);
    char attributes[2];
    char normal[12];
    for(unsigned int i=0; i<numTriangles; i++){
        std::vector<unsigned int> indices;
        stream.read(normal, 12);
        for(int j=0; j<3; j++){

            Vertex vertex = readBinaryVertexLittleEndian(stream);
            // We try to find if the vertex already exists, as we don't want to duplicate it
            if(vertexSet.find(vertex)!=vertexSet.end()){
                auto position = std::find(vertices.rbegin(), vertices.rend(), vertex); // Searching the list backwards significantly speeds up the search
                auto index = (unsigned int)(vertices.size() - std::distance(vertices.rbegin(), position) - 1);
                assert(index < vertices.size());
                indices.emplace_back(index);
            }
            else{
                indices.emplace_back((unsigned int) vertices.size());
                vertices.emplace_back(vertex);
                vertexSet.insert(vertex);
            }
        }
        assert(indices.size()==3);
        triangles.emplace_back(IndexTriangle{indices[0], indices[1], indices[2]});
        stream.read(attributes, 2);
    }
    return {vertices, triangles};
}

std::vector<IndexTriangle> FileParser::triangulate(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {

    assert(indices.size()>=3);
    if(indices.size()==3){
        return std::vector<IndexTriangle>({IndexTriangle{indices[0], indices[1], indices[2]}});
    }

    // TransformUtil the vertices to a plane with constant z coordinates
    glm::vec3 facetNormal(0.0f);

    // Newell's Method to calculate the facet normal
    for (auto current = indices.begin(); current != indices.end(); current++) {
        auto next = std::next(current);
        if(next==indices.end()) next = indices.begin(); // If wrapped

        unsigned int indexA = *current;
        unsigned int indexB = *next;

        Vertex vertexA = vertices[indexA];
        Vertex vertexB = vertices[indexB];

        facetNormal.x += (vertexA.y - vertexB.y) * (vertexA.z + vertexB.z);
        facetNormal.y += (vertexA.z - vertexB.z) * (vertexA.x + vertexB.x);
        facetNormal.z += (vertexA.x - vertexB.x) * (vertexA.y + vertexB.y);
    }

    facetNormal = glm::normalize(facetNormal);

    // Find the rotation for which the z-coordinates of all vertices are equal
    // (the rotation that maps the normal to the z-axis)


    glm::vec3 zAxis(0, 0, 1);
    float angle = glm::angle(zAxis, facetNormal);
    glm::vec3 cross = glm::cross(facetNormal, zAxis);
    if(cross == glm::vec3()){ // TODO should we compare with glm::epsilonEqual?
        // Choose an arbitrary axis to rotate around
        cross = glm::vec3(1,0,0);
    }
    glm::mat4 transformation = glm::rotate(angle, cross);


    // Pass the projected vertices as 2D to the mapbox earcut heuristics
    std::vector<std::vector<std::array<float, 2>>> polygon;
    std::vector<std::array<float, 2>> polyline;
    polyline.reserve(indices.size());
    for (const auto &index : indices) {
        Vertex transformedVertex = transformation * glm::vec4(vertices[index], 1);
        polyline.emplace_back(std::array<float, 2>({transformedVertex.x, transformedVertex.y}));
    }
    polygon.emplace_back(polyline);
    std::vector<int> triangleIndices = mapbox::earcut<int>(polygon);
    assert(triangleIndices.size()%3==0);

    // Vector of triangles to be returned
    std::vector<IndexTriangle> triangles;
    triangles.reserve(triangleIndices.size()/3);
    for (auto iterator = triangleIndices.begin(); iterator!=triangleIndices.end(); iterator++) {
        triangles.emplace_back(IndexTriangle{indices[*iterator++], indices[*iterator++], indices[*iterator]});
    }
    return triangles;
}
