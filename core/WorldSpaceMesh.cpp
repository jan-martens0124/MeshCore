//
// Created by Jonas on 27/01/2021.
//

#include "WorldSpaceMesh.h"
#include "Ray.h"

#include <utility>
#include <string>

int WorldSpaceMesh::nextId = 0;

WorldSpaceMesh::WorldSpaceMesh():
modelSpaceMesh(std::make_shared<ModelSpaceMesh>(ModelSpaceMesh())),
id(std::to_string(nextId++))
{}

WorldSpaceMesh::WorldSpaceMesh(const std::shared_ptr<ModelSpaceMesh>& modelSpaceMesh):
        modelSpaceMesh(modelSpaceMesh),
        modelTransformation(),
        id(std::to_string(nextId++))
{}

const std::string& WorldSpaceMesh::getId() const {
    return id;
}

WorldSpaceMesh::WorldSpaceMesh(ModelSpaceMesh &&modelSpaceMesh):
        WorldSpaceMesh(std::make_shared<ModelSpaceMesh>(modelSpaceMesh))
{}

std::shared_ptr<ModelSpaceMesh> WorldSpaceMesh::getModelSpaceMesh() const {
    return modelSpaceMesh;
}

std::shared_ptr<WorldSpaceMesh> WorldSpaceMesh::clone() const{
    return std::make_shared<WorldSpaceMesh>(*this);
}

std::shared_ptr<WorldSpaceMesh> WorldSpaceMesh::duplicate() const {
    std::shared_ptr<WorldSpaceMesh> returnWorldSpaceMesh = std::make_shared<WorldSpaceMesh>(*this);
    returnWorldSpaceMesh->id = std::to_string(nextId++);
    return returnWorldSpaceMesh;
}

void WorldSpaceMesh::setModelTransformation(Transformation transformation) {
    this->modelTransformation = transformation;
}

const Transformation& WorldSpaceMesh::getModelTransformation() const {
    return modelTransformation;
}

Transformation& WorldSpaceMesh::getModelTransformation() {
    return modelTransformation;
}
