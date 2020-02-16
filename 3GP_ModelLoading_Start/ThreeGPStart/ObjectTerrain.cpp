#include "ObjectTerrain.h"
#include "ImageLoader.h"
#include "MyMesh.h"

ObjectTerrain::ObjectTerrain(int numCellsX, int numCellsZ, float sizeX, float sizeZ) : Object("", 0, 0, 0, 1)
{
	m_numCellsX = numCellsX;
	m_numCellsZ = numCellsZ;
	m_sizeX = sizeX;
	m_sizeZ = sizeZ;
}

bool ObjectTerrain::Initialise()
{
	//Generation of terrain.

	MyMesh terrainMesh;

	int maxHeight{ 5 };
	float heightScale{ 0.1f };
	float terrainScale{ 10 };
	int numberOfVertsInX = m_numCellsX + 1;
	int numberOfVertsInZ = m_numCellsZ + 1;

	Helpers::ImageLoader imageLoader;

	if (!imageLoader.Load("Data\\Textures\\curvy.gif"))
	{
		return false;
	}

	float vertexXtoImage = (float)imageLoader.Width() / numberOfVertsInX;
	float vertexZtoImage = (float)imageLoader.Height() / numberOfVertsInZ;

	GLbyte* imageData = imageLoader.GetData();

	//Vertices Generation - Specific to terrain as self generating mesh.
	std::vector <glm::vec3> vertices;

	//Generation of mesh for terrain in diamond pattern.
	//Leaving height at 0 for now to be added later.
	int height{ 0 };
	for (int x = 0; x < numberOfVertsInX; x++)
	{
		for (int z = 0; z < numberOfVertsInZ; z++)
		{

			int imageX = static_cast <int> (vertexXtoImage) * x;
			int imageZ = static_cast <int> (vertexZtoImage) * z;

			size_t offset = ((size_t)imageX + (size_t)imageZ * imageLoader.Width()) * 4;
			BYTE height = imageData[offset];

			glm::vec3 pos;

			pos.x = x * terrainScale;
			pos.y = height * heightScale;

			pos.z = z * terrainScale;

			//Applied noise to height to "smooth" out edges a bit.
			pos.y += KenPerlin(pos.x, pos.z);

			vertices.push_back(pos);
		}
	}

	bool toggleForDiamondPattern = true;


	//Elements Generation - Specific to terrain as self generating mesh.
	std::vector <GLint> elements;
	for (int z = 0; z < m_numCellsZ; z++)
	{
		for (int x = 0; x < m_numCellsX; x++)
		{

			int startVertIndex = z * numberOfVertsInX + x;

			if (toggleForDiamondPattern)
			{
				elements.push_back(startVertIndex);
				elements.push_back(startVertIndex + 1);
				elements.push_back(startVertIndex + numberOfVertsInX);

				elements.push_back(startVertIndex + 1);
				elements.push_back(startVertIndex + numberOfVertsInX + 1);
				elements.push_back(startVertIndex + numberOfVertsInX);
			}
			else
			{
				//other side of pattern
				elements.push_back(startVertIndex + 1);
				elements.push_back(startVertIndex + numberOfVertsInX + 1);
				elements.push_back(startVertIndex);

				elements.push_back(startVertIndex + numberOfVertsInX + 1);
				elements.push_back(startVertIndex + numberOfVertsInX);
				elements.push_back(startVertIndex);

			}

			toggleForDiamondPattern = !toggleForDiamondPattern;
		}
		if ((m_numCellsX * m_numCellsZ) % 2 == 0)
		{
			toggleForDiamondPattern = !toggleForDiamondPattern;
		}
	}


	//Normals generation - Specific to terrain as self generating mesh.

	std::vector <glm::vec3> normals(vertices.size());

	std::fill(normals.begin(), normals.end(), glm::vec3(0, 0, 0));

	for (size_t elementIndex = 0; elementIndex < elements.size(); elementIndex += 3)
	{
		int index1 = elements[elementIndex + 0];
		int index2 = elements[elementIndex + 1];
		int index3 = elements[elementIndex + 2];

		glm::vec3 v0 = vertices[index1];
		glm::vec3 v1 = vertices[index2];
		glm::vec3 v2 = vertices[index3];

		glm::vec3 edge1 = v1 - v0;
		glm::vec3 edge2 = v2 - v0;

		glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

		normals[index1] += normal;
		normals[index2] += normal;
		normals[index3] += normal;


	}

	////ReNormalize Normals after calculation to make sure normalized.

	for (glm::vec3& normal : normals)
	{
		normal = glm::normalize(normal);
	}


	//Texture Coordinates Generation
	std::vector <GLfloat> uvC;

	for (float u = 0; u < m_numCellsX + 1; u++)
	{
		for (float v = 0; v < m_numCellsZ + 1; v++)
		{
			uvC.push_back(u / m_numCellsX);
			uvC.push_back(v / m_numCellsZ);
		}
	}



	//Creation of VBOs 

	GLuint positionsVBO;
	//POSITIONS
	glGenBuffers(1, &positionsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//ELEMENTS SETUP
	GLuint elementsEBO;
	glGenBuffers(1, &elementsEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * elements.size(), elements.data(), GL_STATIC_DRAW);
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * uvC.size(), uvC.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Creating Texture Object

	if (!imageLoader.Load(m_TexNameVector[0]))
	{
		//Error
		return false;
	}

	glBindTexture(GL_TEXTURE_2D, terrainMesh.m_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageLoader.Width(), imageLoader.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imageLoader.GetData());
	glGenerateMipmap(GL_TEXTURE_2D);


	terrainMesh.m_numElements = elements.size();

	//VAO creation.

	glGenVertexArrays(1, &terrainMesh.m_VAO);
	glBindVertexArray(terrainMesh.m_VAO);

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

	m_MeshVector.push_back(terrainMesh);




	return true;
}


float ObjectTerrain::CosineLerp(float a, float b, float x)
{
	float ft = x * 3.1415927f;
	float f = (1.0f - cos(ft)) * 0.5f;
	return a * (1.0f - f) + b * f;
}

float ObjectTerrain::Noise(int x, int y)
{
	int n = x + y * 57; // 57 is the seed – can be tweaked
	n = (n >> 13) ^ n;
	int nn = (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
	return 1.0f - ((float)nn / 1073741824.0f);
}

float ObjectTerrain::KenPerlin(float xPos, float zPos)
{
	float s = Noise((int)xPos, (int)zPos);
	float t = Noise((int)xPos + 1, (int)zPos);
	float u = Noise((int)xPos, (int)zPos + 1);
	float v = Noise((int)xPos + 1, (int)zPos + 1);
	float c1 = CosineLerp(s, t, xPos);
	float c2 = CosineLerp(u, v, xPos);
	return CosineLerp(c1, c2, zPos);
}


