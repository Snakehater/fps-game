#include "main.hpp"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// prototype macros
void framebuffer_size_callback(GLFWwindow* window, int width, int height);  
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
int  get_mesh_offset(int* mesh_offsets, int target);
void updateGravity(void);

// camera
Camera camera(glm::vec3(0, 22.0f, 0), glm::vec3(0.0f, 2.0f, 0.0f), YAW, -89.9f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// player
float v_speed = 0;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

VecUtils vecUtils;
CollisionUtils collisionUtils;

/* Collision system, items that the player can collide with should be added to here */
std::vector<Mesh*> collisionSystem;

int main() {
	/* We firstly want to load our models since this requires time */
	int vertices_size = 0;
	int stride_offset_counter = 0;
	int arr_offset_cnt = 0;

	Mesh nullCube(NULL);
	Mesh regular_cube("res/objects/cube.obj", 0.5f, &vertices_size, &stride_offset_counter, &arr_offset_cnt);
	Mesh redCube("res/objects/red_cube.obj", 0.5f, &vertices_size, &stride_offset_counter, &arr_offset_cnt);
	Mesh greenCube("res/objects/green_cube.obj", 0.5f, &vertices_size, &stride_offset_counter, &arr_offset_cnt);
	Mesh blueCube("res/objects/blue_cube.obj", 0.5f, &vertices_size, &stride_offset_counter, &arr_offset_cnt);
	Mesh yellowCube("res/objects/yellow_cube.obj", 0.5f, &vertices_size, &stride_offset_counter, &arr_offset_cnt);
//	Mesh plane_mesh("res/objects/plane.obj", 0.5f, &vertices_size, &stride_offset_counter, &arr_offset_cnt);
	Mesh plane_mesh("res/objects/map.obj", 0.5f, &vertices_size, &stride_offset_counter, &arr_offset_cnt);

	return 0;

	std::vector<Mesh*> mesh_types;
	mesh_types.push_back(&nullCube);
	mesh_types.push_back(&regular_cube);
	mesh_types.push_back(&redCube);
	mesh_types.push_back(&greenCube);
	mesh_types.push_back(&blueCube);
	mesh_types.push_back(&yellowCube);

	/* Add meshes to collision system */
	collisionSystem.push_back(&plane_mesh);
	collisionSystem.push_back(&regular_cube);

	regular_cube.set_position(-2, 2, 2);

	/* Model loading should now be done */

	// instantiate the GLFW window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	
	int major, minor, rev;
	glfwGetVersion(&major, &minor, &rev);
	std::cout << "GLFW v" << major << '.' << minor << '.' << rev << std::endl;
	std::cout << glfwGetVersionString() << std::endl;

	// create a window object
	GLFWwindow* window = glfwCreateWindow(800, 600, "FPS", glfwGetPrimaryMonitor(), NULL);
	if (window == NULL) {
	    std::cout << "Failed to create GLFW window" << std::endl;
	    glfwTerminate();
	    return -1;
	}
	glfwMakeContextCurrent(window); // assign it to main thread

	// GLAD manages function pointers for OpenGL so we want to initialize GLAD before we call any OpenGL function:
	// We pass GLAD the function to load the address of the OpenGL function pointers which is OS-specific. GLFW gives us glfwGetProcAddress that defines the correct function based on which OS we're compiling for.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
	    std::cout << "Failed to initialize GLAD" << std::endl;
	    return -1;
	}

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
	
	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST); // this removes fragments that are behind other fragments
	
	// build and compile our shader program
	// ------------------------------------
	// we first try to load shaders from files
	Shader ourShader( "shaders/vertex_core.glsl", "shaders/fragment_core.glsl" );
	
	//float* vertices = &mesh.vertex_array_object[0];
	float vertices[vertices_size] = { };

	regular_cube.fill_arr(&vertices[0]);
	redCube.fill_arr(&vertices[0]);
	greenCube.fill_arr(&vertices[0]);
	blueCube.fill_arr(&vertices[0]);
	yellowCube.fill_arr(&vertices[0]);
	plane_mesh.fill_arr(&vertices[0]);

	// vertex buffer objects (VBO) 
	// vertex array object (VAO)
	// element buffer object (EBO)
	unsigned int VBO, VAO; //, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	
	// ..:: Initialization code :: ..
	// 1. bind Vertex Array Object	
	glBindVertexArray(VAO);

	// 2. copy our vertices array in a vertex buffer for OpenGL to use
	// bind buffer and store vertices on graphics card
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices_size, vertices, GL_STATIC_DRAW);
	// (removed) 3. copy our index array in a element buffer for OpenGL to use
	
	// 4. then set the vertex attributes pointers
	
	// on the attribute pointers: 5 is the number of indexes that points to an index. 3pos + 2texcoord
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// load and create a texture
	// -------------------------
	unsigned int texture1;
	glGenTextures( 1, &texture1 );
	glBindTexture( GL_TEXTURE_2D, texture1 );
	// set the texture wrapping parameters
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	// set texture filtering parameters
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	// load image, create texture and generate mipmap
	//stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	int width, height, nrChannels;
	unsigned char *data = stbi_load( "res/textures/textureatlas.jpg", &width, &height, &nrChannels, 0);

	if ( data ) {
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
		glGenerateMipmap( GL_TEXTURE_2D );
	} else
		std::cout << "ERROR::LOAD::TEXTURE" << std::endl;

	stbi_image_free( data );
	
	// tell opengl for each sample to which texture unit it belongs to ( only once )
	// -----------------------------------------------------------------------------
	ourShader.use();
	glUniform1i( glGetUniformLocation( ourShader.ID, "texture1" ), 0 ); // 0 is our texture id, for another texture, use another id


	// uncomment this call to draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------

	// start our renderloop
	while(!glfwWindowShouldClose(window)) {
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
		// input
		// -----
		processInput(window);
		
		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// bind textures on corresponding texture units
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, texture1 );
		
		// Going 3d with our object
		// ------------------------
		// for our 3d scene to work, we need to perform following calc and get last scene before the viewport transform:
		// Vclip = Mprojection ⋅ Mview ⋅ Mmodel ⋅ Vlocal
		// where each variable is a matrix generated with glm for simplicity and optimatiations.	
		
		// activate shader
		ourShader.use();


		// prep projection matrix since we don't need to set a new one every time
		//					      FOV		aspect ratio		 near far clipping distance
		glm::mat4 projection = glm::perspective( glm::radians( camera.zoom ), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f );
		ourShader.setMat4( "projection", projection );
		

		// create tranformation matrices for view/camera and pass it to the shader
		glm::mat4 view = camera.GetViewMatrix(); 
		ourShader.setMat4( "view", view );
		
		// render boxes
		glBindVertexArray( VAO );
		
		// calculate the model matrix for each object and pass it to shader before drawing
		glm::mat4 model = glm::mat4( 1.0f );
		model = glm::translate(model, plane_mesh.get_position());
		model = glm::rotate( model, glm::radians(plane_mesh.rotation_degree), plane_mesh.get_rotation_vec() );
		ourShader.setMat4( "model", model );
		glDrawArrays( GL_TRIANGLES, plane_mesh.stride_offset(), plane_mesh.vert_num());

		//////////////////////

		model = glm::mat4( 1.0f );
		model = glm::translate(model, regular_cube.get_position());
		model = glm::rotate( model, glm::radians(regular_cube.rotation_degree), regular_cube.get_rotation_vec() );
		ourShader.setMat4( "model", model );
		glDrawArrays( GL_TRIANGLES, regular_cube.stride_offset(), regular_cube.vert_num());

		model = glm::mat4( 1.0f );
		model = glm::translate(model, redCube.get_position());
		model = glm::rotate( model, glm::radians(redCube.rotation_degree), redCube.get_rotation_vec() );
		ourShader.setMat4( "model", model );
		glDrawArrays( GL_TRIANGLES, redCube.stride_offset(), redCube.vert_num());
		
		
		model = glm::mat4( 1.0f );
		model = glm::translate(model, greenCube.get_position());
		model = glm::rotate( model, glm::radians(greenCube.rotation_degree), greenCube.get_rotation_vec() );
		ourShader.setMat4( "model", model );
		glDrawArrays( GL_TRIANGLES, greenCube.stride_offset(), greenCube.vert_num());

		model = glm::mat4( 1.0f );
		model = glm::translate(model, blueCube.get_position());
		model = glm::rotate( model, glm::radians(blueCube.rotation_degree), blueCube.get_rotation_vec() );
		ourShader.setMat4( "model", model );
		glDrawArrays( GL_TRIANGLES, blueCube.stride_offset(), blueCube.vert_num());

		//////////////////////

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window); 
		glfwPollEvents();
		
