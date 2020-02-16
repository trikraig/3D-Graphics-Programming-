#version 330

uniform mat4 combined_xform;
uniform mat4 model_xform;

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normals;
layout(location = 2) in vec2 uv_co;

out vec2 varying_texcoord;
out vec3 varying_normal;
out vec3 varying_position;
out int varying_skybox;

void main(void)
{	

	varying_texcoord = uv_co;

	//Converts it to world space by multiplying by the model transform.
	varying_normal = mat3(model_xform) * vertex_normals;

	varying_position = mat4x3(model_xform) * vec4(vertex_position,1.0);
	
	gl_Position = combined_xform * model_xform * vec4(vertex_position, 1.0);

}