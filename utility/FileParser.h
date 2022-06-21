//
// Created by Jonas on 16/11/2020.
//

#ifndef MESHCORE2_FILEPARSER_H
#define MESHCORE2_FILEPARSER_H
#include <string>
#include "../core/ModelSpaceMesh.h"
#include <unordered_map>
#include <mutex>
#include <memory>

class FileParser {
public:
    static std::shared_ptr<ModelSpaceMesh> parseFile(const std::string& filePath);
    static void saveFile(const std::string& filePath, const std::shared_ptr<ModelSpaceMesh>&);

    [[maybe_unused]] static std::vector<std::shared_ptr<ModelSpaceMesh>> parseFolder(const std::string& folderPath);
private:

    static std::mutex cacheMapMutex;
    static std::unordered_map<std::string, std::shared_ptr<ModelSpaceMesh>> meshCacheMap;

    static ModelSpaceMesh parseFileSTL(const std::string& filePath);
    static ModelSpaceMesh parseFileOBJ(const std::string& filePath);
    static void saveFileOBJ(const std::string& filePath, const std::shared_ptr<ModelSpaceMesh>& mesh);
    static ModelSpaceMesh parseFileBinarySTL(const std::string &filePath);
    static std::vector<IndexTriangle> triangulate(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
};

#endif //MESHCORE2_FILEPARSER_H
