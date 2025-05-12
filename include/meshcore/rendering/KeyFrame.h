//
// Created by Jonas on 23/04/2024.
//

#ifndef EXTENDEDMESHCORE_KEYFRAME_H
#define EXTENDEDMESHCORE_KEYFRAME_H

#include "meshcore/rendering/PhongMaterial.h"

struct KeyFrameObject{
    std::string group{};
    std::shared_ptr<WorldSpaceMesh> mesh{};
    Transformation transformation{};
    PhongMaterial material{};
};

class KeyFrame{
    std::vector<KeyFrameObject> objects{};

public:
    KeyFrame() = default;

    void addObject(const std::string& group, std::shared_ptr<WorldSpaceMesh> mesh, const Transformation& transformation, const PhongMaterial& material){
        this->addObject({group, mesh, transformation, material});
    }

    void addObject(const std::string& group, std::shared_ptr<WorldSpaceMesh> mesh, const Transformation& transformation, const Color& color){
        this->addObject({group, mesh, transformation, PhongMaterial(color)});
    }

    void addObject(const KeyFrameObject& object){
        objects.push_back(object);
    }

    [[nodiscard]] const std::vector<KeyFrameObject>& getObjects() const {
        return objects;
    }
};

#endif //EXTENDEDMESHCORE_KEYFRAME_H
