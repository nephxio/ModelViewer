#version 450

layout(location = 0) in vec3 position;

layout(location = 0) out vec3 fragColor;

vec3 triangle_colors[4] = vec3[](
	vec3(1.0, 0.0, 0.0),
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 0.0, 1.0),
	vec3(1.0, 0.0, 1.0)
);

layout (push_constant) uniform Push
{
	mat4 viewProjection;
	mat4 transform;
} push;

void main()
{
	gl_Position = push.viewProjection *  push.transform * vec4(position, 1.0);
	fragColor = triangle_colors[gl_VertexIndex % 4];
}