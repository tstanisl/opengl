#version 130

//layout (location = 0) in vec3 pos;
in vec3 vpos;
in vec2 vtex;
in vec3 vnorm;
in vec3 vtan;

uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 M;

out vec3 fnorm;
out vec2 ftex;
out vec3 fpos;
out vec3 ftan;

void main()
{
	ftex = vtex;
	ftan = (M * vec4(vtan, 0)).xyz;
	fnorm = (M * vec4(vnorm, 0.0)).xyz;
	fpos = (M * vec4(vpos, 1.0)).xyz;
	gl_Position = MVP * vec4(vpos, 1.0);
	
}
