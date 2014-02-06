#version 130

in vec3 fnorm;
in vec2 ftex;

void main()
{
	vec3 normal = normalize(fnorm);
	vec3 lightDir = vec3(1, 0, 0);
	float light = dot(normal, lightDir);
	vec3 diffuse = vec3(1,1,1) * clamp(light, 0, 1);
	gl_FragColor.rgb = diffuse;
	gl_FragColor.a = 1.0;
}
