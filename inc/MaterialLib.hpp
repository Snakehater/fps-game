#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include "Material.hpp"
#include "reader.hpp"

class MaterialLib
{
	std::vector<std::string> _split(std::string s, std::string delimiter, bool avoid_couble = true);
public:
	std::vector<Material> materials;
	MaterialLib(void){};
	void load(const char *filename){
		// Creating reader with display of length 100 (100 #'s)
		std::cout << "-- reading file \"" << filename << "\"" << std::endl;
		reader infile(50, filename);

		std::string line;
		
		if(!infile) {
			std::cout << "ERROR::MTLLIB::LOAD_FILE" << std::endl;
			return;
		}
		
		Material material = Material();
		bool materialStarted = false; /* This is so we don't add empty materials to our vector */
		while(std::getline(infile, line)) {
		//	infile.drawbar();
			if (line.empty()) {
				if (materialStarted) {
					materials.push_back(material);
					materialStarted = false;
				}
				continue;
			}
			std::vector<std::string> splitted = this->_split(line, " ");
			const char* key = splitted[0].c_str();
			
			/* Handle keyword and load value into the material object */
			if (strcmp(key, "newmtl") == 0) {
				material.name = splitted[1];
				materialStarted = true;
			} else if (strcmp(key, "Ka") == 0) {
				material.ambient_color.x = std::stof(splitted[1]);
				material.ambient_color.y = std::stof(splitted[2]);
				material.ambient_color.z = std::stof(splitted[3]);
			} else if (strcmp(key, "Kd") == 0) {
				material.diffuse_color.x = std::stof(splitted[1]);
				material.diffuse_color.y = std::stof(splitted[2]);
				material.diffuse_color.z = std::stof(splitted[3]);
			} else if (strcmp(key, "Ks") == 0) {
				material.specular_color.x = std::stof(splitted[1]);
				material.specular_color.y = std::stof(splitted[2]);
				material.specular_color.z = std::stof(splitted[3]);
			} else if (strcmp(key, "Ns") == 0) {
				material.specular_weight = std::stof(splitted[1]);
			} else if (strcmp(key, "d") == 0)
				material.transparency = std::stof(splitted[1]);
		}
		infile.drawbar();
	};
};


std::vector<std::string> MaterialLib::_split(std::string s, std::string delimiter, bool avoid_double){
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
