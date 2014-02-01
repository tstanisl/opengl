#version 130

//layout (location = 0) in vec3 pos;
in vec3 pos;
uniform mat4 MVP;
out vec3 color;

void main()
{
	color = 0.5 * (1 + pos);
	vec4 v = vec4(pos, 1.0);
	gl_Position = MVP * v;
}
