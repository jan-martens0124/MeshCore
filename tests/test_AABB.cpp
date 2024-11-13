//
// Created by Jonas on 28/11/2023.
//

#include <gtest/gtest.h>
#include "core/AABB.h"

TEST(AABBTest, ConstructorDefault) {
    AABB aabb;
    EXPECT_EQ(aabb.getMinimum(), Vertex());
    EXPECT_EQ(aabb.getMaximum(), Vertex());
}

TEST(AABBTest, ConstructorWithValues) {
    Vertex min(1.0f, 2.0f, 3.0f);
    Vertex max(4.0f, 5.0f, 6.0f);
    AABB aabb(min, max);
    EXPECT_EQ(aabb.getMinimum(), min);
    EXPECT_EQ(aabb.getMaximum(), max);
}

TEST(AABBTest, GetCenter) {
    Vertex min(1.0f, 2.0f, 3.0f);
    Vertex max(4.0f, 5.0f, 6.0f);
    AABB aabb(min, max);
    Vertex center = (min + max) / 2.0f;
    EXPECT_EQ(aabb.getCenter(), center);
}

TEST(AABBTest, GetHalf) {
    Vertex min(1.0f, 2.0f, 3.0f);
    Vertex max(4.0f, 5.0f, 6.0f);
    AABB aabb(min, max);
    Vertex half = (max - min) / 2.0f;
    EXPECT_EQ(aabb.getHalf(), half);
}

TEST(AABBTest, GetSurfaceArea) {
    Vertex min(1.0f, 2.0f, 3.0f);
    Vertex max(4.0f, 5.0f, 6.0f);
    AABB aabb(min, max);
    float expectedSurfaceArea = 2 * ((4 - 1) * (5 - 2) + (4 - 1) * (6 - 3) + (5 - 2) * (6 - 3));
    EXPECT_EQ(aabb.getSurfaceArea(), expectedSurfaceArea);
}

TEST(AABBTest, GetVolume) {
    Vertex min(1.0f, 2.0f, 3.0f);
    Vertex max(4.0f, 5.0f, 6.0f);
    AABB aabb(min, max);
    float expectedVolume = (4 - 1) * (5 - 2) * (6 - 3);
    EXPECT_EQ(aabb.getVolume(), expectedVolume);
}

TEST(AABBTest, ContainsPoint) {
    AABB aabb(Vertex(1.0f, 2.0f, 3.0f), Vertex(4.0f, 5.0f, 6.0f));
    EXPECT_TRUE(aabb.containsPoint(Vertex(2.0f, 3.0f, 4.0f)));
    EXPECT_FALSE(aabb.containsPoint(Vertex(0.0f, 3.0f, 4.0f)));
}

TEST(AABBTest, ContainsPoint2) {
    AABB aabb(Vertex(-4.0f, -5.0f, -6.0f), Vertex(1.0f, 2.0f, 3.0f));
    EXPECT_TRUE(aabb.containsPoint(Vertex(0.0f, 0.0f, 0.0f)));
    EXPECT_FALSE(aabb.containsPoint(Vertex(2.0f, 3.0f, 4.0f)));
}

TEST(AABBTest, ContainsAABB) {
    AABB aabb1(Vertex(1.0f, 2.0f, 3.0f), Vertex(4.0f, 5.0f, 6.0f));
    AABB aabb2(Vertex(2.0f, 3.0f, 4.0f), Vertex(3.0f, 4.0f, 5.0f));
    EXPECT_TRUE(aabb1.containsAABB(aabb2));
    EXPECT_FALSE(aabb2.containsAABB(aabb1));
}

TEST(AABBTest, GetClosestPoint) {
    AABB aabb(Vertex(1.0f, 2.0f, 3.0f), Vertex(4.0f, 5.0f, 6.0f));
    Vertex closestPoint = aabb.getClosestPoint(Vertex(0.0f, 2.5f, 5.0f));
    EXPECT_EQ(closestPoint, Vertex(1.0f, 2.5f, 5.0f));
}

TEST(AABBTest, GetDistanceSquaredTo) {
    AABB aabb(Vertex(1.0f, 2.0f, 3.0f), Vertex(4.0f, 5.0f, 6.0f));
    float distanceSquared = aabb.getDistanceSquaredTo(Vertex(0.0f, 2.5f, 5.0f));
    EXPECT_EQ(distanceSquared, 1.0f);
}

