#version 410

layout(location=0) out vec4 vFragColor;

uniform sampler2D diffuse_map;

in vec2 vertex_color;

layout(std140) uniform Modifiers {
    float light_intensity;
    vec3 light_color;
};

void main() {
    vFragColor = texture(diffuse_map, vertex_color);
}
