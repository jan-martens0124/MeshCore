//
// Created by Jonas on 27/01/2021.
//

#ifndef MESHCORE_WORLDSPACEMESH_H
#define MESHCORE_WORLDSPACEMESH_H

#include "ModelSpaceMesh.h"
#include "Transformation.h"
#include <optional>
#include <memory>

class WorldSpaceMesh {
private:
    static int nextId;
    std::string id;
    Transformation modelTransformation;
    std::shared_ptr<ModelSpaceMesh> modelSpaceMesh;

public:
    WorldSpaceMesh();
    explicit WorldSpaceMesh(std::shared_ptr<ModelSpaceMesh> modelSpaceMesh);
    explicit WorldSpaceMesh(ModelSpaceMesh&& modelSpaceMesh);
    WorldSpaceMesh(const WorldSpaceMesh &other) = default;

    WorldSpaceMesh(WorldSpaceMesh &&other) noexcept = default;
    WorldSpaceMesh& operator=(WorldSpaceMesh &&other) = default;
    ~WorldSpaceMesh() = default;

    [[nodiscard]] WorldSpaceMesh clone () const;

    [[nodiscard]] const std::string& getId() const;
    [[nodiscard]] std::shared_ptr<ModelSpaceMesh> getModelSpaceMesh() const;

    [[nodiscard]] Transformation& getModelTransformation();
    [[nodiscard]] const Transformation& getModelTransformation() const;
    void setModelTransformation(Transformation transformation);
};


#endif //MESHCORE_WORLDSPACEMESH_H