//		auto start = std::chrono::high_resolution_clock::now();
		//updateGravity();
/*		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
		std::cout << std::endl << "time:" << std::endl;
		std::cout << ((float)duration.count())/1000 << std::endl;
		glfwTerminate();
		return 0; */

		usleep( 1 );
	}
	glfwTerminate(); // delete all of GLFW's resources that were allocated
	return 0;
}

#define FEET_COL_DIST 2 // distance from player to feet for collision system

void updateGravity() {
	/* Collision system first */
	glm::vec3 ppos = camera.position;
	glm::vec3 pdir = glm::vec3(0, -1, 0);
	bool freefall = true;
	glm::vec3 closest_I;
	bool closest_I_set = false;

	for (Mesh *mesh : collisionSystem) {
		std::vector<float> v_buffer = mesh->vertex_array_object;

		vecUtils.setMatrices(camera, *mesh, SCR_WIDTH, SCR_HEIGHT);
		
		/* We do three vertices each time */
		for (int i = 0; i < mesh->vsize; i += 3) {
			
			glm::vec3 v1, v2, v3;
			v1.x = v_buffer[i + 0 + 0];
			v1.y = v_buffer[i + 0 + 1];
			v1.z = v_buffer[i + 0 + 2];
			
			v2.x = v_buffer[i + 5 + 0];
			v2.y = v_buffer[i + 5 + 1];
			v2.z = v_buffer[i + 5 + 2];
			
			v3.x = v_buffer[i + 10 + 0];
			v3.y = v_buffer[i + 10 + 1];
			v3.z = v_buffer[i + 10 + 2];
			
			glm::vec3 normal = vecUtils.computeNormal(v1, v2, v3);

			/* */
			vecUtils.transformVertexWorld(&v1);
			vecUtils.transformVertexWorld(&v2);
			vecUtils.transformVertexWorld(&v3);
			
			if (v_speed >= 0) {
				if (collisionUtils.triangleIntersect(v1, v2, v3, normal, ppos, pdir)) {
					freefall = false;
					glm::vec3 I = collisionUtils.planeIntersect(normal, v1, ppos, pdir);
					if (!closest_I_set || glm::length(ppos - I) - glm::length(ppos - closest_I)) {
						closest_I_set = true;
						closest_I = I;
					}
					if (glm::length(ppos - I) <= FEET_COL_DIST) {
						v_speed = 0;
						return; // if we collide with a triangle, stop movement
					}
				}
			}
		}
	}

	/* Calc new position according to gravity */
	float t = 0.05;
	float g = 9.82;

	glm::vec3 xf = camera.position - glm::vec3(0, v_speed*t + 0.5*g*(t*t), 0); // new position
	v_speed += 0.5f*g*t; // instead of assigning speed to the distance / time we just add above equation to it and divide with t

	/* Make sure we our new position is inside of object bounding box */
	if (!freefall && glm::length(xf - closest_I) < FEET_COL_DIST)
		camera.position = closest_I + glm::vec3(0, FEET_COL_DIST, 0);
	else
		camera.position = xf;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	v_speed = -5;
//        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
	camera.ProcessKeyboard(DOWN, deltaTime);
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
