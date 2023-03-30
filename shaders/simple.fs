#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D diffuse_map;

uniform vec3 camera;
uniform vec3 lightPos;

void main()
{
	vec3 color = texture(diffuse_map, TexCoords).rgb;
	
	vec3 V = normalize(camera - FragPos);
	vec3 L = normalize(lightPos - FragPos);
	vec3 H = normalize(V + L);
	vec3 R = reflect(-L, Normal);
	
	vec3 ambient = 0.05 * color;
	
	float diff = max(dot(L, Normal), 0.0);
	vec3 diffuse = diff * color;
	
	float spec = pow(max(dot(Normal, H), 0.0), 32.0);
	vec3 specular = vec3(0.3) * spec;
	
	FragColor = vec4(ambient + diffuse + specular, 1.0);
}
