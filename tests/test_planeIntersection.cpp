//
// Created by Jonas Tollenaere on 20/01/2025.
//

#include <gtest/gtest.h>
#include "geometric/Intersection.h"

TEST(PlaneIntersection, PlanePlaneIntersection) {
    Plane planeA(glm::vec3(1, 0, 0), glm::vec3(0, 0, 0));
    Plane planeB(glm::vec3(0, 1, 0), glm::vec3(0, 0, 0));
    Plane planeC(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0));
    Plane planeD(glm::vec3(1, 0, 0), glm::vec3(1, 0, 0));

    auto lineAB = Intersection::intersect(planeA, planeB);
    ASSERT_TRUE(lineAB.has_value());
    ASSERT_TRUE(glm::all(glm::epsilonEqual(glm::abs(lineAB.value().direction), glm::vec3(0, 0, 1), 1e-6f)));

    auto lineAC = Intersection::intersect(planeA, planeC);
    ASSERT_TRUE(lineAC.has_value());
    ASSERT_TRUE(glm::all(glm::epsilonEqual(glm::abs(lineAC.value().direction), glm::vec3(0, 1, 0), 1e-6f)));

    auto lineAD = Intersection::intersect(planeA, planeD);
    ASSERT_FALSE(lineAD.has_value());
}

TEST(PlaneIntersection, PlaneLineIntersection) {
    Plane planeA(glm::vec3(1, 0, 0), glm::vec3(0, 0, 0));
    Plane planeB(glm::vec3(0, 1, 0), glm::vec3(0, 0, 0));
    Plane planeC(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0));
    Plane planeD(glm::vec3(1, 0, 0), glm::vec3(1, 0, 0));

    Line lineA(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0));
    Line lineB(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    Line lineC(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
    Line lineD(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0));

    auto pointAA = Intersection::intersect(planeA, lineA);
    ASSERT_TRUE(pointAA.has_value());
    ASSERT_TRUE(glm::all(glm::epsilonEqual(pointAA.value(), glm::vec3(0, 0, 0), 1e-6f)));

    auto pointBB = Intersection::intersect(planeB, lineB);
    ASSERT_TRUE(pointBB.has_value());
    ASSERT_TRUE(glm::all(glm::epsilonEqual(pointBB.value(), glm::vec3(0, 0, 0), 1e-6f)));

    auto pointCC = Intersection::intersect(planeC, lineC);
    ASSERT_TRUE(pointCC.has_value());
    ASSERT_TRUE(glm::all(glm::epsilonEqual(pointCC.value(), glm::vec3(0, 0, 0), 1e-6f)));

    auto pointDD = Intersection::intersect(planeD, lineD);
    ASSERT_TRUE(pointDD.has_value());
    ASSERT_TRUE(glm::all(glm::epsilonEqual(pointDD.value(), glm::vec3(1, 0, 0), 1e-6f)));

    auto pointBD = Intersection::intersect(planeB, lineD);
    ASSERT_FALSE(pointBD.has_value());
}

TEST(PlaneIntersection, PlanePlanePlaneIntersection) {

    Plane planeA(glm::vec3(1, 0, 0), glm::vec3(0, 0, 0));
    Plane planeB(glm::vec3(0, 1, 0), glm::vec3(0, 0, 0));
    Plane planeC(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0));
    Plane planeD(glm::vec3(1, 0, 0), glm::vec3(1, 0, 0));

    auto pointABC = Intersection::intersect(planeA, planeB, planeC);
    auto pointACB = Intersection::intersect(planeA, planeC, planeB);
    auto pointCBA = Intersection::intersect(planeC, planeB, planeA);
    ASSERT_TRUE(pointABC.has_value());
    ASSERT_TRUE(pointACB.has_value());
    ASSERT_TRUE(pointCBA.has_value());
    ASSERT_TRUE(glm::all(glm::epsilonEqual(pointABC.value(), glm::vec3(0, 0, 0), 1e-6f)));
    ASSERT_TRUE(glm::all(glm::epsilonEqual(pointACB.value(), glm::vec3(0, 0, 0), 1e-6f)));
    ASSERT_TRUE(glm::all(glm::epsilonEqual(pointCBA.value(), glm::vec3(0, 0, 0), 1e-6f)));

    auto pointABD = Intersection::intersect(planeA, planeB, planeD);
    auto pointDAB = Intersection::intersect(planeD, planeA, planeB);
    auto pointBDA = Intersection::intersect(planeB, planeD, planeA);
    ASSERT_FALSE(pointABD.has_value());
    ASSERT_FALSE(pointDAB.has_value());
    ASSERT_FALSE(pointBDA.has_value());

    auto pointACD = Intersection::intersect(planeA, planeC, planeD);
    auto pointCAD = Intersection::intersect(planeC, planeA, planeD);
    auto pointDAC = Intersection::intersect(planeD, planeA, planeC);
    ASSERT_FALSE(pointACD.has_value());
    ASSERT_FALSE(pointCAD.has_value());
    ASSERT_FALSE(pointDAC.has_value());

    auto BCD = Intersection::intersect(planeB, planeC, planeD);
    auto CDB = Intersection::intersect(planeC, planeD, planeB);
    auto DBC = Intersection::intersect(planeD, planeB, planeC);
    ASSERT_TRUE(BCD.has_value());
    ASSERT_TRUE(CDB.has_value());
    ASSERT_TRUE(DBC.has_value());
    ASSERT_TRUE(glm::all(glm::epsilonEqual(BCD.value(), glm::vec3(1, 0, 0), 1e-6f)));
    ASSERT_TRUE(glm::all(glm::epsilonEqual(CDB.value(), glm::vec3(1, 0, 0), 1e-6f)));
    ASSERT_TRUE(glm::all(glm::epsilonEqual(DBC.value(), glm::vec3(1, 0, 0), 1e-6f)));
}
