#version 130

in vec3 color;

void main()
{
	gl_FragColor.rgb = color;
	gl_FragColor.a = 1.0;
}
