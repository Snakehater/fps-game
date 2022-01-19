#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>


class CollisionUtils
{
public:
	CollisionUtils(){}; /* Null constructor */
	glm::vec3 planeIntersect(glm::vec3 normal, glm::vec3 random_point, Camera camera) {
		glm::vec3 x0 = camera.position; // player position
		glm::vec3 v1 = camera.cam_front;  // player look vector
		glm::vec3 n  = normal; // the plane normal
		glm::vec3 c  = random_point; // we need a random point on the plane
		glm::vec3 w1 = c - x0; // vector from player to vec c

// this is slow calc
/*		glm::vec3 v2; // the direction from player to plane
		glm::vec3 w2; // v2 but all the way to the plane

		v2 = ((v1*n)/(n*n))*n;
		w2 = ((w1*n)/(n*n))*n;
		glm::vec3 k = w2 / v2; */

// faster calc - this is because stuff in v2 and w2 cancels out
		/* In order to divide, we need to get the lengthes since that is what we need to divide to get the proportion */
		float k = glm::length(w1*n)/glm::length(v1*n);
		
		glm::vec3 ray_vec = k * v1; // vector going from x0 to I
		glm::vec3 I = x0 + ray_vec; // intersection point on the plane
		return I;
	}
	// projection of vector v1 onto v2
	glm::vec3 project(glm::vec3 v1, glm::vec3 v2) {
		float v2_ls = glm::length(v2)*glm::length(v2);
		return v2 * ( glm::dot(v2, v1)/v2_ls);
//		return (glm::length(onto * of)/(glm::length(onto)*glm::length(onto)))*onto;
	}
	float barycentricCoordinateA(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 I) {
		glm::vec3 AB = B - A;
		glm::vec3 CB = B - C;
		glm::vec3 AI = I - A;
		
		// create vector perpendicular to CB (opposite edge from point A)
		glm::vec3 v = AB - project(AB, CB);
		
		// project intersection point onto V and compare to V, if length is bigger for V, I is between point A and opposite edge
		return 1 - glm::length(project(AI, v))/glm::length(project(AB, v));
	}
	bool triangleIntersect(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 normal, Camera camera) {
		glm::vec3 I = planeIntersect(normal, v1, camera);

		float a = barycentricCoordinateA(v1, v2, v3, I);
		float b = barycentricCoordinateA(v2, v3, v1, I);
		float c = barycentricCoordinateA(v3, v1, v2, I);
		glm::vec3 coors = glm::vec3(a, b, c);
		
		if (	coors.x < 0 ||
			coors.y < 0 ||
			coors.z < 0 ) 
			return false;
		else
			return true;
	}		
};
