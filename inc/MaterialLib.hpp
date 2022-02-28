#ifndef MATERIAL_LIB_H
#define MATERIAL_LIB_H
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
	void _removeCarriage(std::string *str);
public:
	std::vector<Material> materials;
	Material selected_m;
	bool selected;
	long unsigned int selected_idx;
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
			this->_removeCarriage(&line);
			std::vector<std::string> splitted = this->_split(line, " ");
			const char* key = splitted[0].c_str();
			
			/* Handle keyword and load value into the material object */
			if (strcmp(key, "newmtl") == 0) {
				if (materialStarted) 
					materials.push_back(material);
				material.name = splitted[1];
				materialStarted = true;
			} else if (strcmp(key, "Ka") == 0) {
				material.ambient_color.x = std::stof(splitted[1]) * 255;
				material.ambient_color.y = std::stof(splitted[2]) * 255;
				material.ambient_color.z = std::stof(splitted[3]) * 255;
			} else if (strcmp(key, "Kd") == 0) {
				material.diffuse_color.x = std::stof(splitted[1]) * 255;
				material.diffuse_color.y = std::stof(splitted[2]) * 255;
				material.diffuse_color.z = std::stof(splitted[3]) * 255;
			} else if (strcmp(key, "Ks") == 0) {
				material.specular_color.x = std::stof(splitted[1]) * 255;
				material.specular_color.y = std::stof(splitted[2]) * 255;
				material.specular_color.z = std::stof(splitted[3]) * 255;
			} else if (strcmp(key, "Ns") == 0) {
				material.specular_weight = std::stof(splitted[1]);
			} else if (strcmp(key, "d") == 0) {
				material.transparency = std::stof(splitted[1]);
			} else if (strcmp(key, "map_Ka") == 0)
				material.ambient_map = splitted[splitted.size() - 1]; // NB! additional texture options is removed
		}
		if (materialStarted) 
			materials.push_back(material);
		infile.drawbar();
	};
	void select(const char *selector) {
		long unsigned int i = 0;
		for (Material m : materials) {
			if (strcmp(m.name.c_str(), selector) == 0) {
				this->selected_m = m;
				this->selected_idx = i;
				this->selected = true;
				return;
			}
			i++;
		}
		this->selected = false;
	}
	void select(long unsigned int selector) {
		if (materials.size() > selector) {
			this->selected_m = materials[selector];
			this->selected_idx = selector;
			this->selected = true;
			return;
		}
		this->selected = false;
	}
	void push_mtl(std::vector<float>* output) {
		if (!this->selected) {
			output->push_back(255.0f);
			output->push_back(255.0f);
			output->push_back(255.0f);
			return;
		}

		output->push_back(this->selected_m.ambient_color.x);
		output->push_back(this->selected_m.ambient_color.y);
		output->push_back(this->selected_m.ambient_color.z);
	}

};

void MaterialLib::_removeCarriage(std::string *str) {
	for (long unsigned int i = 0; i < str->length(); i++) {
		/* Match with windows carriage return */
		if ((*str)[i] == 0x0D) {
			str->erase(i, 1);
			i--;
		}
	}
}


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
#endif
