#pragma once
#include <string>
#include <vector>
#include "Helper.h"
#include "Camera.h"

class MyMesh;

class Object
{
protected:

	std::string m_modelFilename;
	std::string m_textureFilename;

	float m_posX{ 0 }, m_posY{ 0 }, m_posZ{ 0 };
	float m_scale{ 0 };

	std::vector <MyMesh> m_MeshVector;

	std::vector <std::string> m_TexNameVector;

	//Assists aligning mesh to correct texture. Skybox works via alternative
	int counter = 0;

public:

	Object(const std::string& modelFileName, const float& posX, const float& posY, const float& posZ, const float& scale);

	virtual bool Initialise();

	virtual void Render(const Helpers::Camera& camera, GLuint m_program, glm::mat4& projection_xform, glm::mat4& view_xform);

	void AddTexture(const std::string& filename);

	void Move(const float& x, const float& y, const float& z);

	glm::vec3 GetTranslation();

};

