#pragma once

#include "ExternalLibraryHeaders.h"
#include "Helper.h"
#include "Mesh.h"
#include "Camera.h"
#include "glm/glm.hpp"
#include <string>

class Object;

class Renderer
{
private:
	// Program object - to host shaders
	GLuint m_program{ 0 };

	// Vertex Array Object to wrap all render settings
	GLuint m_VAO{ 0 };

	// Number of elments to use when rendering
	GLuint m_numElements{ 0 };

	std::vector <Object*> allObjects;

	const int kMaxNumberTrees{ 5 };

	const int kMaxNumberRocks{ 5 };
		
	bool CreateProgram();


public:
	Renderer()=default;
	~Renderer();

	// Create and / or load geometry, this is like 'level load'
	bool InitialiseGeometry();

	// Render the scene
	void Render(const Helpers::Camera& camera, float deltaTime);

	void MoveObject(const float& x, const float& y, const float& z);

	
};

