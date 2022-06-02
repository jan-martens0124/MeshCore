/********************************************************/

/* AABB-triangle overlap test code                      */

/* by Tomas Akenine-M�ller                              */

/* Function: int triBoxOverlap(float boxcenter[3],      */

/*          float boxhalfsize[3],float triverts[3][3]); */

/* History:                                             */

/*   2001-03-05: released the code in its first version */

/*   2001-06-18: changed the order of the tests, faster */

/*                                                      */

/* Acknowledgement: Many thanks to Pierre Terdiman for  */

/* suggestions and discussions on how to optimize code. */

/* Thanks to David Hunt for finding a ">="-bug!         */

/********************************************************/

#ifdef __INTEL_COMPILER
#define GLM_FORCE_PURE
#endif
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "acceleration/AABBOctree.h"


#define CROSS(dest,v1,v2) \
dest[0] = v1[1] * v2[2] - v1[2] * v2[1]; \
dest[1] = v1[2] * v2[0] - v1[0] * v2[2]; \
dest[2] = v1[0] * v2[1] - v1[1] * v2[0];


#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])



#define SUB(dest,v1,v2) \
dest[0] = v1[0] - v2[0]; \
dest[1] = v1[1] - v2[1]; \
dest[2] = v1[2] - v2[2];


int planeBoxOverlap(const glm::dvec3& normal, const glm::dvec3& vert, const glm::dvec3& maxbox)	// -NJMP-
{
	int q;
	double vmin[3], vmax[3], v;
	for (q = 0; q <= 2; q++)
	{
		v = vert[q];					// -NJMP-
		if (normal[q] > 0.0)
		{
			vmin[q] = -maxbox[q] - v;	// -NJMP-
			vmax[q] = maxbox[q] - v;	// -NJMP-
		}
		else
		{
			vmin[q] = maxbox[q] - v;	// -NJMP-
			vmax[q] = -maxbox[q] - v;	// -NJMP-
		}
	}

	if (DOT(normal, vmin) > 0.0) return 0;	// -NJMP-
	if (DOT(normal, vmax) >= 0.0) return 1;	// -NJMP-

	return 0;
}





/*======================== X-tests ========================*/

#define AXISTEST_X01(a, b, fa, fb)			   \
p0 = a * v0[1] - b * v0[2];			       	   \
p2 = a * v2[1] - b * v2[2];			       	   \
if (p0 < p2) { min = p0; max = p2; } else { min = p2; max = p0; } \
rad = fa * boxhalfsize[1] + fb * boxhalfsize[2];   \
if (min > rad || max < -rad) return 0;

#define AXISTEST_X2(a, b, fa, fb)			   \
p0 = a * v0[1] - b * v0[2];			           \
p1 = a * v1[1] - b * v1[2];			       	   \
if (p0 < p1) { min = p0; max = p1; } else { min = p1; max = p0; } \
rad = fa * boxhalfsize[1] + fb * boxhalfsize[2];   \
if (min > rad || max < -rad) return 0;


/*======================== Y-tests ========================*/

#define AXISTEST_Y02(a, b, fa, fb)			   \
p0 = -a * v0[0] + b * v0[2];		      	   \
p2 = -a * v2[0] + b * v2[2];	       	       	   \
if (p0 < p2) { min = p0; max = p2; } else { min = p2; max = p0; } \
rad = fa * boxhalfsize[0] + fb * boxhalfsize[2];   \
if (min > rad || max < -rad) return 0;



#define AXISTEST_Y1(a, b, fa, fb)			   \
p0 = -a * v0[0] + b * v0[2];		      	   \
p1 = -a * v1[0] + b * v1[2];	     	       	   \
if (p0 < p1) { min = p0; max = p1; } else { min = p1; max = p0; } \
rad = fa * boxhalfsize[0] + fb * boxhalfsize[2];   \
if (min > rad || max < -rad) return 0;



/*======================== Z-tests ========================*/



#define AXISTEST_Z12(a, b, fa, fb)			   \
p1 = a * v1[0] - b * v1[1];			           \
p2 = a * v2[0] - b * v2[1];			       	   \
if (p2 < p1) { min = p2; max = p1; } else { min = p1; max = p2; } \
rad = fa * boxhalfsize[0] + fb * boxhalfsize[1];   \
if (min > rad || max < -rad) return 0;



#define AXISTEST_Z0(a, b, fa, fb)			   \
p0 = a * v0[0] - b * v0[1];				   \
p1 = a * v1[0] - b * v1[1];			           \
if (p0 < p1) { min = p0; max = p1; } else { min = p1; max = p0; } \
rad = fa * boxhalfsize[0] + fb * boxhalfsize[1];   \
if (min > rad || max < -rad) return 0;

