#version 330

uniform sampler2D myTexture;

in vec3 varying_normal;
in vec2 varying_texcoord;
in vec3 varying_position;

out vec4 fragment_colour;

void main(void)
{
	
	//POINT LIGHT
	
	vec3 light_direction = vec3 (-0.9,-0.1,0);

	//SPOT LIGHT
//
//	vec3 light_position = vec3 (400,50,0);
//
//	vec3 P = varying_position;
//
//	vec3 light_direction = P - light_position;

	vec3 L = normalize(-light_direction);
	vec3 N = normalize(varying_normal);

	//Clamp to ensure doesnt become negative.
	float diffuse_intesity = max(0,dot(L,N));

	vec3 ambient_light = vec3(2);
	vec3 tex_colour = texture(myTexture, varying_texcoord).rgb;
	vec3 final_colour = (tex_colour * diffuse_intesity) * ambient_light ;

	fragment_colour = vec4 (final_colour,1.0);

}


