#version 140

uniform mat4 uni_ProjectionMatrix;

in vec2 in_Position;
in vec4 in_Color;
in vec2 in_UV;

out vec4 vertexShader_Color;
out vec2 vertexShader_UV;

void main(void) {
    vec4 position = vec4(in_Position, 0.0, 1.0);
    vertexShader_Color = in_Color;
    vertexShader_UV = in_UV;
    gl_Position = uni_ProjectionMatrix * position;
}
