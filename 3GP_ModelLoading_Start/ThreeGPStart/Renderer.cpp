#include "Renderer.h"
#include "ImageLoader.h"
#include "Object.h"
#include "ObjectTerrain.h"
#include "ObjectSkybox.h"
#include "MyMesh.h"

// On exit must clean up any OpenGL resources e.g. the program, the buffers
Renderer::~Renderer()
{
	for (int i = 0; i < allObjects.size(); i++)
	{
		delete allObjects[i];
	}

	glDeleteProgram(m_program);
	glDeleteBuffers(1, &m_VAO);

}

// Load, compile and link the shaders and create a program object to host them
bool Renderer::CreateProgram()
{
	// Create a new program (returns a unqiue id)
	m_program = glCreateProgram();

	// Load and create vertex and fragment shaders
	GLuint vertex_shader{ Helpers::LoadAndCompileShader(GL_VERTEX_SHADER, "Data/Shaders/vertex_shader.glsl") };
	GLuint fragment_shader{ Helpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, "Data/Shaders/fragment_shader.glsl") };
	if (vertex_shader == 0 || fragment_shader == 0)
		return false;

	// Attach the vertex shader to this program (copies it)
	glAttachShader(m_program, vertex_shader);

	// The attibute 0 maps to the input stream "vertex_position" in the vertex shader
	// Not needed if you use (location=0) in the vertex shader itself
	//glBindAttribLocation(m_program, 0, "vertex_position");

	// Attach the fragment shader (copies it)
	glAttachShader(m_program, fragment_shader);

	// Done with the originals of these as we have made copies
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	// Link the shaders, checking for errors
	if (!Helpers::LinkProgramShaders(m_program))
		return false;

	return !Helpers::CheckForGLError();
}

// Load / create geometry into OpenGL buffers	
bool Renderer::InitialiseGeometry()
{
	// Load and compile shaders into m_program
	if (!CreateProgram())
	{
		return false;
	}


	//Skybox --------------------------------------------------------------
	ObjectSkybox* skybox = new ObjectSkybox("Data\\Sky\\Clouds\\skybox.x");

	if (!skybox->Initialise())
	{
		std::cout << "Error initialising Skybox" << std::endl;
	}

	//Add texture not required as extracts filename from mesh.

	allObjects.push_back(skybox);

	//Jeep --------------------------------------------------------------

	Object* jeep = new Object("Data\\Models\\Jeep\\jeep.obj", 155, 15, 150, 0.01f);

	jeep->AddTexture("Data\\Models\\Jeep\\jeep_rood.jpg");

	//Can remove the Add Texture function by implemnting similar to skybox method. 

	if (!jeep->Initialise())
	{
		std::cout << "Error Initialising Object" << std::endl;
		return false;
	}

	allObjects.push_back(jeep);

	//Trees------------------------------------------------------------------------------

	//NOTE DID NOT SEED RANDOM BUT WORKS FOR PURPOSES OF THIS DEMO.

	//Generation of number of trees, found any more than 5 likely to crash. Randomly scattered through terrain.
	for (int i = 0; i < kMaxNumberTrees; i++)
	{
		Object* tree = new Object("Data\\Models\\Tree\\Tree_OBJ.obj", static_cast <float> ( std::rand() % 320) , 0, static_cast <float> ( std::rand() % 320) , 0.01f);
		tree->AddTexture("Data\\Models\\Tree\\trunk.jpg");

		if (!tree->Initialise())
		{
			std::cout << "Error Initialising Tree" << std::endl;
			return false;
		}

		allObjects.push_back(tree);
	}

	//Rock
	//Generation of number of rocks and randomly scattered through terrain.
	for (int i = 0; i < kMaxNumberRocks; i++)
	{
		Object* rock = new Object("Data\\Models\\Rock\\rockkk.obj", static_cast <float> (std::rand() % 320), 10, static_cast <float> (std::rand() % 320), 10);
		rock->AddTexture("Data\\Models\\Rock\\Mountain brown rock texture 4770x3178.jpg");

		if (!rock->Initialise())
		{
			std::cout << "Error Initialising Rock" << std::endl;
			return false;
		}

		allObjects.push_back(rock);
	}
	

	//Terrain-------------------------------------------------------------


	ObjectTerrain* terrain = new ObjectTerrain(32, 32, 0, 0 );

	terrain->AddTexture("Data\\Textures\\dirt_earth-n-moss_df_.dds");

	if (!terrain->Initialise())
	{
		std::cout << "Error Initialising Terrain" << std::endl;
		return false;
	}

	allObjects.push_back(terrain);

	
	
	return true;

}

// Render the scene. Passed the delta time since last called.
void Renderer::Render(const Helpers::Camera& camera, float deltaTime)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Uncomment to render in wireframe (can be useful when debugging)
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	// Clear buffers from previous frame
	glClearColor(0.0f, 0.0f, 0.0f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Compute viewport and projection matrix
	GLint viewportSize[4];
	glGetIntegerv(GL_VIEWPORT, viewportSize);
	const float aspect_ratio = viewportSize[2] / (float)viewportSize[3];
	glm::mat4 projection_xform = glm::perspective(glm::radians(45.0f), aspect_ratio, 0.1f, 10000.0f);

	// Compute camera view matrix and combine with projection matrix for passing to shader
	glm::mat4 view_xform = glm::lookAt(camera.GetPosition(), camera.GetPosition() + camera.GetLookVector(), camera.GetUpVector());
	

	// Use our program. Doing this enables the shaders we attached previously.
	glUseProgram(m_program);
	Helpers::CheckForGLError();

	for (auto& p : allObjects)
	{
		
		p->Render(camera, m_program, projection_xform, view_xform);
	}
}

void Renderer::MoveObject(const float& x, const float& y, const float& z)
{
	//allObjects position 1 is the main player jeep. Hardcoded for testing but if time would create reference pointer.
	allObjects[1]->Move(x, y, z);
}


