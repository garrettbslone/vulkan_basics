#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 uv;

layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec3 fragWorldPos;
layout (location = 2) out vec3 fragNormalWorld;

layout (set = 0, binding = 0) uniform global_ubo {
    mat4 projectionViewMatrix;
    vec4 ambientLightColor;
    vec4 lightPosition;
    vec4 lightColor;
} ubo;

layout (push_constant) uniform Push {
    mat4 model_matrix;
    mat4 normal_matrix;
} push;

void main() {
    vec4 positionWorld = push.model_matrix * vec4(position, 1.f);
    gl_Position = ubo.projectionViewMatrix * positionWorld;

    fragNormalWorld = normalize(mat3(push.normal_matrix) * normal);
    fragWorldPos = positionWorld.xyz;
    fragColor = color;
}