#include "Simulation.h"
#include "Renderer.h"

// Initialise this as well as the renderer, returns false on error
bool Simulation::Initialise()
{
	// Set up camera
	m_camera = std::make_shared<Helpers::Camera>();
	m_camera->Initialise(glm::vec3(46, 34, 120), glm::vec3(0)); // Jeep

	// Set up renderer
	m_renderer = std::make_shared<Renderer>();

	m_renderer->InitialiseGeometry();

	

	return true;
}

// Handle any user input. Return false if program should close.
bool Simulation::HandleInput(GLFWwindow* window)
{

	//Input for player movement of single jeep. Uses arrow keys.
	//Can go forward, backward, left and right.

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)// means UP key pressed
	{
		player.posX = movementStep;
	}
	
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)// means DOWN key pressed
	{
		player.posX = -movementStep;
	}
	else
	{
		player.posX = 0;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)// means LEFT key pressed
	{
		player.posZ = -movementStep;
	}
	
	else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)// means RIGHT key pressed
	{
		player.posZ = movementStep;
	}
	else
	{
		player.posZ = 0;
	}

	// You can get mouse button input, returned state is GLFW_PRESS or GLFW_RELEASE
	// int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

	// Use this to get the mouse position:
	// double xpos, ypos;
	// glfwGetCursorPos(window, &xpos, &ypos);

	// To prevent the mouse leaving the window (and to hide the arrow) you can call:
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// To reenable it use GLFW_CURSOR_NORMAL

	// To see an example of input using GLFW see the camera.cpp file.
	return true;
}

// Update the simulation (and render) returns false if program should close
bool Simulation::Update(GLFWwindow* window)
{
	// Deal with any input
	if (!HandleInput(window))
		return false;

	// Calculate delta time since last called
	// We pass the delta time to the camera and renderer
	float timeNow = (float)glfwGetTime();
	float deltaTime{ timeNow - m_lastTime };
	m_lastTime = timeNow;

	// The camera needs updating to handle user input internally
	m_camera->Update(window, deltaTime);

	//Moving Jeep Object - Player
	m_renderer->MoveObject(player.posX, player.posY, player.posZ);
	

	// Render the scene
	m_renderer->Render(*m_camera, deltaTime);

	return true;
}
