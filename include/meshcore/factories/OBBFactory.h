//
// Created by Jonas on 17/03/2022.
//

#ifndef MESHCORE_OBBFACTORY_H
#define MESHCORE_OBBFACTORY_H

#include "AABBFactory.h"
#include <gsl/gsl_eigen.h>
#include <unordered_set>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

#include "meshcore/utility/hash.h"
#include "meshcore/core/ModelSpaceMesh.h"
#include "meshcore/core/OBB.h"

class OBBFactory{

public:
    static OBB createOBB(const std::vector<Vertex>& vertices){
        return createOBB(vertices.begin(), vertices.end(), vertices.size());
    }

    static OBB createOBB(const VertexTriangle& vertexTriangle){

        // We look for the quaternion that aligns the axis with the normal and one edge of the triangle
        auto axisX = glm::normalize( vertexTriangle.edges[0]);
        auto axisY = glm::normalize(vertexTriangle.normal);
        auto axisZ = glm::normalize(glm::cross(axisX, axisY));
        Quaternion quaternion({axisX, axisY, axisZ});

        // This quaternion should align the primary x, y and z axis with the required ones
        assert(glm::all(glm::epsilonEqual(quaternion.rotateVertex(glm::vec3(1.0f,0.0f,0.0f)), axisX, 1e-6f)));
        assert(glm::all(glm::epsilonEqual(quaternion.rotateVertex(glm::vec3(0.0f,1.0f,0.0f)), axisY, 1e-6f)));
        assert(glm::all(glm::epsilonEqual(quaternion.rotateVertex(glm::vec3(0.0f,0.0f,1.0f)), axisZ, 1e-6f)));

        // This quaternion should align the triangles edge and normal with the primary ones in OBB space
        assert(glm::all(glm::epsilonEqual(quaternion.inverseRotateVertex(glm::normalize(vertexTriangle.edges[0])), glm::vec3(1.0f,0.0f,0.0f), 1e-6f)));
        assert(glm::all(glm::epsilonEqual(quaternion.inverseRotateVertex(glm::normalize(vertexTriangle.normal)), glm::vec3(0.0f,1.0f,0.0f), 1e-6f)));

        // Rotate the vertices of the triangle to OBB space
        auto rotatedVertex0 = quaternion.inverseRotateVertex(vertexTriangle.vertices[0]);
        auto rotatedVertex1 = quaternion.inverseRotateVertex(vertexTriangle.vertices[1]);
        auto rotatedVertex2 = quaternion.inverseRotateVertex(vertexTriangle.vertices[2]);

        // The vertices should lie in the same XZ plane
        assert(glm::epsilonEqual(rotatedVertex0.y, rotatedVertex1.y, 1e-6f));
        assert(glm::epsilonEqual(rotatedVertex0.y, rotatedVertex2.y, 1e-6f));
        assert(glm::epsilonEqual(rotatedVertex1.y, rotatedVertex2.y, 1e-6f));

        // Edge 0 should align with the X axis
        assert(glm::epsilonEqual(rotatedVertex1.y, rotatedVertex0.y, 1e-5f)); // Redundant
        assert(glm::epsilonEqual(rotatedVertex1.z, rotatedVertex0.z, 1e-5f));

        auto aabb = AABBFactory::createAABB({rotatedVertex0, rotatedVertex1, rotatedVertex2});

        assert(glm::epsilonEqual(aabb.getVolume(), 0.0f, 1e-5f));

        return {aabb, quaternion};
    }

