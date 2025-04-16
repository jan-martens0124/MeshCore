/*************************************************************************\

  Copyright 1999 The University of North Carolina at Chapel Hill.
  All Rights Reserved.

  Permission to use, copy, modify and distribute this software and its
  documentation for educational, research and non-profit purposes, without
  fee, and without a written agreement is hereby granted, provided that the
  above copyright notice and the following three paragraphs appear in all
  copies.

  IN NO EVENT SHALL THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL BE
  LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
  CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE
  USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY
  OF NORTH CAROLINA HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH
  DAMAGES.

  THE UNIVERSITY OF NORTH CAROLINA SPECIFICALLY DISCLAIM ANY
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE
  PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
  NORTH CAROLINA HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT,
  UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

  The authors may be contacted via:

  US Mail:             E. Larsen
                       Department of Computer Science
                       Sitterson Hall, CB #3175
                       University of N. Carolina
                       Chapel Hill, NC 27599-3175

  Phone:               (919)962-1749

  EMail:               geom@cs.unc.edu


\**************************************************************************/

/**************************************************************************\
    Downloaded from http://gamma.cs.unc.edu/SSV/
    Adapted for MeshCore by Jonas Tollenaere
\**************************************************************************/


//--------------------------------------------------------------------------
// File:   TriDist.cpp
// Author: Eric Larsen
// Description:
// contains SegPoints() for finding closest points on a pair of line
// segments and TriDist() for finding closest points on a pair of triangles
//--------------------------------------------------------------------------

#include "meshcore/geometric/Distance.h"

#include <math.h>

//--------------------------------------------------------------------------
// SegPoints() 
//
// Returns closest points between an segment pair.
// Implemented from an algorithm described in
//
// Vladimir J. Lumelsky,
// On fast computation of distance between line segments.
// In Information Processing Letters, no. 21, pages 55-61, 1985.   
//--------------------------------------------------------------------------

void
SegPoints(glm::vec3* VEC, glm::vec3* X, glm::vec3* Y,             // closest points
          const glm::vec3 P, const glm::vec3 A, // seg 1 origin, vector
          const glm::vec3 Q, const glm::vec3 B) // seg 2 origin, vector
{
    auto T = Q - P;
    auto A_dot_A = glm::dot(A,A);
    auto B_dot_B = glm::dot(B,B);
    auto A_dot_B = glm::dot(A,B);
    auto A_dot_T = glm::dot(A,T);
    auto B_dot_T = glm::dot(B,T);


    // t parameterizes ray P,A
    // compute t for the closest point on ray P,A to ray Q,B
    auto denom = A_dot_A*B_dot_B - A_dot_B*A_dot_B;
    auto t = (A_dot_T*B_dot_B - B_dot_T*A_dot_B) / denom;

    // clamp result so t is on the segment P,A
    if ((t < 0) || isnan(t)) t = 0; else if (t > 1) t = 1;


    // u parameterizes ray Q,B
    // find u for point on ray Q,B closest to point at t
    auto u = (t*A_dot_B - B_dot_T) / B_dot_B;

    // if u is on segment Q,B, t and u correspond to
    // closest points, otherwise, clamp u, recompute and
    // clamp t

    if ((u <= 0) || isnan(u)) {
        *Y = Q;

        t = A_dot_T / A_dot_A;

        if ((t <= 0) || isnan(t)) {
            *X = P;
            *VEC = Q - P;
        }
        else if (t >= 1) {
            *X = P + A;
            *VEC = Q - *X;
        }
        else {
            *X = P + (A*t);
            *VEC = glm::cross(A, glm::cross(T, A));
        }
    }
    else if (u >= 1) {

        *Y = Q + B;
        t = (A_dot_B + A_dot_T) / A_dot_A;

        if ((t <= 0) || isnan(t)) {
            *X = P;
            *VEC = *Y - P;
        }
        else if (t >= 1) {
            *X = P + A;
            *VEC = *Y - *X;
        }
        else {
            *X = P + (A*t);
            T = *Y - P;
            *VEC = glm::cross(A, glm::cross(T, A));
        }
    }
    else {

        *Y = Q + (B*u);

        if ((t <= 0) || isnan(t)) {
            *X = P;
            *VEC = glm::cross(B, glm::cross(T, B));
        }
        else if (t >= 1) {
            *X = P + A;
            T = Q - *X;
            *VEC = glm::cross(B, glm::cross(T, B));
        }
        else {
            *X = P + (A*t);
            *VEC = glm::cross(A, B);
            if(glm::dot(*VEC, T) < 0){
                *VEC *= -1;
            }
        }
    }
}

