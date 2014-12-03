#version 140

uniform mat4 uni_ProjectionMatrix;
uniform mat4 uni_ModelViewMatrix;
uniform mat3 uni_NormalMatrix;
uniform float   uni_ZCoeff;

in vec3 in_Position;
in vec3 in_Normal;

out vec3 vertexShader_Normal;
out vec3 vertexShader_EyeNormal;
out vec4 vertexShader_Eye;

void main(void) {
    vec4 position = vec4(in_Position, 1);
    vertexShader_Normal = normalize(uni_NormalMatrix * in_Normal);
    vertexShader_EyeNormal = normalize(uni_ModelViewMatrix * vec4(in_Normal, 0.0)).xyz;
    vertexShader_Eye = -(uni_ModelViewMatrix * position);
    gl_Position = uni_ProjectionMatrix * uni_ModelViewMatrix * position;
    gl_Position.z = log2(max(1e-6, 1.0 + gl_Position.w)) * uni_ZCoeff - 1.0;
}