TEST(AABBTest, Equality) {
    AABB aabb1(Vertex(1.0f, 2.0f, 3.0f), Vertex(4.0f, 5.0f, 6.0f));
    AABB aabb2(Vertex(1.0f, 2.0f, 3.0f), Vertex(4.0f, 5.0f, 6.0f));
    AABB aabb3(Vertex(0.0f, 0.0f, 0.0f), Vertex(1.0f, 1.0f, 1.0f));
    EXPECT_EQ(aabb1, aabb2);
    EXPECT_NE(aabb1, aabb3);
}

TEST(AABBTest, Translation) {
    AABB aabb(Vertex(1.0f, 2.0f, 3.0f), Vertex(4.0f, 5.0f, 6.0f));
    AABB translatedAABB = aabb.getTranslated(glm::vec3(1.0f, 2.0f, 3.0f));
    AABB expectedAABB(Vertex(2.0f, 4.0f, 6.0f), Vertex(5.0f, 7.0f, 9.0f));
    EXPECT_EQ(translatedAABB, expectedAABB);
}

TEST(AABBTest, OperatorEqual) {
    AABB aabb1(Vertex(1.0f, 2.0f, 3.0f), Vertex(4.0f, 5.0f, 6.0f));
    AABB aabb2(Vertex(1.0f, 2.0f, 3.0f), Vertex(4.0f, 5.0f, 6.0f));
    EXPECT_TRUE(aabb1 == aabb2);
}

TEST(AABBTest, OperatorNotEqual) {
    AABB aabb1(Vertex(1.0f, 2.0f, 3.0f), Vertex(4.0f, 5.0f, 6.0f));
    AABB aabb2(Vertex(1.0f, 2.0f, 3.0f), Vertex(4.0f, 5.0f, 7.0f));
    EXPECT_TRUE(aabb1 != aabb2);
}

TEST(AABBTest, GetDistanceSquaredToInsideAABB) {
    AABB aabb(Vertex(1.0f, 2.0f, 3.0f), Vertex(4.0f, 5.0f, 6.0f));
    float distanceSquared = aabb.getDistanceSquaredTo(Vertex(2.0f, 3.0f, 4.0f));
    EXPECT_EQ(distanceSquared, 0.0f);
}

TEST(AABBTest, GetDistanceSquaredToOutsideAABB) {
    AABB aabb(Vertex(1.0f, 2.0f, 3.0f), Vertex(4.0f, 5.0f, 6.0f));
    float distanceSquared = aabb.getDistanceSquaredTo(Vertex(0.0f, 0.0f, 0.0f));
    EXPECT_EQ(distanceSquared, 14.0f);
}

TEST(AABBTest, GetDistanceSquaredToOnEdge) {
    AABB aabb(Vertex(1.0f, 2.0f, 3.0f), Vertex(4.0f, 5.0f, 6.0f));
    float distanceSquared = aabb.getDistanceSquaredTo(Vertex(1.0f, 2.0f, 3.0f));
    EXPECT_EQ(distanceSquared, 0.0f);
}

TEST(AABBTest, GetDistanceSquaredToOnCorner) {
    AABB aabb(Vertex(1.0f, 2.0f, 3.0f), Vertex(4.0f, 5.0f, 6.0f));
    float distanceSquared = aabb.getDistanceSquaredTo(Vertex(1.0f, 2.0f, 3.0f));
    EXPECT_EQ(distanceSquared, 0.0f);
}

TEST(AABBTest, GetDistanceSquaredToInsideAABBWithNegativeValues) {
    AABB aabb(Vertex(-4.0f, -5.0f, -6.0f), Vertex(-1.0f, -2.0f, -3.0f));
    float distanceSquared = aabb.getDistanceSquaredTo(Vertex(-2.0f, -3.0f, -4.0f));
    EXPECT_EQ(distanceSquared, 0.0f);
}

TEST(AABBTest, GetDistanceSquaredToOutsideAABBWithNegativeValues) {
    AABB aabb(Vertex(-4.0f, -5.0f, -6.0f), Vertex(-1.0f, -2.0f, -3.0f));
    float distanceSquared = aabb.getDistanceSquaredTo(Vertex(0.0f, 0.0f, 0.0f));
    EXPECT_EQ(distanceSquared, 14.0f);
}

