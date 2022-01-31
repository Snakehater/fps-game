#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "reader.hpp"
#include "MaterialLib.hpp"

class Mesh
{
	struct vertex{
		float x;
		float y;
		float z;
	};
	struct uv{
		float u, v;
	};
	struct face {
		std::vector<std::string> a, b, c;
	};
	std::vector<vertex> vertices;
	std::vector<uv> uvs;
	int num_of_vertices;
	std::vector<std::string> split(std::string s, std::string delimiter, bool avoid_double = true){
		std::vector<std::string> output;
		
		size_t pos = 0;	
		std::string token;
		while ((pos = s.find(delimiter)) != std::string::npos) {
			/* If pos is 0 we have a double delimiter and something is probably wrong */
			if (pos == 0 && avoid_double) {
				s.erase(0, 1);
				continue;
			}
			token = s.substr(0, pos);
			output.push_back(token);
			s.erase(0, pos + delimiter.length());
		}
		// add the rest of the string to last element
		output.push_back(s);
		return output;
	}
	float scale;
	bool mesh_null; // if mesh is empty and acting as 'air' or 'void', passing NULL as filename in constructor causes this
	int stride_offset_var; // offset to pass to glDrawArrays
	int arr_offset; // offset in vertices array
public:
	std::string name;
	glm::vec3 position_vec;
	glm::vec3 rotation_vec;
	float rotation_degree;
	bool is_null() {
		return mesh_null;
	}
	int stride_offset() {
		return this->stride_offset_var;
	}
	int size(){
		return this->vertex_array_object.size();
	}
	int vert_num(){
		return this->num_of_vertices;
	}
	void set_position(float x, float y, float z) {
		this->position_vec = glm::vec3(x, y, z);
	}
	glm::vec3 get_position() {
		return this->position_vec;
	}
	void set_rotation_vec(float x, float y, float z) {
		this->rotation_vec = glm::vec3(x, y, z);
	}
	glm::vec3 get_rotation_vec() {
		return this->rotation_vec;
	}
	std::vector<float> vertex_array_object;

	Mesh() { // default constructor
		this->mesh_null = true;
	}
	
	Mesh(std::string object_name, std::vector<float> vector_in, int num_of_vertices_in, float scale_in = 1.0f, int* vertices_size = NULL, int* stride_offset_var_counter = NULL, int* arr_offset_cnt = NULL) {
		this->num_of_vertices = num_of_vertices_in;
		this->scale = scale_in;
		this->name = object_name;

		// copy vector to vertex_array_object
		vertex_array_object = vector_in;
		
		set_rotation_vec(1.0f, 1.0f, 1.0f);
		rotation_degree = 0.0f;

		if (vertices_size != NULL)
			*vertices_size += this->size();
		if (stride_offset_var_counter != NULL) {
			this->stride_offset_var = *stride_offset_var_counter;
			*stride_offset_var_counter += num_of_vertices;
		} else
			this->stride_offset_var = 0;

		if (arr_offset_cnt != NULL) {
			this->arr_offset = *arr_offset_cnt;
			*arr_offset_cnt += vertex_array_object.size();
		} else
			this->arr_offset = 0;
	}
	void fill_arr(float* output, int start);
};

void Mesh::fill_arr(float* output, int start = -1){
	if (start == -1)
		start = this->arr_offset;
	for(int i = 0; i < this->size(); i++) {
		output[start+i] = this->vertex_array_object[i];
	}
}
