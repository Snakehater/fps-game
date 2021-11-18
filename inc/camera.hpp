#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  5.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// camera Attributes
	glm::vec3 position;
	glm::vec3 cam_front;
	glm::vec3 pos_front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 world_up;
	// euler Angles
	float yaw;
	float pitch;
	// camera options
	float movement_speed;
	float mouse_sensitivity;
	float zoom;

	// constructor with vectors
	Camera(glm::vec3 position_in = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw_in = YAW, float pitch_in_in = PITCH) : cam_front(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), zoom(ZOOM) {
		position = position_in;
		world_up = up;
		yaw = yaw_in;
		pitch = pitch_in_in;
		updateCameraVectors();
	}
	// constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw_in, float pitch_in_in) : cam_front(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), zoom(ZOOM) {
		position = glm::vec3(posX, posY, posZ);
		world_up = glm::vec3(upX, upY, upZ);
		yaw = yaw_in;
		pitch = pitch_in_in;
		updateCameraVectors();
	}

	// returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix() {
		// lookAt returns a view matrix used in caller function. it needs the camera position, a target position and a vector taht represents the up vector in world space, the up vector is used for calculating the right vector for the camera
		return glm::lookAt(position, position + cam_front, up);
	}

	// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime) {
		float velocity = movement_speed * deltaTime;
			if (direction == FORWARD)
				position += pos_front * velocity;
			if (direction == BACKWARD)
				position -= pos_front * velocity;
			if (direction == LEFT)
				position -= right * velocity;
			if (direction == RIGHT)
				position += right * velocity;
			if (direction == UP) {
				position += glm::vec3(0.0f, 1.0f, 0.0f) * velocity;
			}
			if (direction == DOWN) {
				position += glm::vec3(0.0f, -1.0f, 0.0f) * velocity;
			}
	}

	// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainpitch = true) {
		xoffset *= mouse_sensitivity;
		yoffset *= mouse_sensitivity;

		yaw   += xoffset;
		pitch += yoffset;

		// make sure that when pitch_in_in is out of bounds, screen doesn't get flipped
		if (constrainpitch)
		{
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
		}

		// update front, right and up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
		{
			zoom -= (float)yoffset;
			if (zoom < 1.0f)
				zoom = 1.0f;
			if (zoom > 45.0f)
				zoom = 45.0f; 
		}

	private:
		// calculates the front_in vector from the Camera's (updated) Euler Angles
		void updateCameraVectors() {
			// calculate the new camera front vector
			glm::vec3 cam_front_in;
			cam_front_in.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			cam_front_in.y = sin(glm::radians(pitch));
			cam_front_in.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
			cam_front = glm::normalize(cam_front_in);
			// calculate position front vector that doesn't have pitch included;
			glm::vec3 pos_front_in;
			pos_front_in.x = cos(glm::radians(yaw)) * cos(glm::radians(0.0f));
			pos_front_in.y = sin(glm::radians(0.0f));
			pos_front_in.z = sin(glm::radians(yaw)) * cos(glm::radians(0.0f));
			pos_front = glm::normalize(pos_front_in);
			// also re-calculate the right and up vector
			right = glm::normalize(glm::cross(cam_front, world_up));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
			up    = glm::normalize(glm::cross(right, cam_front));
		}
};
#endif
