#define MAX_TEXTURES 128

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <thread>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

// math functions Open(GL M)athematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#define STB_IMAGE_IMPLEMENTATION // only import relevant stuff from stb_image.h
#include "stb_image.h"

#include <chrono>

#include "shader.hpp"
#include "camera.hpp"
#include "mesh.hpp"
#include "VecUtils.hpp"
#include "CollisionUtils.hpp"
#include "ObjLoader.hpp"
