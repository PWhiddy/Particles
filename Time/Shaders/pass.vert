#version 420 core
layout (location = 0) in vec3 position;

uniform vec2 resolution;

void main()
{
	gl_PointSize = 4.0;
	float aspect = resolution.x/resolution.y;
	float scale = 0.5;
	gl_Position = vec4(scale*position.x/aspect, scale*position.y, position.z, 1.0);
}