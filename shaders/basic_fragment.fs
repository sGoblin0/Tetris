#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;

//cor
uniform vec3 color;

void main()
{
    FragColor = texture(texture1, TexCoord) * vec4(color, 1.0); 
}
