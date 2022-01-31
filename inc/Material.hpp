#ifndef MATERIAL_H
#define MATERIAL_H
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>


class Material
{
public:
	std::string name;
	
	glm::vec3 ambient_color;
	glm::vec3 diffuse_color;
	glm::vec3 specular_color;
	float specular_weight;
	float transparency;
	
	Material(void){
		name = "";
		ambient_color = glm::vec3(255,255,255);
		diffuse_color = glm::vec3(255,255,255);
		specular_color = glm::vec3(255,255,255);
		specular_weight = 1.0f;
		transparency = 1.0f;
	};
};
#endif
