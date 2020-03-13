#version 150 core 

uniform mat4 uModelViewProjectionMatrix;

in vec4 aPosition;
in vec4 aColor;
out vec4 vColor;

void main()
{
	gl_Position = uModelViewProjectionMatrix * aPosition;
	vColor = aColor;
}