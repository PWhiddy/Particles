#version 420 core


// Uniforms 

out vec4 color;

uniform float time;
uniform vec2 resolution;
uniform vec2 cameraRotation;
uniform vec3 cameraPos;


/////////


//// Math Utility ////

vec2 rot2D(vec2 p, float angle) 
{
    float s = sin(angle);
    float c = cos(angle);
    return p * mat2(c,s,-s,c);
}

void main()
{
	
	
	//color = vec4(color1*mix(dot(normal, ray.direction+0.2), 1.0, 0.7), 1.0); 
	//color = vec4(objects[1].color, 1.0);
	color = vec4(sin(20.0*gl_FragCoord.x/resolution.x+10.0*time)*0.5+0.5, 0.8, 0.0, 1.0);
}