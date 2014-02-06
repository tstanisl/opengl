#version 130

in vec3 fnorm;
in vec3 fpos;
in vec2 ftex;

uniform vec3 camera_pos;

void main()
{
	vec3 normal = normalize(fnorm);
	float light;

	vec3 diffuseDir = vec3(1, 0, 0);
	light = dot(normal, diffuseDir);
	vec3 diffuse = vec3(1,1,1) * clamp(light, 0, 1);

	vec3 specularDir = vec3(1, 0, 0);
	vec3 to_camera = normalize(camera_pos - fpos);
	vec3 specularRef = reflect(specularDir, normal);
	light = dot(specularRef, to_camera);
	light = clamp(light, 0, 1);

	gl_FragColor.rgb = fpos * (diffuse + vec3(1,1,0) * pow(light, 8));
	gl_FragColor.a = 1.0;
}
