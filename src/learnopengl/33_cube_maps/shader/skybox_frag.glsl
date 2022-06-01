#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform vec3 eyepos;
void main()
{    
    FragColor = texture(skybox, normalize(TexCoords-eyepos));
}