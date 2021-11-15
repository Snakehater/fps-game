#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

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
	
	std::vector<std::string> split(std::string s, std::string delimiter){
		std::vector<std::string> output;
		
		size_t pos = 0;	
		std::string token;
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			output.push_back(token);
			s.erase(0, pos + delimiter.length());
		}
		// add the rest of the string to last element
		output.push_back(s);
		return output;
	}
	
public:
	std::vector<float> vertex_array_object;
	
	Mesh(const char *filename) {
		readfile(filename, &vertex_array_object);
	}
	void readfile(const char* filename, std::vector<float>* output);
};

void Mesh::readfile(const char *filename, std::vector<float>* output) {
	std::cout << "STARING::MESH::READING_FROM_FILE" << std::endl;
	std::string tmp;
	std::ifstream file(filename);
	if(!file) {
		std::cout << "ERROR::MESH::LOAD_FILE" << std::endl;
		return;
	}
	while(std::getline(file, tmp)) {
		char ctrl_id[3] = {tmp[0], tmp[1], '\0'};
		if (strcmp(ctrl_id, "v ") == 0) {
			vertex v;
			std::vector<std::string> vertex_raw = this->split(tmp, " ");
			v.x = std::stof(vertex_raw[1]);
			v.y = std::stof(vertex_raw[2]);
			v.z = std::stof(vertex_raw[3]);
			std::cout << v.x << ' ' << v.y << ' ' << v.z << std::endl;
			vertices.push_back(v);
		} else if (strcmp(ctrl_id, "vn") == 0) {
			uv u;
			std::vector<std::string> uv_raw = this->split(tmp, " ");
			u.u = std::stof(uv_raw[1]);
			u.v = std::stof(uv_raw[2]);
			std::cout << u.u << ' ' << u.v << std::endl;
			uvs.push_back(u);
		} else if (strcmp(ctrl_id, "f ") == 0) {
			std::vector<std::string> face_raw = this->split(tmp, " ");
			face f;
			// store string vectors somewhere 
			f.a = this->split(face_raw[1], "/");
			f.b = this->split(face_raw[2], "/");
			f.c = this->split(face_raw[3], "/");
			std::cout << face_raw[1] << ' ' << face_raw[2] << ' ' << face_raw[3] << std::endl;
			// prep failsafe
			uv failsafe;
			failsafe.u = 0; failsafe.v = 0;
			// prep for output
			vertex va = vertices[std::stoi(f.a[0])];
			uv ua;
			if (f.a.size() > 1)
				ua = uvs[std::stoi(f.a[1])];
			else
				ua = failsafe;

			vertex vb = vertices[std::stoi(f.b[0])];
			uv ub;
			if (f.b.size() > 1)
				ub = uvs[std::stoi(f.b[1])];
			else
				ub = failsafe;
			
			vertex vc = vertices[std::stoi(f.c[0])];
			uv uc;
			if (f.c.size() > 1)
				uc = uvs[std::stoi(f.c[1])];
			else
				uc = failsafe;
			// load data into output  -- each stride consists of 5 floats, x y z u v, is the format, vertex three and textcoord 2
			output->push_back(va.x); output->push_back(va.y); output->push_back(va.z); output->push_back(ua.u); output->push_back(ua.v);
			output->push_back(vb.x); output->push_back(vb.y); output->push_back(vb.z); output->push_back(ub.u); output->push_back(ub.v);
			output->push_back(vc.x); output->push_back(vc.y); output->push_back(vc.z); output->push_back(uc.u); output->push_back(uc.v);
		}
	}
}
