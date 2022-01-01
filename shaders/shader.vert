#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 uv;

layout (location = 0) out vec3 fragColor;

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

    vec3 normal_world_space = normalize(mat3(push.normal_matrix) * normal);
    vec3 directionToLight = ubo.lightPosition.xyz - position.xyz;

    float attenuation = 1.0 / dot(directionToLight, directionToLight);

    vec3 lightColor = ubo.lightColor.xyz * ubo.lightColor.w * attenuation;
    vec3 ambientLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
    vec3 diffuseLight = lightColor * max(dot(normal_world_space, normalize(directionToLight)), 0);

    fragColor = (diffuseLight + ambientLight) * color;
}