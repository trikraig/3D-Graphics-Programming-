#include "ObjectSkybox.h"
#include "MyMesh.h"
#include "Mesh.h"
#include "ImageLoader.h"


ObjectSkybox::ObjectSkybox(const std::string& modelFileName) : Object(modelFileName, 0, 0, 0, 1)
{
}

bool ObjectSkybox::Initialise()
{
	Helpers::ModelLoader loader;
	Helpers::ImageLoader imageLoader;

	int counter = 0;

	if (!loader.LoadFromFile(m_modelFilename)) {

		//Error
		return false;
	}



	for (const Helpers::Mesh& mesh : loader.GetMeshVector())
	{
		MyMesh newMesh;

		//Override the normals from the file to make sure that lighting is not applied in shader. In better circumstances would have own shader.

		std::vector <glm::vec3> normals(mesh.vertices.size());

		std::fill(normals.begin(), normals.end(), glm::vec3(1, 1, 1));

		GLuint positionsVBO;
		//POSITIONS
		glGenBuffers(1, &positionsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//ELEMENTS SETUP
		GLuint elementsEBO;
		glGenBuffers(1, &elementsEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh.elements.size(), mesh.elements.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//NORMALS SETUP
		GLuint normalsVBO;
		glGenBuffers(1, &normalsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//texCoordinates
		GLuint uvVBO;
		glGenBuffers(1, &uvVBO);
		glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * mesh.uvCoords.size(), mesh.uvCoords.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Hard coded filename for skybox location, would ordinarily pass in.
		if (!imageLoader.Load("Data\\Sky\\Clouds\\" + loader.GetMaterialVector()[mesh.materialIndex].diffuseTextureFilename))
		{
			//Error
			return false;
		}

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

		//VAO

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

void ObjectSkybox::Render(const Helpers::Camera& camera, GLuint m_program, glm::mat4& projection_xform, glm::mat4& view_xform)
{
	//Alternative view xform to "place skybox on viewers head"
	//Disabled Depthmask and test for seamless edges then renabled at end of function.


	for (MyMesh& currentMesh : m_MeshVector)
	{

		glm::mat4 view_xform2 = glm::mat4(glm::mat3(view_xform));

		glm::mat4 combined_xform = projection_xform * view_xform2;

		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


		// Configure pipeline settings		
		// Send the combined matrix to the shader in a uniform
		GLuint combined_xform_id = glGetUniformLocation(m_program, "combined_xform");

		glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));

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

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);

		// Always a good idea, when debugging at least, to check for GL errors
		Helpers::CheckForGLError();
	}


}


