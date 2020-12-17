#version 130

in vec3 fnorm;
in vec3 fpos;
in vec2 ftex;
in vec3 ftan;

uniform vec3 camera_pos;
uniform sampler2D tex0;
uniform sampler2D normal0;

void main()
{
	vec3 btan = cross(fnorm, ftan);
	mat3 NM = mat3(ftan, fnorm, -btan);

	vec3 tnormal = texture(normal0, 16 * ftex).xyz - 0.5;
	vec3 normal = normalize(fnorm + 1.3 * NM * tnormal);
	normal = normalize(normal);

	float light;

	vec3 diffuseDir = vec3(1, 0, 0);
	light = dot(normal, diffuseDir);
	float diffuse = clamp(light, 0, 1);

	/*normal = fnorm + 0.8 * NM * texture(normal0, 16 * ftex).xyz;
	normal = normalize(normal);*/

	vec3 specularDir = vec3(1, 0, 0);
	vec3 to_camera = normalize(camera_pos - fpos);
	vec3 specularRef = reflect(-specularDir, normal);
	light = dot(specularRef, to_camera);
	light = clamp(light, 0, 1);

	light = 0.1 + diffuse + pow(light, 16);

	vec3 color = 0.05 + light * texture(tex0, ftex).rgb;
	//gl_FragColor.rgb = 0.5 + 0.5 * normalize(ftan) + 0.001 * color;
	gl_FragColor.rgb = color;
	gl_FragColor.a = diffuse;
}
