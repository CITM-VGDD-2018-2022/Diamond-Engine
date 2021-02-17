#ifdef vertex
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normals;
layout (location = 3) in vec3 tangents;

out vec3 ourColor;
out vec2 TexCoord;
out vec3 Normal;
out vec3 fPosition;

uniform mat4 model_matrix;
uniform mat4 view;
uniform mat4 projection;

uniform float time;

void main()
{
Normal = mat3(transpose(inverse(model_matrix))) * normals;

vec4 pos = model_matrix * vec4(position, 1.0);
fPosition = pos.xyz;

gl_Position = projection * view * model_matrix * vec4(position, 1.0f);

ourColor = vec3(1.0, 1.0, 1.0);
TexCoord = texCoord;
}
#endif

#ifdef fragment
#version 330 core
in vec3 ourColor;
in vec2 TexCoord;
out vec4 color;

in vec3 fPosition;
in vec3 Normal;

uniform sampler2D diffuseTexture;
uniform sampler2D normalMap;
uniform vec3 light;
uniform int hasTexture;
uniform vec3 altColor;

uniform float time;

vec2 blinnPhongDir(vec3 lightDir, float lightInt, float Ka, float Kd, float Ks, float shininess)
{
vec3 s = normalize(lightDir);
vec3 v = normalize(-fPosition);
vec3 n = normalize(Normal);
vec3 h = normalize(v+s);
float diffuse = Ka + Kd * lightInt * max(0.0, dot(n, s));
float spec = Ks * pow(max(0.0, dot(n,h)), shininess);
return vec2(diffuse, spec);
}

void main()
{
vec3 lcolor = vec3(1.0,1.0,1.0);
vec2 inten = blinnPhongDir(light, 0.5, 0.2, 0.8, 0.3, 80.0);
color = (hasTexture == 1) ? vec4(lcolor * inten.x + vec3(1.0) * inten.y, 1.0) * texture(diffuseTexture, TexCoord) * vec4(altColor, 1.0) : vec4(altColor, 1.0);
color.g = texture(normalMap, TexCoord).r;
}
#endif

















































