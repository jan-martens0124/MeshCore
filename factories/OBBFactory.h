//
// Created by Jonas on 17/03/2022.
//

#ifndef OPTIXMESHCORE_OBBFACTORY_H
#define OPTIXMESHCORE_OBBFACTORY_H

#include "../utility/hash.h"
#include "../core/ModelSpaceMesh.h"
#include "../utility/hash.h"
#include "../core/OBB.h"
#include <gsl/gsl_eigen.h>
#include <unordered_set>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

class OBBFactory{

public:
    static OBB createOBB(const std::vector<Vertex>& vertices){
        return createOBB(vertices.begin(), vertices.end(), vertices.size());
    }

    static OBB createOBB(const std::vector<VertexTriangle>& triangles){
        std::unordered_set<Vertex> vertices;
        for (const auto &item : triangles){ // Is this set worth it? We might as well just count every vertex twice instead of hashing it (PCA works in O(n) anyway)
            vertices.insert(item.vertex0);
            vertices.insert(item.vertex1);
            vertices.insert(item.vertex2);
        }
        return createOBB(vertices.begin(), vertices.end(), vertices.size());
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
        else return {modelSpaceMesh->getBounds(), Quaternion(glm::mat3(Transformation().getRotationMatrix()))};
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
#endif //OPTIXMESHCORE_OBBFACTORY_H
