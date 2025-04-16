//
// Created by Jonas on 28/11/2023.
//

#include <gtest/gtest.h>

#include "meshcore/core/Transformation.h"

#define EXPECT_ANGLE_EQ(a, b) EXPECT_NEAR(glm::mod(a, 2 * glm::pi<float>()), glm::mod(b, 2 * glm::pi<float>()), 1e-5f)

TEST(TransformationTest, GetterSetter){
    Transformation transformation;
    transformation.setPosition(Vertex(1, 2, 3));
    transformation.setScale(4);
    transformation.setRotation(Quaternion(5, 6, 7));

    EXPECT_EQ(transformation.getPosition(), Vertex(1, 2, 3));
    EXPECT_EQ(transformation.getScale(), 4);
    EXPECT_ANGLE_EQ(transformation.getRotation().getYaw(), 5.0f);
    EXPECT_ANGLE_EQ(transformation.getRotation().getPitch(), 6.0f);
    EXPECT_ANGLE_EQ(transformation.getRotation().getRoll(), 7.0f);
}

TEST(TransformationTest, Delta){
    Transformation transformation;
    transformation.deltaPosition(Vertex(1, 2, 3));
    transformation.deltaScale(4);
    transformation.factorRotation(Quaternion(5, 6, 7));

    EXPECT_EQ(transformation.getPosition(), Vertex(1, 2, 3));
    EXPECT_EQ(transformation.getScale(), 1+4);
    EXPECT_ANGLE_EQ(transformation.getRotation().getYaw(), 5.0f);
    EXPECT_ANGLE_EQ(transformation.getRotation().getPitch(), 6.0f);
    EXPECT_ANGLE_EQ(transformation.getRotation().getRoll(), 7.0f);
}

TEST(TransformationTest, Factor){
    Transformation transformation;
    transformation.setScale(3);
    transformation.factorScale(4);
    transformation.deltaScale(7);

    EXPECT_EQ(transformation.getScale(), 3*4+7);
}

TEST(TransformationTest, TransformVertex){
    Transformation transformation;
    transformation.setPosition(Vertex(1, 2, 3));
    transformation.setScale(4);

    EXPECT_EQ(transformation.transformVertex(Vertex(1, 2, 3)), 4.0f*Vertex(1, 2, 3) + Vertex(1, 2, 3));
    EXPECT_EQ(transformation.transformVertex(Vertex(0, 0, 0)), 4.0f*Vertex(0, 0, 0) + Vertex(1, 2, 3));
    EXPECT_EQ(transformation.transformVertex(Vertex(1, 0, 0)), 4.0f*Vertex(1, 0, 0) + Vertex(1, 2, 3));
    EXPECT_EQ(transformation.transformVertex(Vertex(0, 1, 0)), 4.0f*Vertex(0, 1, 0) + Vertex(1, 2, 3));
    EXPECT_EQ(transformation.transformVertex(Vertex(0, 0, 1)), 4.0f*Vertex(0, 0, 1) + Vertex(1, 2, 3));
    EXPECT_EQ(transformation.transformVertex(Vertex(1, 1, 1)), 4.0f*Vertex(1, 1, 1) + Vertex(1, 2, 3));
    EXPECT_EQ(transformation.transformVertex(Vertex(2, 3, 4)), 4.0f*Vertex(2, 3, 4) + Vertex(1, 2, 3));
    EXPECT_EQ(transformation.transformVertex(Vertex(1, 2, 3)), 4.0f*Vertex(1, 2, 3) + Vertex(1, 2, 3));
}

