#version 130

in vec3 fnorm;
in vec3 fpos;
in vec2 ftex;

uniform vec3 camera_pos;
uniform sampler2D tex0;

void main()
{
	vec3 normal = normalize(fnorm);
	float light;

	vec3 diffuseDir = vec3(1, 0, 0);
	light = dot(normal, diffuseDir);
	float diffuse = clamp(light, 0, 1);

	vec3 specularDir = vec3(1, 0, 0);
	vec3 to_camera = normalize(camera_pos - fpos);
	vec3 specularRef = reflect(-specularDir, normal);
	light = dot(specularRef, to_camera);
	light = clamp(light, 0, 1);

	light = 0.1 + diffuse + pow(light, 16);

	gl_FragColor.rgb = 0.05 + light * texture2D(tex0, ftex).rgb;
	gl_FragColor.a = 1.0;
}