    static OBB createOBB(const VertexTriangle& vertexTriangle0, const VertexTriangle& vertexTriangle1){

        std::vector<Quaternion> alignmentsToEvaluate;
        // This is O(N^2), but probably won't yield exact solutions for more triangles // TODO test
        // Add the quaternions that align with the edges of the first triangle
        alignmentsToEvaluate.emplace_back(Quaternion({glm::normalize(vertexTriangle0.edges[0]),glm::normalize(vertexTriangle0.normal),
                                                      glm::normalize(glm::cross(vertexTriangle0.edges[0], vertexTriangle0.normal))}));
        alignmentsToEvaluate.emplace_back(Quaternion({glm::normalize(vertexTriangle0.edges[1]),glm::normalize(vertexTriangle0.normal),
                                                      glm::normalize(glm::cross(vertexTriangle0.edges[1], vertexTriangle0.normal))}));
        alignmentsToEvaluate.emplace_back(Quaternion({glm::normalize(vertexTriangle0.edges[2]),glm::normalize(vertexTriangle0.normal),
                                                      glm::normalize(glm::cross(vertexTriangle0.edges[2], vertexTriangle0.normal))}));

        // Add the quaternions that align with the edges of the second triangle
        alignmentsToEvaluate.emplace_back(Quaternion({glm::normalize(vertexTriangle1.edges[0]),glm::normalize(vertexTriangle1.normal),
                                                      glm::normalize(glm::cross(vertexTriangle1.edges[0], vertexTriangle1.normal))}));
        alignmentsToEvaluate.emplace_back(Quaternion({glm::normalize(vertexTriangle1.edges[1]),glm::normalize(vertexTriangle1.normal),
                                                      glm::normalize(glm::cross(vertexTriangle1.edges[1], vertexTriangle1.normal))}));
        alignmentsToEvaluate.emplace_back(Quaternion({glm::normalize(vertexTriangle1.edges[2]),glm::normalize(vertexTriangle1.normal),
                                                      glm::normalize(glm::cross(vertexTriangle1.edges[2], vertexTriangle1.normal))}));

        auto smallestVolume = std::numeric_limits<float>::max();
        AABB smallestAABB;
        Quaternion bestAlignment;
        for (const auto &quaternion: alignmentsToEvaluate){
            auto aabb = AABBFactory::createAABB({
                quaternion.inverseRotateVertex(vertexTriangle0.vertices[0]),
                quaternion.inverseRotateVertex(vertexTriangle0.vertices[1]),
                quaternion.inverseRotateVertex(vertexTriangle0.vertices[2]),
                quaternion.inverseRotateVertex(vertexTriangle1.vertices[0]),
                quaternion.inverseRotateVertex(vertexTriangle1.vertices[1]),
                quaternion.inverseRotateVertex(vertexTriangle1.vertices[2])
            });

            if(aabb.getVolume() < smallestVolume){
                smallestVolume = aabb.getVolume();
                smallestAABB = aabb;
                bestAlignment = quaternion;
            }
        }
        // Assert actual Quaternion is selected
        assert(smallestVolume!=std::numeric_limits<float>::max());
        assert(smallestAABB!=AABB());
        return {smallestAABB, bestAlignment};
    }

    static OBB createOBB(const std::vector<VertexTriangle>& triangles){
        if(triangles.size()==1){
            return createOBB(triangles[0]);
        } else if(triangles.size()==2){
            return createOBB(triangles[0], triangles[1]);
        } else {
            std::unordered_set<Vertex> vertices;
            for (const auto &item : triangles){ // Is this set worth it? We might as well just count every vertex twice instead of hashing it (PCA works in O(n) anyway)
                vertices.insert(item.vertices[0]);
                vertices.insert(item.vertices[1]);
                vertices.insert(item.vertices[2]);
            }
            return createOBB(vertices.begin(), vertices.end(), vertices.size());
        }
    }

    static OBB createOBB(const std::shared_ptr<ModelSpaceMesh>& modelSpaceMesh){
        auto vertices = modelSpaceMesh->getVertices();

        // TODO
        // Try to create an optimal OBB if the number of vertices is limited
//        if(vertices.size()<=20){
//            // Find the optimal OBB using the convex hull
//            std::optional<OBB> optimalOBB = findOptimalOBB(*modelSpaceMesh->getConvexHull());
//            if(optimalOBB.has_value()){
//                return optimalOBB.value();
//            }
//        }

        // If the number of vertices is too high, or the optimal OBB could not be found, use PCA
        auto pcaOBB = findPCAOBB(vertices.begin(), vertices.end());

        // TODO test if we can improve heuristically

        // Compare with the AABB of the model space mesh that is present anyway, use it if the OBB is worse
        if(pcaOBB.getVolume() <= modelSpaceMesh->getBounds().getVolume()){
            return pcaOBB;
        }
        else return {modelSpaceMesh->getBounds(), Quaternion()};
    }

private:

