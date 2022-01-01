#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 uv;

layout (location = 0) out vec3 fragColor;

layout (set = 0, binding = 0) uniform global_ubo {
    mat4 projectionViewMatrix;
    vec3 directionToLight;
} ubo;

layout (push_constant) uniform Push {
    mat4 model_matrix;
    mat4 normal_matrix;
} push;

const vec3 DIRECTION_TO_LIGHT = normalize(vec3(1.0, -3.0, -1.0));
const float AMBIENT = 0.02;

void main() {
    gl_Position = ubo.projectionViewMatrix * push.model_matrix * vec4(position, 1.0);

    // only works for uniform scaling (equal in all directions)
    // vec3 normal_world_space = normalize(mat3(push.model_matrix) * normal);

//    mat3 normal_matrix = transpose(linverse(mat3(push.model_matrix)));
//    vec3 normal_world_space = normalize(normal_matrix * normal);

    vec3 normal_world_space = normalize(mat3(push.normal_matrix) * normal);
    float light_intensity = AMBIENT + max(dot(normal_world_space, ubo.directionToLight), 0);

    fragColor = color * light_intensity;
}