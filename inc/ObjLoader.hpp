#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "reader.hpp"
#include "MaterialLib.hpp"

class ObjLoader
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
public:

	ObjLoader(const char *filename, std::vector<Mesh> *mesh_vector, float scale = 1.0f, int* vertices_size = NULL, int* stride_offset_var_counter = NULL, int* arr_offset_cnt = NULL) {

		std::string object_name = "";
		std::vector<float> vertex_buffer;
		int num_of_vertices;

		// Creating reader with display of length 100 (100 #'s)
		reader infile(50, filename);
		std::cout << "-- reading file \"" << filename << "\"" << std::endl;

		std::string tmp;
		
		if(!infile) {
			std::cout << "ERROR::OBJLOADER::LOAD_FILE" << std::endl;
			return;
		}
		MaterialLib materialLib;

		while(std::getline(infile, tmp)) {
			infile.drawbar();
			
			std::vector<std::string> splitted = this->split(tmp, " ");
			const char *ctrl_id = splitted[0].c_str();
			if (strcmp(ctrl_id, "o") == 0) {
				if (object_name.length() != 0) {
					Mesh mesh(object_name, vertex_buffer, num_of_vertices, scale, vertices_size, stride_offset_var_counter, arr_offset_cnt);
					mesh_vector->push_back(mesh);
					vertex_buffer.clear();
					num_of_vertices = 0;
					object_name = splitted[1];
				} else
					object_name = splitted[1];
			} else if (strcmp(ctrl_id, "mtllib") == 0) {
				std::cout << '\r';

				/* Split path and mesh file name in order to add the mtl file to the path */
				std::string f_name = filename;
				size_t pos = f_name.find_last_of("/");
				
				/* Get file name and remove ending returns and new lines etc */
				std::string lib_fname = tmp.substr(7);
				lib_fname.erase(lib_fname.find_last_not_of(" \n\r\t")+1);
				
				/* Assemble file path */
				std::string lib_path = f_name.substr(0, pos);
				lib_path += "/";
				lib_path += lib_fname;

				/* Load the file */
				materialLib.load(lib_path.c_str());
			} else if (strcmp(ctrl_id, "usemtl") == 0) {
				materialLib.select(splitted[1].c_str());
			} else if (strcmp(ctrl_id, "v") == 0) {
				vertex v;
				std::vector<std::string> vertex_raw = this->split(tmp, " ");
				v.x = std::stof(vertex_raw[1]) * scale;
				v.y = std::stof(vertex_raw[2]) * scale;
				v.z = std::stof(vertex_raw[3]) * scale;
	//			std::cout << v.x << ' ' << v.y << ' ' << v.z << std::endl;
				vertices.push_back(v);
			} else if (strcmp(ctrl_id, "vt") == 0) {
				uv u;
				std::vector<std::string> uv_raw = this->split(tmp, " ");
				u.u = std::stof(uv_raw[1]);
				u.v = std::stof(uv_raw[2]);
	//			std::cout << u.u << ' ' << u.v << std::endl;
				uvs.push_back(u);
			} else if (strcmp(ctrl_id, "f") == 0) {
				num_of_vertices += 3;
				std::vector<std::string> face_raw = this->split(tmp, " ");
				face f;
				// store string vectors somewhere 
				f.a = this->split(face_raw[1], "/");
				f.b = this->split(face_raw[2], "/");
				f.c = this->split(face_raw[3], "/");
	//			std::cout << face_raw[1] << ' ' << face_raw[2] << ' ' << face_raw[3] << std::endl;
				// prep failsafe
				uv failsafe;
				failsafe.u = 0; failsafe.v = 0;
				// prep for output
				vertex va = vertices[std::stoi(f.a[0]) - 1];
				uv ua;
				if (f.a.size() > 1)
					ua = uvs[std::stoi(f.a[1]) - 1];
				else
					ua = failsafe;

				vertex vb = vertices[std::stoi(f.b[0]) - 1];
				uv ub;
				if (f.b.size() > 1)
					ub = uvs[std::stoi(f.b[1]) - 1];
				else
					ub = failsafe;
				
				vertex vc = vertices[std::stoi(f.c[0]) - 1];
				uv uc;
				if (f.c.size() > 1)
					uc = uvs[std::stoi(f.c[1]) - 1];
				else
					uc = failsafe;
				// load data into output  -- each stride consists of 8 floats, x y z u v r g b, is the format, vertex three, textcoord 2 and color 3
				vertex_buffer.push_back(va.x); vertex_buffer.push_back(va.y); vertex_buffer.push_back(va.z); vertex_buffer.push_back(ua.u); vertex_buffer.push_back(ua.v);
					materialLib.push_mtl(&vertex_buffer);
				vertex_buffer.push_back(vb.x); vertex_buffer.push_back(vb.y); vertex_buffer.push_back(vb.z); vertex_buffer.push_back(ub.u); vertex_buffer.push_back(ub.v);
					materialLib.push_mtl(&vertex_buffer);
				vertex_buffer.push_back(vc.x); vertex_buffer.push_back(vc.y); vertex_buffer.push_back(vc.z); vertex_buffer.push_back(uc.u); vertex_buffer.push_back(uc.v);
					materialLib.push_mtl(&vertex_buffer);
			}
		}
		// above code will only create a new mesh when obj file states a new object, therefore, we need to add the last mesh manually:
		Mesh mesh(object_name, vertex_buffer, num_of_vertices, scale, vertices_size, stride_offset_var_counter, arr_offset_cnt);
		mesh_vector->push_back(mesh);
		vertex_buffer.clear();
		num_of_vertices = 0;

		// draw bar one last time to get the 100%
		infile.drawbar();
	}
};