//--------------------------------------------------------------------------
// TriDist() 
//
// Computes the closest points on two triangles, and returns the 
// distance between them.
// 
// S and T are the triangles, stored tri[point][dimension].
//
// If the triangles are disjoint, P and Q give the closest points of 
// S and T respectively. However, if the triangles overlap, P and Q 
// are basically a random pair of points from the triangles, not 
// coincident points on the intersection of the triangles, as might 
// be expected.
//--------------------------------------------------------------------------

float TriDistSqr(glm::vec3* P, glm::vec3* Q, const VertexTriangle& S, const VertexTriangle T)
{

    glm::vec3 VEC;

    // For each edge pair, the vector connecting the closest points
    // of the edges defines a slab (parallel planes at head and tail
    // enclose the slab). If we can show that the off-edge vertex of
    // each triangle is outside of the slab, then the closest points
    // of the edges are the closest points for the triangles.
    // Even if these tests fail, it may be helpful to know the closest
    // points found, and whether the triangles were shown disjoint

    glm::vec3 V;
    glm::vec3 Z;
    glm::vec3 minP, minQ;
    float mindd;
    int shown_disjoint = 0;

    {
        auto delta = S.vertices[0] - T.vertices[0];
        mindd = glm::dot(delta, delta) + 1; // Set first minimum safely high
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {

            // Find closest points on edges i & j, plus the
            // vector (and distance squared) between these points
            SegPoints(&VEC,P,Q,S.vertices[i],S.edges[i],T.vertices[j],T.edges[j]);

            V = *Q - *P;
            auto dd = glm::dot(V, V);

            // Verify this closest point pair only if the distance
            // squared is less than the minimum found thus far.
            if (dd <= mindd) {
                minP = *P;
                minQ = *Q;
                mindd = dd;

                Z = S.vertices[(i+2)%3] - *P;
                auto a = glm::dot(Z, VEC);

                Z = T.vertices[(j+2)%3] - *Q;
                auto b = glm::dot(Z, VEC);

                if ((a <= 0) && (b >= 0)) return dd;

                auto p = glm::dot(V, VEC);

                if (a < 0) a = 0;
                if (b > 0) b = 0;
                if ((p - a + b) > 0) shown_disjoint = 1;
            }
        }
    }

    // No edge pairs contained the closest points.
    // either:
    // 1. one of the closest points is a vertex, and the
    //    other point is interior to a face.
    // 2. the triangles are overlapping.
    // 3. an edge of one triangle is parallel to the other's face. If
    //    cases 1 and 2 are not true, then the closest points from the 9
    //    edge pairs checks above can be taken as closest points for the
    //    triangles.
    // 4. possibly, the triangles were degenerate.  When the
    //    triangle points are nearly colinear or coincident, one
    //    of above tests might fail even though the edges tested
    //    contain the closest points.

    // First check for case 1
    auto Snl = glm::dot(S.normal,S.normal);      // Compute square of length of normal
    if (Snl > 1e-15) { // If cross product is long enough,

        // Get projection lengths of T points
        glm::vec3 Tp;

        V = S.vertices[0] - T.vertices[0];
        Tp[0] = glm::dot(V,S.normal);

        V = S.vertices[0] - T.vertices[1];
        Tp[1] = glm::dot(V,S.normal);

        V = S.vertices[0] - T.vertices[2];
        Tp[2] = glm::dot(V,S.normal);

        // If Sn is a separating direction,
        // find point with smallest projection

        int point = -1;
        if ((Tp[0] > 0) && (Tp[1] > 0) && (Tp[2] > 0))
        {
            if (Tp[0] < Tp[1]) point = 0; else point = 1;
            if (Tp[2] < Tp[point]) point = 2;
        }
        else if ((Tp[0] < 0) && (Tp[1] < 0) && (Tp[2] < 0))
        {
            if (Tp[0] > Tp[1]) point = 0; else point = 1;
            if (Tp[2] > Tp[point]) point = 2;
        }

        // If Sn is a separating direction,
        if (point >= 0) {

            shown_disjoint = 1;

            // Test whether the point found, when projected onto the
            // other triangle, lies within the face.
            V = T.vertices[point] - S.vertices[0];
            Z = glm::cross(S.normal,S.edges[0]);
            if (glm::dot(V,Z) > 0){
                V  = T.vertices[point] - S.vertices[1];
                Z = glm::cross(S.normal, S.edges[1]);

                if (glm::dot(V,Z) > 0){
                    V  = T.vertices[point] - S.vertices[2];
                    Z = glm::cross(S.normal,S.edges[2]);

                    if (glm::dot(V,Z) > 0) {

                        // T[point] passed the test - it's a closest point for
                        // the T triangle; the other point is on the face of S
                        *P = T.vertices[point] + S.normal * Tp[point]/Snl;
                        *Q = T.vertices[point];
                        {
                            auto delta = *P - *Q;
                            return glm::dot(delta, delta);
                        }
                    }
                }
            }
        }
    }

    auto Tnl = glm::dot(T.normal,T.normal);
    if (Tnl > 1e-15) {
        glm::vec3 Sp;

        V = T.vertices[0] - S.vertices[0];
        Sp[0] = glm::dot(V,T.normal);

        V = T.vertices[0] - S.vertices[1];
        Sp[1] = glm::dot(V,T.normal);

        V = T.vertices[0] - S.vertices[2];
        Sp[2] = glm::dot(V,T.normal);

        int point = -1;
        if ((Sp[0] > 0) && (Sp[1] > 0) && (Sp[2] > 0))
        {
            if (Sp[0] < Sp[1]) point = 0; else point = 1;
            if (Sp[2] < Sp[point]) point = 2;
        }
        else if ((Sp[0] < 0) && (Sp[1] < 0) && (Sp[2] < 0))
        {
            if (Sp[0] > Sp[1]) point = 0; else point = 1;
            if (Sp[2] > Sp[point]) point = 2;
        }

        if (point >= 0) {
            shown_disjoint = 1;
            V = S.vertices[point] - T.vertices[0];
            Z = glm::cross(T.normal,T.edges[0]);

            if (glm::dot(V,Z) > 0) {
                V = S.vertices[point] - T.vertices[1];
                Z = glm::cross(T.normal,T.edges[1]);

                if (glm::dot(V,Z) > 0) {
                    V = S.vertices[point] - T.vertices[2];
                    Z = glm::cross(T.normal,T.edges[2]);
                    if (glm::dot(V,Z) > 0) {
                        *P = S.vertices[point];
                        *Q = S.vertices[point] + T.normal * Sp[point]/Tnl;
                        {
                            auto delta = *P - *Q;
                            return glm::dot(delta, delta);
                        }
                    }
                }
            }
        }
    }

    // Case 1 can't be shown.
    // If one of these tests showed the triangles disjoint,
    // we assume case 3 or 4, otherwise we conclude case 2,
    // that the triangles overlap.
    if (shown_disjoint) {
        *P = minP;
        *Q = minQ;
        return mindd;
    }
    else return 0;
}

namespace Distance{
    float distance(const VertexTriangle& triangleA, const VertexTriangle& triangleB){
        glm::vec3 P, Q;
        return glm::sqrt(TriDistSqr(&P, &Q, triangleA, triangleB));
    }

    float distanceSquared(const VertexTriangle& triangleA, const VertexTriangle& triangleB, glm::vec3* triangleAClosestPoint, glm::vec3* triangleBClosestPoint){
        auto result = TriDistSqr(triangleAClosestPoint, triangleBClosestPoint, triangleA, triangleB);

#if !NDEBUG
        for (auto vertexA : triangleA.vertices){
            for (auto vertexB : triangleB.vertices){
                auto delta = vertexA - vertexB;
                auto distance = glm::dot(delta, delta);
                assert(distance/result >= 1 - 1e-5f);
            }
        }
#endif
        return result;
    }
}