TEST(TransformationTest, TransformVertex2){
    Transformation transformation;
    transformation.setPosition(Vertex(1, 2, 3));
    transformation.setScale(4);
    transformation.setRotation(Quaternion(5, 6, 7));

    // Rotation matrix to quaternion has limited accuracy
    EXPECT_TRUE(glm::all(glm::epsilonEqual(transformation.transformVertex(Vertex(1, 2, 3)), 4.0f*Quaternion(transformation.getRotation().computeMatrix()).rotateVertex(Vertex(1, 2, 3)) + Vertex(1, 2, 3), 1e-5f)));
    EXPECT_TRUE(glm::all(glm::epsilonEqual(transformation.transformVertex(Vertex(0, 0, 0)), 4.0f*Quaternion(transformation.getRotation().computeMatrix()).rotateVertex(Vertex(0, 0, 0)) + Vertex(1, 2, 3), 1e-5f)));
    EXPECT_TRUE(glm::all(glm::epsilonEqual(transformation.transformVertex(Vertex(1, 0, 0)), 4.0f*Quaternion(transformation.getRotation().computeMatrix()).rotateVertex(Vertex(1, 0, 0)) + Vertex(1, 2, 3), 1e-5f)));
    EXPECT_TRUE(glm::all(glm::epsilonEqual(transformation.transformVertex(Vertex(0, 1, 0)), 4.0f*Quaternion(transformation.getRotation().computeMatrix()).rotateVertex(Vertex(0, 1, 0)) + Vertex(1, 2, 3), 1e-5f)));
    EXPECT_TRUE(glm::all(glm::epsilonEqual(transformation.transformVertex(Vertex(0, 0, 1)), 4.0f*Quaternion(transformation.getRotation().computeMatrix()).rotateVertex(Vertex(0, 0, 1)) + Vertex(1, 2, 3), 1e-5f)));
    EXPECT_TRUE(glm::all(glm::epsilonEqual(transformation.transformVertex(Vertex(1, 1, 1)), 4.0f*Quaternion(transformation.getRotation().computeMatrix()).rotateVertex(Vertex(1, 1, 1)) + Vertex(1, 2, 3), 1e-5f)));
    EXPECT_TRUE(glm::all(glm::epsilonEqual(transformation.transformVertex(Vertex(2, 3, 4)), 4.0f*Quaternion(transformation.getRotation().computeMatrix()).rotateVertex(Vertex(2, 3, 4)) + Vertex(1, 2, 3), 1e-5f)));
    EXPECT_TRUE(glm::all(glm::epsilonEqual(transformation.transformVertex(Vertex(1, 2, 3)), 4.0f*Quaternion(transformation.getRotation().computeMatrix()).rotateVertex(Vertex(1, 2, 3)) + Vertex(1, 2, 3), 1e-5f)));
}

TEST(TransformationTest, Inverse){
    Transformation transformation;
    transformation.setPosition(Vertex(1, 2, 3));
    transformation.setScale(4);
    transformation.setRotation(Quaternion(5, 6, 7));

    auto inverse = transformation.getInverse();
    EXPECT_TRUE(glm::all(glm::epsilonEqual(inverse.transformVertex(transformation.transformVertex(Vertex(1, 2, 3))), Vertex(1, 2, 3), 1e-5f)));
    EXPECT_TRUE(glm::all(glm::epsilonEqual(inverse.transformVertex(transformation.transformVertex(Vertex(0, 0, 0))), Vertex(0, 0, 0), 1e-5f)));
    EXPECT_TRUE(glm::all(glm::epsilonEqual(inverse.transformVertex(transformation.transformVertex(Vertex(1, 0, 0))), Vertex(1, 0, 0), 1e-5f)));
    EXPECT_TRUE(glm::all(glm::epsilonEqual(inverse.transformVertex(transformation.transformVertex(Vertex(0, 1, 0))), Vertex(0, 1, 0), 1e-5f)));
    EXPECT_TRUE(glm::all(glm::epsilonEqual(inverse.transformVertex(transformation.transformVertex(Vertex(0, 0, 1))), Vertex(0, 0, 1), 1e-5f)));
    EXPECT_TRUE(glm::all(glm::epsilonEqual(inverse.transformVertex(transformation.transformVertex(Vertex(1, 1, 1))), Vertex(1, 1, 1), 1e-5f)));
    EXPECT_TRUE(glm::all(glm::epsilonEqual(inverse.transformVertex(transformation.transformVertex(Vertex(2, 3, 4))), Vertex(2, 3, 4), 1e-5f)));
    EXPECT_TRUE(glm::all(glm::epsilonEqual(inverse.transformVertex(transformation.transformVertex(Vertex(1, 2, 3))), Vertex(1, 2, 3), 1e-5f)));

}