#version 130

//layout (location = 0) in vec3 pos;
in vec3 pos;
uniform mat4 MVP;

void main()
{
	vec4 v = vec4(pos, 1.0);
	gl_Position = MVP * v;
}
