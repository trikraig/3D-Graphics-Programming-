#pragma once

#include "ExternalLibraryHeaders.h"
#include "Camera.h"

class Renderer;
struct GLFWwindow;

struct Object
{
	float posX{ 0 }, posY{ 0 }, posZ{ 0 };
};

// Simulation class to handle input, updating of the simulation and calling the renderer
class Simulation
{
private:
	// A simple camera
	std::shared_ptr<Helpers::Camera> m_camera;

	// The renderer
	std::shared_ptr<Renderer> m_renderer;

	// Remember last update time so we can calculate delta time
	float m_lastTime{ 0 };

	// Handle any user input. Return false if program should close.
	bool HandleInput(GLFWwindow* window);

	Object player;
	//Amount player object moves on each key press.
	float movementStep{ 0.1f };

public:
	// Initialise this as well as the renderer, returns false on error
	bool Initialise();	

	// Update the simulation (and render) returns false if program should clse
	bool Update(GLFWwindow* window);
};