TEST(AABBTest, GetDistanceSquaredToOnEdgeWithNegativeValues) {
    AABB aabb(Vertex(-4.0f, -5.0f, -6.0f), Vertex(-1.0f, -2.0f, -3.0f));
    float distanceSquared = aabb.getDistanceSquaredTo(Vertex(-1.0f, -2.0f, -3.0f));
    EXPECT_EQ(distanceSquared, 0.0f);
}

TEST(AABBTest, GetDistanceSquaredToOnCornerWithNegativeValues) {
    AABB aabb(Vertex(-4.0f, -5.0f, -6.0f), Vertex(-1.0f, -2.0f, -3.0f));
    float distanceSquared = aabb.getDistanceSquaredTo(Vertex(-1.0f, -2.0f, -3.0f));
    EXPECT_EQ(distanceSquared, 0.0f);
}

TEST(AABBTest, GetDistanceSquaredToInsideAABBWithNegativeAndPositiveValues) {
    AABB aabb(Vertex(-4.0f, -5.0f, -6.0f), Vertex(1.0f, 2.0f, 3.0f));
    float distanceSquared = aabb.getDistanceSquaredTo(Vertex(0.0f, 0.0f, 0.0f));
    EXPECT_EQ(distanceSquared, 0.0f);
}

TEST(AABBTest, GetDistanceSquaredToOutsideAABBWithNegativeAndPositiveValues) {
    AABB aabb(Vertex(-4.0f, -5.0f, -6.0f), Vertex(1.0f, 2.0f, 3.0f));
    float distanceSquared = aabb.getDistanceSquaredTo(Vertex(2.0f, 3.0f, 5.0f));
    EXPECT_EQ(distanceSquared, 6.0f);
}

TEST(AABBTest, GetDistanceSquaredToOnEdgeWithNegativeAndPositiveValues) {
    AABB aabb(Vertex(-4.0f, -5.0f, -6.0f), Vertex(1.0f, 2.0f, 3.0f));
    float distanceSquared = aabb.getDistanceSquaredTo(Vertex(1.0f, 2.0f, 3.0f));
    EXPECT_EQ(distanceSquared, 0.0f);
}

TEST(AABBTest, GetDistanceSquaredToOnCornerWithNegativeAndPositiveValues) {
    AABB aabb(Vertex(-4.0f, -5.0f, -6.0f), Vertex(1.0f, 2.0f, 3.0f));
    float distanceSquared = aabb.getDistanceSquaredTo(Vertex(1.0f, 2.0f, 3.0f));
    EXPECT_EQ(distanceSquared, 0.0f);
}

TEST(AABBTest, GetDistanceSquaredToInsideAABBWithNegativeAndPositiveValues2) {
    AABB aabb(Vertex(-4.0f, -5.0f, -6.0f), Vertex(1.0f, 2.0f, 3.0f));
    float distanceSquared = aabb.getDistanceSquaredTo(Vertex(-2.0f, -3.0f, -4.0f));
    EXPECT_EQ(distanceSquared, 0.0f);
}

TEST(AABBTest, GetDistanceSquaredToOutsideAABBWithNegativeAndPositiveValues2) {
    AABB aabb(Vertex(-4.0f, -5.0f, -6.0f), Vertex(1.0f, 2.0f, 3.0f));
    float distanceSquared = aabb.getDistanceSquaredTo(Vertex(0.0f, 0.0f, 0.0f));
    EXPECT_EQ(distanceSquared, 0.0f);
}

TEST(AABBTest, GetDistanceSquaredToOnEdgeWithNegativeAndPositiveValues2) {
    AABB aabb(Vertex(-4.0f, -5.0f, -6.0f), Vertex(1.0f, 2.0f, 3.0f));
    float distanceSquared = aabb.getDistanceSquaredTo(Vertex(-1.0f, -2.0f, -3.0f));
    EXPECT_EQ(distanceSquared, 0.0f);
}

TEST(AABBTest, GetDistanceSquaredToOnCornerWithNegativeAndPositiveValues2) {
    AABB aabb(Vertex(-4.0f, -5.0f, -6.0f), Vertex(1.0f, 2.0f, 3.0f));
    float distanceSquared = aabb.getDistanceSquaredTo(Vertex(-1.0f, -2.0f, -3.0f));
    EXPECT_EQ(distanceSquared, 0.0f);
}