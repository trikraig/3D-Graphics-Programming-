#pragma once
#include "Object.h"
class ObjectSkybox : public Object
{
public: 
	
	ObjectSkybox(const std::string& modelFileName);
	bool Initialise() override final;
	void Render(const Helpers::Camera& camera, GLuint m_program, glm::mat4& projection_xform, glm::mat4& view_xform) override final;

};

