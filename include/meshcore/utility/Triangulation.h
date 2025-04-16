//
// Created by Jonas on 13/02/2025.
//

#ifndef MESHCORE_TRIANGULATE_H
#define MESHCORE_TRIANGULATE_H

#include <vector>
#include "../core/Vertex.h"
#include "../core/IndexFace.h"
#include "../core/IndexTriangle.h"


class Triangulation{
public:
    static std::vector<IndexTriangle> triangulateFace(const std::vector<Vertex>& vertices, const IndexFace& face);
};

#endif //MESHCORE_TRIANGULATE_H
