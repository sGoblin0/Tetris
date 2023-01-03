#version 330 core
layout (location = 0) in vec3 aPos; //posi�ao
layout (location = 1) in vec2 aTexCoord; //coordenadas das texturas

out vec2 TexCoord;

//mvp
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
	TexCoord = aTexCoord;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