    template<class Iter>
    static OBB createOBB(const Iter& first, const Iter& last, unsigned int numberOfVertices){

        // Try to create an optimal OBB if the number of vertices is limited
//        if(numberOfVertices<=20){
//
//            // Create the convex hull
//            std::vector<Vertex> vertices(first, last);
//            ModelSpaceMesh tempMesh(vertices, {});
//            auto convexHull = tempMesh.getConvexHull();
//
//            // Find the optimal OBB using this hull
//            std::optional<OBB> optimalOBB = findOptimalOBB(*convexHull);
//            if(optimalOBB.has_value()){
//                return optimalOBB.value();
//            }
//        }

        // If the number of vertices is too high, or the optimal OBB could not be found, use PCA
        return findPCAOBB(first, last);
    }

    /** construct an exact OBB **/
    static std::optional<OBB> findOptimalOBB(const ModelSpaceMesh& convexHull){

        // Find the optimal OBB based on this convex hull calculation
        // An Exact Algorithm for Finding Minimum Oriented Bounding Boxes by Jukka Jylanki, improving on O'Rourke
        // TODO implement Jylanki's algorithm
        return std::nullopt;
    }

    /** construct an OBB based on principal component analysis **/
    template<class Iter>
    static OBB findPCAOBB(const Iter& first, const Iter& last){

        //0. Calculate the mean and the number of elements
        Vertex totalSum(0.0f);
        unsigned int n = 0;
        for(auto iter = first;iter!=last; ++iter){
            totalSum += *iter;
            n++;
        }
        assert(n>=2);
        Vertex mean = totalSum / float(n);

        //1. Find the covariance matrix
        glm::mat3 covarianceMatrix;
        for(int i=0; i<3; i++){
            for(int j=i;j<3;j++){

                float sum = 0.0;

                for(auto iter = first;iter!=last; ++iter){
                    sum += ((*iter)[j]-mean[j])*((*iter)[i]-mean[i]);
                }

                float covariance = sum / float(n-1);

                covarianceMatrix[i][j] = covariance;
                covarianceMatrix[j][i] = covariance;
            }
        }

        //2. Find the eigenvectors using GSL
        auto dVarianceMatrix = glm::dmat3(covarianceMatrix);
        gsl_matrix_view m = gsl_matrix_view_array(glm::value_ptr(dVarianceMatrix), 3, 3);

        gsl_vector *eval = gsl_vector_alloc (3);
        gsl_matrix *evec = gsl_matrix_alloc (3, 3);

        gsl_eigen_symmv_workspace * w = gsl_eigen_symmv_alloc (3);
        gsl_eigen_symmv (&m.matrix, eval, evec, w);
        gsl_eigen_symmv_free (w);
        gsl_vector_free(eval);
        auto mat = glm::mat3(glm::make_mat3(evec->data));
        gsl_matrix_free(evec);
        mat = glm::transpose(mat);

        // 3. These eigenvectors are the axis of the bounding box, use them to calculate the OBBs transformation
        auto swap = [&](glm::vec3& a, glm::vec3& b){
            auto tmp = a;
            a = b;
            b = tmp;
        };
        if(mat[1].x < mat[2].x){
            swap(mat[1], mat[2]);
        }
        if(mat[0].x < mat[1].x){
            swap(mat[0], mat[1]);
        }
        if(mat[1].y < mat[2].y){
            swap(mat[1], mat[2]);
        }

        bool rightHanded = glm::dot(mat[2], glm::cross(mat[0], mat[1])) > 0.0;
        if(!rightHanded){
            mat[2] *= -1;
        }
        Quaternion rotation(mat);
//        auto transformation = Transformation::fromRotationMatrix(mat);

        // 4. Calculate the AABB bounds in this transformed space
        auto minimum = rotation.inverseRotateVertex(*first);
        auto maximum = minimum;
        auto second = first; second++;
        for(auto iter = second;iter!=last; ++iter){
            minimum = glm::min(minimum, rotation.inverseRotateVertex(*iter));
            maximum = glm::max(maximum, rotation.inverseRotateVertex(*iter));
        }

        return {AABB(minimum, maximum), rotation};
    }
};
#endif //MESHCORE_OBBFACTORY_H
