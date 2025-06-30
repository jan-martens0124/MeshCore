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

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "meshcore/acceleration/AABBOctree.h"
#include "meshcore/geometric/AABBTriangleData.h"

int planeBoxOverlap(const glm::vec3& normal, const glm::vec3& vert, const glm::vec3& maxbox)	// -NJMP-
{
	int q;
	glm::vec3 vmin, vmax;
	float v;
	for (q = 0; q <= 2; q++) {
		v = vert[q];					// -NJMP-
		if (normal[q] > 0.0) {
			vmin[q] = -maxbox[q] - v;	// -NJMP-
			vmax[q] = maxbox[q] - v;	// -NJMP-
		}
		else {
			vmin[q] = maxbox[q] - v;	// -NJMP-
			vmax[q] = -maxbox[q] - v;	// -NJMP-
		}
	}

	if (glm::dot(normal, vmin) > 0.0) return 0;	// -NJMP-
	if (glm::dot(normal, vmax) >= 0.0) return 1;	// -NJMP-

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

namespace Intersection{

    int intersect(const AABB& aabb, const VertexTriangle& vertexTriangle){

		// TODO consider replacing this with the cleaned up version for better readability, maintainability

        /* Bullet 1: */

        /*  first test overlap in the {x,y,z}-directions */
        /*  find min, max of the triangle each direction, and test for overlap in */
        /*  that direction -- this is equivalent to testing a minimal AABB around */
        /*  the triangle against the AABB */

    	if(!intersect(aabb,vertexTriangle.bounds)){
    		return false; // Triangles' AABB completely outside AABB
    	}

    	if(aabb.containsPoint(vertexTriangle.vertices[0]) || aabb.containsPoint(vertexTriangle.vertices[1]) || aabb.containsPoint(vertexTriangle.vertices[2])) {
    		return true; // Containment of any vertex means intersection
    	}
        /*    use separating axis theorem to test overlap between triangle and box */
        /*    need to test for overlap in these directions: */
        /*    1) the {x,y,z}-directions (actually, since we use the AABB of the triangle */
        /*       we do not even need to test these) */
        /*    2) normal of the triangle */
        /*    3) crossproduct(edge from tri, {x,y,z}-directin) */
        /*       this gives 3x3=9 more tests */

        glm::vec3 v0{}, v1{}, v2{};

        double min, max, p0, p1, p2, rad, fex, fey, fez;		// -NJMP- "d" local variable removed

        glm::vec3 half = aabb.getHalf();
        const float* boxhalfsize = glm::value_ptr(half);

        /* This is the fastest branch on Sun */

        /* move everything so that the boxcenter is in (0,0,0) */

        glm::vec3 center = aabb.getCenter();
        v0 = vertexTriangle.vertices[0] - center;
        v1 = vertexTriangle.vertices[1] - center;
        v2 = vertexTriangle.vertices[2] - center;

        /* Bullet 3:  */

        /*  test the 9 tests first (this was faster) */

        fex = fabs(vertexTriangle.edges[0][0]);

        fey = fabs(vertexTriangle.edges[0][1]);

        fez = fabs(vertexTriangle.edges[0][2]);

        AXISTEST_X01(vertexTriangle.edges[0][2], vertexTriangle.edges[0][1], fez, fey);

        AXISTEST_Y02(vertexTriangle.edges[0][2], vertexTriangle.edges[0][0], fez, fex);

        AXISTEST_Z12(vertexTriangle.edges[0][1], vertexTriangle.edges[0][0], fey, fex);



        fex = fabs(vertexTriangle.edges[1][0]);

        fey = fabs(vertexTriangle.edges[1][1]);

        fez = fabs(vertexTriangle.edges[1][2]);

        AXISTEST_X01(vertexTriangle.edges[1][2], vertexTriangle.edges[1][1], fez, fey);

        AXISTEST_Y02(vertexTriangle.edges[1][2], vertexTriangle.edges[1][0], fez, fex);

        AXISTEST_Z0(vertexTriangle.edges[1][1], vertexTriangle.edges[1][0], fey, fex);



        fex = fabs(vertexTriangle.edges[2][0]);

        fey = fabs(vertexTriangle.edges[2][1]);

        fez = fabs(vertexTriangle.edges[2][2]);

        AXISTEST_X2(vertexTriangle.edges[2][2], vertexTriangle.edges[2][1], fez, fey);

        AXISTEST_Y1(vertexTriangle.edges[2][2], vertexTriangle.edges[2][0], fez, fex);

        AXISTEST_Z12(vertexTriangle.edges[2][1], vertexTriangle.edges[2][0], fey, fex);


        /* Bullet 2: */

        /*  test if the box intersects the plane of the triangle */

        /*  compute plane equation of triangle: normal*x+d=0 */

        // -NJMP- (line removed here)

        if (!planeBoxOverlap(vertexTriangle.normal, v0, half)) return 0;	// -NJMP-

        return 1;   /* box and triangle overlaps */

    }

	bool intersect(const AABB& aabb, const VertexTriangle &t, const AABBTriangleData& data) {

	    // https://omnigoat.github.io/2015/03/09/box-triangle-intersection/
	    if(!intersect(aabb,t.bounds)){
	        return false; // Triangles' AABB completely outside AABB
	    }

		if(aabb.containsPoint(t.vertices[0]) || aabb.containsPoint(t.vertices[1]) || aabb.containsPoint(t.vertices[2])) {
		    return true; // Containment of any vertex means intersection
		}

	    // p & delta-p
	    const auto& p  = aabb.getMinimum();
	    const auto dp = aabb.getMaximum() - p;

	    // test for triangle-plane/box overlap
	    auto c = glm::vec3(t.normal.x > 0.0f ? dp.x : 0.0f,
	                        t.normal.y > 0.0f ? dp.y : 0.0f,
	                        t.normal.z > 0.0f ? dp.z : 0.0f);

	    const auto d1 = glm::dot(t.normal, c - t.vertices[0]);
	    const auto d2 = glm::dot(t.normal, dp - c - t.vertices[0]);
	    const auto dot = glm::dot(t.normal, p);

	    if ((dot + d1) * (dot  + d2) > 0.0f) {
	        return false;
	    }

	    // xy-plane projection-overlap
	    auto de0xy = data.de0xy_base + std::max(0.0f, dp.x * data.ne0xy.x) + std::max(0.0f, dp.y * data.ne0xy.y);
	    auto de1xy = data.de1xy_base + std::max(0.0f, dp.x * data.ne1xy.x) + std::max(0.0f, dp.y * data.ne1xy.y);
	    auto de2xy = data.de2xy_base + std::max(0.0f, dp.x * data.ne2xy.x) + std::max(0.0f, dp.y * data.ne2xy.y);
	    const auto pxy = glm::vec2(p.x, p.y);
	    if ((glm::dot(data.ne0xy, pxy) + de0xy) < 0.0f || (glm::dot(data.ne1xy, pxy) + de1xy) < 0.0f || (glm::dot(data.ne2xy, pxy) + de2xy) < 0.0f) {
	        return false;
	    }

	    // yz-plane projection overlap
	    auto de0yz = data.de0yz_base + std::max(0.0f, dp.y * data.ne0yz.x) + std::max(0.0f, dp.z * data.ne0yz.y);
	    auto de1yz = data.de1yz_base + std::max(0.0f, dp.y * data.ne1yz.x) + std::max(0.0f, dp.z * data.ne1yz.y);
	    auto de2yz = data.de2yz_base + std::max(0.0f, dp.y * data.ne2yz.x) + std::max(0.0f, dp.z * data.ne2yz.y);

	    auto pyz = glm::vec2(p.y, p.z);

	    if ((glm::dot(data.ne0yz, pyz) + de0yz) < 0.0f || (glm::dot(data.ne1yz, pyz) + de1yz) < 0.0f || (glm::dot(data.ne2yz, pyz) + de2yz) < 0.0f) {
	        return false;
	    }

	    // zx-plane projection overlap
	    auto de0zx = data.de0zx_base + std::max(0.0f, dp.z * data.ne0zx.x) + std::max(0.0f, dp.x * data.ne0zx.y);
	    auto de1zx = data.de1zx_base + std::max(0.0f, dp.z * data.ne1zx.x) + std::max(0.0f, dp.x * data.ne1zx.y);
	    auto de2zx = data.de2zx_base + std::max(0.0f, dp.z * data.ne2zx.x) + std::max(0.0f, dp.x * data.ne2zx.y);

	    const auto pzx = glm::vec2(p.z, p.x);

	    if ((glm::dot(data.ne0zx, pzx) + de0zx) < 0.0f || (glm::dot(data.ne1zx, pzx) + de1zx) < 0.0f || (glm::dot(data.ne2zx, pzx) + de2zx) < 0.0f) {
	        return false;
	    }

	    return true;
	}
}