#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;

//layout(location = 0) out vec3 fragColor;

layout( push_constant ) uniform constants{
	vec2 offset;
	vec3 color;
} PushConstants;

void main(){
	gl_Position = vec4(position + PushConstants.offset, 0.0, 1.0);
	//fragColor = push.color;
}