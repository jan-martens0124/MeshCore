//
// Created by Jonas Tollenaere on 30/06/2025.
//

#ifndef AABBTRIANGLEDATA_H
#define AABBTRIANGLEDATA_H

#include "meshcore/core/VertexTriangle.h"

/**
 * @brief Represents precomputed data for axis-aligned bounding box (AABB) triangle intersection tests.
 *
 * This structure stores edge normals and base distances for triangle edges projected onto the XY, YZ, and ZX planes.
 * It is used to efficiently compute intersection tests between triangles and AABBs.
 */
struct AABBTriangleData{

    glm::vec2 ne0xy, ne1xy, ne2xy;
    float de0xy_base, de1xy_base, de2xy_base;
    glm::vec2 ne0yz,ne1yz,ne2yz;
    float de0yz_base, de1yz_base, de2yz_base;
    glm::vec2 ne0zx, ne1zx, ne2zx;
    float de0zx_base, de1zx_base, de2zx_base;

    explicit AABBTriangleData(const VertexTriangle& t){
        auto xym = glm::sign(t.normal.z);
        ne0xy.x = xym * -t.edges[0].y;
        ne0xy.y = xym * t.edges[0].x;
        ne1xy.x = xym * -t.edges[1].y;
        ne1xy.y = xym * t.edges[1].x;
        ne2xy.x = xym * -t.edges[2].y;
        ne2xy.y = xym * t.edges[2].x;

        auto zxm = glm::sign(t.normal.y);
        ne0zx.x = zxm * -t.edges[0].x;
        ne0zx.y = zxm * t.edges[0].z;
        ne1zx.x = zxm * -t.edges[1].x;
        ne1zx.y = zxm * t.edges[1].z;
        ne2zx.x = zxm * -t.edges[2].x;
        ne2zx.y = zxm * t.edges[2].z;

        auto yzm = glm::sign(t.normal.x);
        ne0yz.x = yzm * -t.edges[0].z;
        ne0yz.y = yzm * t.edges[0].y;
        ne1yz.x = yzm * -t.edges[1].z;
        ne1yz.y = yzm * t.edges[1].y;
        ne2yz.x = yzm * -t.edges[2].z;
        ne2yz.y = yzm * t.edges[2].y;

        de0xy_base = -(ne0xy.x * t.vertices[0].x + ne0xy.y * t.vertices[0].y);
        de1xy_base = -(ne1xy.x * t.vertices[1].x + ne1xy.y * t.vertices[1].y);
        de2xy_base = -(ne2xy.x * t.vertices[2].x + ne2xy.y * t.vertices[2].y);

        de0yz_base = -(ne0yz.x * t.vertices[0].y + ne0yz.y * t.vertices[0].z);
        de1yz_base = -(ne1yz.x * t.vertices[1].y + ne1yz.y * t.vertices[1].z);
        de2yz_base = -(ne2yz.x * t.vertices[2].y + ne2yz.y * t.vertices[2].z);

        de0zx_base = -(ne0zx.x * t.vertices[0].z + ne0zx.y * t.vertices[0].x);
        de1zx_base = -(ne1zx.x * t.vertices[1].z + ne1zx.y * t.vertices[1].x);
        de2zx_base = -(ne2zx.x * t.vertices[2].z + ne2zx.y * t.vertices[2].x);
    };
};

#endif //AABBTRIANGLEDATA_H