//int triBoxOverlap(double const boxcenter[3], double const boxhalfsize[3], double const vertA[3], double const vertB[3], double const vertC[3], double const e0[3], double const e1[3], double const e2[3], double const normal[3])
namespace Intersection{

    int intersect(const AABB& aabb, const VertexTriangle& vertexTriangle)
    {

        /* Bullet 1: */

        /*  first test overlap in the {x,y,z}-directions */
        /*  find min, max of the triangle each direction, and test for overlap in */
        /*  that direction -- this is equivalent to testing a minimal AABB around */
        /*  the triangle against the AABB */

        /* test in X-direction */

        const glm::vec3& minv = vertexTriangle.bounds.getMinimum();
        const glm::vec3& maxv = vertexTriangle.bounds.getMaximum();

        if (minv.x > aabb.getMaximum().x || maxv.x < aabb.getMinimum().x) return 0;
        if (minv.y > aabb.getMaximum().y || maxv.y < aabb.getMinimum().y) return 0;
        if (minv.z > aabb.getMaximum().z || maxv.z < aabb.getMinimum().z) return 0;


        /*    use separating axis theorem to test overlap between triangle and box */
        /*    need to test for overlap in these directions: */
        /*    1) the {x,y,z}-directions (actually, since we use the AABB of the triangle */
        /*       we do not even need to test these) */
        /*    2) normal of the triangle */
        /*    3) crossproduct(edge from tri, {x,y,z}-directin) */
        /*       this gives 3x3=9 more tests */

        glm::vec3 v0{}, v1{}, v2{};

        //   float axis[3];

        double min, max, p0, p1, p2, rad, fex, fey, fez;		// -NJMP- "d" local variable removed

//	double normal[3]; //, e0[3], e1[3], e2[3];

        glm::vec3 half = aabb.getHalf();
        const float* boxhalfsize = glm::value_ptr(half);

        /* This is the fastest branch on Sun */

        /* move everything so that the boxcenter is in (0,0,0) */

        glm::vec3 center = aabb.getCenter();
        v0 = vertexTriangle.vertex0 - center;
        v1 = vertexTriangle.vertex1 - center;
        v2 = vertexTriangle.vertex2 - center;



        /* compute triangle edges */

//	SUB(e0, v1, v0);      /* tri edge 0 */
//
//	SUB(e1, v2, v1);      /* tri edge 1 */
//
//	SUB(e2, v0, v2);      /* tri edge 2 */

        /* Bullet 3:  */

        /*  test the 9 tests first (this was faster) */

        fex = fabs(vertexTriangle.edge0[0]);

        fey = fabs(vertexTriangle.edge0[1]);

        fez = fabs(vertexTriangle.edge0[2]);

        AXISTEST_X01(vertexTriangle.edge0[2], vertexTriangle.edge0[1], fez, fey);

        AXISTEST_Y02(vertexTriangle.edge0[2], vertexTriangle.edge0[0], fez, fex);

        AXISTEST_Z12(vertexTriangle.edge0[1], vertexTriangle.edge0[0], fey, fex);



        fex = fabs(vertexTriangle.edge1[0]);

        fey = fabs(vertexTriangle.edge1[1]);

        fez = fabs(vertexTriangle.edge1[2]);

        AXISTEST_X01(vertexTriangle.edge1[2], vertexTriangle.edge1[1], fez, fey);

        AXISTEST_Y02(vertexTriangle.edge1[2], vertexTriangle.edge1[0], fez, fex);

        AXISTEST_Z0(vertexTriangle.edge1[1], vertexTriangle.edge1[0], fey, fex);



        fex = fabs(vertexTriangle.edge2[0]);

        fey = fabs(vertexTriangle.edge2[1]);

        fez = fabs(vertexTriangle.edge2[2]);

        AXISTEST_X2(vertexTriangle.edge2[2], vertexTriangle.edge2[1], fez, fey);

        AXISTEST_Y1(vertexTriangle.edge2[2], vertexTriangle.edge2[0], fez, fex);

        AXISTEST_Z12(vertexTriangle.edge2[1], vertexTriangle.edge2[0], fey, fex);


        /* Bullet 2: */

        /*  test if the box intersects the plane of the triangle */

        /*  compute plane equation of triangle: normal*x+d=0 */

//	CROSS(normal, e0, e1);

        // -NJMP- (line removed here)

        if (!planeBoxOverlap(vertexTriangle.normal, v0, half)) return 0;	// -NJMP-

        return 1;   /* box and triangle overlaps */

    }
}