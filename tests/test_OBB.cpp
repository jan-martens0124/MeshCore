//
// Created by Jonas on 28/11/2023.
//

#include <gtest/gtest.h>

#include "meshcore/core/OBB.h"
#include "meshcore/core/Quaternion.h"

TEST(OBBTest, SurfaceArea) {
    // Create an OBB with known parameters
    OBB obb(AABB(Vertex(0, 0, 0), Vertex(1, 1, 1)), Quaternion(glm::fquat(1, 0, 0, 0)));

    // Calculate the expected surface area
    float expectedSurfaceArea = 6.0f; // For a unit cube

    // Check if the calculated surface area matches the expected value
    ASSERT_FLOAT_EQ(obb.getSurfaceArea(), expectedSurfaceArea);
}

TEST(OBBTest, SurfaceAreaRotated) {
    // Create an OBB with known parameters
    OBB obb(AABB(Vertex(0, 0, 0), Vertex(1, 1, 1)), Quaternion(glm::fquat(0.70710678118, 0, 0, 0.70710678118)));

    // Calculate the expected surface area
    float expectedSurfaceArea = 6.0f; // For a unit cube

    // Check if the calculated surface area matches the expected value
    ASSERT_FLOAT_EQ(obb.getSurfaceArea(), expectedSurfaceArea);
}

TEST(OBBTest, SurfaceAreaAbitraryRotated) {
    // Create an OBB with known parameters
    OBB obb(AABB(Vertex(0, 0, 0), Vertex(1, 1, 1)), Quaternion(glm::fquat(0.5, 0.5, 0.5, 0.5)));

    // Calculate the expected surface area
    float expectedSurfaceArea = 6.0f; // For a unit cube

    // Check if the calculated surface area matches the expected value
    ASSERT_FLOAT_EQ(obb.getSurfaceArea(), expectedSurfaceArea);
}

TEST(OBBTest, Volume) {
    // Create an OBB with known parameters
    OBB obb(AABB(Vertex(0, 0, 0), Vertex(1, 1, 1)), Quaternion(glm::fquat(1, 0, 0, 0)));

    // Calculate the expected volume
    float expectedVolume = 1.0f; // For a unit cube

    // Check if the calculated volume matches the expected value
    ASSERT_FLOAT_EQ(obb.getVolume(), expectedVolume);
}

TEST(OBBTest, VolumeRotated) {
    // Create an OBB with known parameters
    OBB obb(AABB(Vertex(0, 0, 0), Vertex(1, 1, 1)), Quaternion(glm::fquat(0.70710678118, 0, 0, 0.70710678118)));

    // Calculate the expected volume
    float expectedVolume = 1.0f; // For a unit cube

    // Check if the calculated volume matches the expected value
    ASSERT_FLOAT_EQ(obb.getVolume(), expectedVolume);
}

TEST(OBBTest, VolumeAbitraryRotated) {
    // Create an OBB with known parameters
    OBB obb(AABB(Vertex(0, 0, 0), Vertex(1, 1, 1)), Quaternion(glm::fquat(0.5, 0.5, 0.5, 0.5)));

    // Calculate the expected volume
    float expectedVolume = 1.0f; // For a unit cube

    // Check if the calculated volume matches the expected value
    ASSERT_FLOAT_EQ(obb.getVolume(), expectedVolume);
}

TEST(OBBTest, Center) {
    // Create an OBB with known parameters
    OBB obb(AABB(Vertex(1, 2, 3), Vertex(4, 5, 6)), Quaternion(glm::fquat(1, 0, 0, 0)));

    // Calculate the expected center after rotation
    Vertex expectedCenter = Vertex(2.5, 3.5, 4.5);

    // Check if the calculated center matches the expected value
    ASSERT_TRUE(glm::all(glm::epsilonEqual(obb.getCenter(), expectedCenter, glm::epsilon<float>())));
}

TEST(OBBTest, ContainsPoint) {
    // Create an OBB with known parameters
    OBB obb(AABB(Vertex(1, 2, 3), Vertex(4, 5, 6)), Quaternion(glm::fquat(1, 0, 0, 0)));

    // Check if the OBB contains a point inside
    ASSERT_TRUE(obb.containsPoint(Vertex(2, 3, 4)));

    // Check if the OBB contains a point on the surface
    ASSERT_TRUE(obb.containsPoint(Vertex(1, 2, 3)));
    ASSERT_TRUE(obb.containsPoint(Vertex(4, 5, 6)));

    // Check if the OBB contains a point outside
    ASSERT_FALSE(obb.containsPoint(Vertex(0, 0, 0)));
    ASSERT_FALSE(obb.containsPoint(Vertex(5, 5, 5)));
}