#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>


class VecUtils
{
public:
	VecUtils(void){}; /* void constructor */
	glm::vec3 computeNormal(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
		
		this->transformVertexWorld(&v1);
		this->transformVertexWorld(&v2);
		this->transformVertexWorld(&v3);

		// perform cross product of two lines on plane to find out normal
		glm::vec3 normal = glm::cross((v1 - v2), (v2 - v3));
		normal = glm::normalize(normal);
		return normal;
		
	}
	glm::vec3 crossProduct(const glm::vec3 vector1, const glm::vec3 rhs) {
		float x = vector1.x;
		float y = vector1.y;
		float z = vector1.z;

		return glm::vec3(	y*rhs.z - z*rhs.y,
					z*rhs.x - x*rhs.z,
					x*rhs.y - y*rhs.x);
	}
	void normalize(glm::vec3 *vec) {
		float x = vec->x;
		float y = vec->y;
		float z = vec->z;

		double length = sqrt(x*x+y*y+z*z);

		vec->x = x/length;
		vec->y = y/length;
		vec->z = z/length;
	}
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
	/* Sets the matrices to the value of given parameters */
	void setMatrices(glm::mat4 model_in, glm::mat4 view_in, glm::mat4 projection_in) {
		this->model = model_in;
		this->view = view_in;
		this->projection = projection_in;
	}
	/* Sets the matrices automatically given the camera object and mesh object */
	void setMatrices(Camera camera, Mesh mesh, const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT) {
		glm::mat4 projection = glm::perspective( glm::radians( camera.zoom ), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f );
		glm::mat4 view = camera.GetViewMatrix(); 

		glm::mat4 model = glm::mat4( 1.0f );
		model = glm::translate(model, mesh.get_position());
		model = glm::rotate( model, glm::radians(mesh.rotation_degree), mesh.get_rotation_vec() );
		this->setMatrices(model, view, projection);
	}
	/* Transforms the vertex location in the view space according to matrices */
	glm::vec3 transformVertex(glm::vec3 vertex) {
		glm::vec4 pos4 = projection * view * model * glm::vec4(vertex, 1.0f);
		glm::vec3 pos3(pos4);
		return pos3;
	}
	/* Transforms the vertex location in the view space according to matrices, with pointer */
	void transformVertex(glm::vec3 *vertex) {
		glm::vec4 pos4 = projection * view * model * glm::vec4(*vertex, 1.0f);
		glm::vec3 pos3(pos4);
		*vertex = pos3;
	}
	/* Transforms the vertex location relative to the world without camera influence */
	void transformVertexWorld(glm::vec3 *vertex) {
		glm::vec4 pos4 = model * glm::vec4(*vertex, 1.0f);
		glm::vec3 pos3(pos4);
		*vertex = pos3;
	}

};
