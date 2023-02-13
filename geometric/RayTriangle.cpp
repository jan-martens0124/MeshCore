//
// Created by Jonas on 5/11/2021.
//

#ifndef OPTIXMESHCORE_RAYTRIANGLE_CPP
#define OPTIXMESHCORE_RAYTRIANGLE_CPP

#define EPSILON 0.00000001f

#include "../core/Ray.h"
#include "../core/VertexTriangle.h"
#include "Intersection.h"

namespace Intersection {

    bool intersect(const Ray& ray, const VertexTriangle& triangle){
        return Intersection::intersectionDistance(ray, triangle) > 0;
    }

    float intersectionDistance(const Ray& ray, const VertexTriangle& triangle) {

        //Möller–Trumbore
        glm::vec3 h = glm::cross(ray.direction, -triangle.edges[2]);

        float a = glm::dot(triangle.edges[0], h);
        if (a > -EPSILON && a < EPSILON) {
            return - std::numeric_limits<float>::max();    // This ray is parallel to this triangle, no line intersection
        }
        float f = 1.0f / a;
        glm::vec3 s = ray.origin - triangle.vertices[0];
        float u = f * (glm::dot(s, h));
        if (u < 0.0 || u > 1.0) {
            return - std::numeric_limits<float>::max(); // No line intersection
        }
        glm::vec3 q = glm::cross(s, triangle.edges[0]);
        float v = f * glm::dot(ray.direction, q);
        if (v < 0.0 || u + v > 1.0) {
            return - std::numeric_limits<float>::max(); // No line intersection
        }
        // At this stage we can compute t to find out where the intersection point is on the line.
        float t = f * glm::dot(-triangle.edges[2], q);
        return t;
    }

    float intersectionDistanceJGT(const Ray& ray, const VertexTriangle& triangle){
        //Möller–Trumbore  // https://github.com/erich666/jgt-code/blob/master/Volume_02/Number_1/Moller1997a/raytri.c
        glm::vec3 h = glm::cross(ray.direction, -triangle.edges[2]);


        /* if determinant is near zero, ray lies in plane of triangle */
        float a = glm::dot(triangle.edges[0], h);

        glm::vec3 q;
        if (a > EPSILON)
        {
            /* calculate distance from vert0 to ray origin */
            glm::vec3 s = ray.origin - triangle.vertices[0];

            /* calculate U parameter and test bounds */
            float u = glm::dot(s, h);
            if (u < 0.0 || u > a)
                return - std::numeric_limits<float>::max();

            /* prepare to test V parameter */
            q = glm::cross(s, triangle.edges[0]);

            /* calculate V parameter and test bounds */
            float v = glm::dot(ray.direction, q);
            if (v < 0.0 || u + v > a)
                return - std::numeric_limits<float>::max();

        }
        else if(a < -EPSILON)
        {
            /* calculate distance from vert0 to ray origin */
            glm::vec3 s = ray.origin - triangle.vertices[0];

            /* calculate U parameter and test bounds */
            float u = glm::dot(s, h);

            if (u > 0.0 || u < a)
                return - std::numeric_limits<float>::max();

            /* prepare to test V parameter */
            q = glm::cross(s, triangle.edges[0]);

            /* calculate V parameter and test bounds */
            float v = glm::dot(ray.direction, q);
            if (v > 0.0 || u + v < a)
                return - std::numeric_limits<float>::max();
        }
        else return - std::numeric_limits<float>::max();  /* ray is parallel to the plane of the triangle */


        float f = 1.0f / a;

        /* calculate t, ray intersects triangle */
        float t = glm::dot(-triangle.edges[2], q) * f;
        return t;
    }
}


#endif //OPTIXMESHCORE_RAYTRIANGLE_CPP
