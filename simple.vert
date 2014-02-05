#version 130

//layout (location = 0) in vec3 pos;
in vec3 position;
in vec2 texcoord;
in vec3 normal;

uniform mat4 MVP;
out vec3 color;

void main()
{
	vec4 mvp_normal = MVP * vec4(normal, 0.0);
	color = vec3(1,1,1) * clamp(mvp_normal.y, 0, 1);
	color += 0.05;
	vec4 v = vec4(position, 1.0);
	gl_Position = MVP * v;
}
