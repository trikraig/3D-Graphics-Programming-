#include "ExternalLibraryHeaders.h"
#include "Mesh.h"
#include "glm/glm.hpp"
#include "ImageLoader.h"
#include "Object.h"
#include "MyMesh.h"

Object::Object(const std::string& modelFileName, const float& posX, const float& posY, const float& posZ, const float& scale) : m_modelFilename(modelFileName), m_posX(posX), m_posY(posY), m_posZ(posZ), m_scale(scale)
{
}

bool Object::Initialise()
{
	Helpers::ModelLoader loader;
	Helpers::ImageLoader imageLoader;

	//Loads model from file.
	if (!loader.LoadFromFile(m_modelFilename)) {

		//Error
		return false;
	}

	//Loops through each mesh per object and creates VAO
	for (const Helpers::Mesh& mesh : loader.GetMeshVector())
	{
		MyMesh newMesh;

		//Vertices / Positions extraction from object file.
		GLuint positionsVBO;
		glGenBuffers(1, &positionsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//ELEMENTS extraction from object file.
		GLuint elementsEBO;
		glGenBuffers(1, &elementsEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh.elements.size(), mesh.elements.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//NORMALS extraction from object file.
		GLuint normalsVBO;
		glGenBuffers(1, &normalsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.normals.size(), mesh.normals.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//texCoordinates extraction from object file.
		GLuint uvVBO;
		glGenBuffers(1, &uvVBO);
		glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * mesh.uvCoords.size(), mesh.uvCoords.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Gets corresponding texture name from vector and loads using imageLoader, alternative method used for skybox as seen below to get correct filename. 
		//loader.GetMaterialVector()[mesh.materialIndex].diffuseTextureFilename)

		if (!imageLoader.Load(m_TexNameVector[counter]))
		{
			//Error
			return false;
		}

		counter++;

		//Creating Texture Object
		glGenTextures(1, &newMesh.m_texture);
		glBindTexture(GL_TEXTURE_2D, newMesh.m_texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageLoader.Width(), imageLoader.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imageLoader.GetData());
		glGenerateMipmap(GL_TEXTURE_2D);

		Helpers::CheckForGLError();

		newMesh.m_numElements = static_cast <int> (mesh.elements.size());

		//Generation of VAOs

		glGenVertexArrays(1, &newMesh.m_VAO);
		glBindVertexArray(newMesh.m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(
			0,                  // attribute 0
			3,                  // size in comp of each item in the stream - POSITIONS ARE VEC 3
			GL_FLOAT,           // type of the item
			GL_FALSE,           // normalized or not (advanced)
			0,                  // stride (advanced)
			(void*)0            // array buffer offset (advanced)
		);

		glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(
			1,                  // attribute 1
			3,                  // size in comp of each item in the stream - NORMALS ARE VEC 3
			GL_FLOAT,           // type of the item
			GL_FALSE,           // normalized or not (advanced)
			0,                  // stride (advanced)
			(void*)0            // array buffer offset (advanced)
		);


		glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(
			2,                  // attribute 2
			2,                  // size in comp of each item in the stream -TEXTURE COORDINATES ARE VEC 2.
			GL_FLOAT,           // type of the item
			GL_FALSE,           // normalized or not (advanced)
			0,                  // stride (advanced)
			(void*)0            // array buffer offset (advanced)
		);

		//Keep elements at the end. 
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);
		glBindVertexArray(0);
		m_MeshVector.push_back(newMesh);

	}


	// Good idea to check for an error now:	
	Helpers::CheckForGLError();

	// Clear VAO binding
	glBindVertexArray(0);

	return true;
}

void Object::Render(const Helpers::Camera& camera, GLuint m_program, glm::mat4& projection_xform, glm::mat4& view_xform)
{
	//Calculation of combined xform outside of mesh loop for performance enhancement.
	glm::mat4 combined_xform = projection_xform * view_xform;

	for (MyMesh& currentMesh : m_MeshVector)
	{
		// Configure pipeline settings		
		// Send the combined matrix to the shader in a uniform
		GLuint combined_xform_id = glGetUniformLocation(m_program, "combined_xform");

		glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));

		//glm::mat4 model_xform = glm::mat4(1);
		glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(m_scale, m_scale, m_scale));
		glm::mat4 transform = glm::translate(glm::mat4(1.0), glm::vec3(m_posX, m_posY, m_posZ));

		glm::mat4 model_xform = transform * scale;

		//Texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, currentMesh.m_texture);
		GLuint sampler_id = glGetUniformLocation(m_program, "myTexture");
		glUniform1i(sampler_id, 0);

		// Send the model matrix to the shader in a uniform
		GLuint model_xform_id = glGetUniformLocation(m_program, "model_xform");
		glUniformMatrix4fv(model_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));
		Helpers::CheckForGLError();


		// Bind our VAO and render
		glBindVertexArray(currentMesh.m_VAO);
		glDrawElements(GL_TRIANGLES, currentMesh.m_numElements, GL_UNSIGNED_INT, (void*)0);

		// Always a good idea, when debugging at least, to check for GL errors
		Helpers::CheckForGLError();
	}
}


void Object::AddTexture(const std::string& filename)
{
	m_TexNameVector.push_back(filename);
	return;
}

void Object::Move(const float& x, const float& y, const float& z)
{
	m_posX += x;
	m_posY += y;
	m_posZ += z;

	return;
}

glm::vec3 Object::GetTranslation()
{

	return glm::vec3(m_posX, m_posY, m_posZ);
}
