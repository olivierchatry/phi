#version 140

uniform vec4  uni_MaterialDiffuse;
uniform vec4  uni_MaterialAmbient;
uniform vec4  uni_MaterialSpecular;
uniform float uni_MaterialShininess;
uniform vec3  uni_LightDirection;

in vec3 vertexShader_Normal;
in vec3 vertexShader_EyeNormal;
in vec4 vertexShader_Eye;

out vec4 out_FragColor;

float computeFresnelFactor(float cosAngle, float fresValue)
{
	return clamp(fresValue + (1.0 - fresValue) * pow(1.0 - cosAngle, 5.0), 0.0, 1.0);

}

void main(void)
{
    // set the specular term to black
    vec4 spec = vec4(0.0);
	float fresValue = 0.1461584658; // diamond..
    
    // normalize both input vectors
    vec3 n = normalize(vertexShader_Normal);
    vec3 e = normalize(vec3(vertexShader_Eye));
    if(dot(vertexShader_EyeNormal, e) < 0)
		n = -n;
    float intensity = max(dot(n,uni_LightDirection), 0.0);
    
    // if the vertex is lit compute the specular color
    if (intensity > 0.0)
	{
        // compute the half vector
        vec3 h = normalize(uni_LightDirection + e);
        // compute the specular term into spec
        float intSpec = max(dot(h,n), 0.0);
        spec = uni_MaterialSpecular * pow(intSpec,uni_MaterialShininess);
    }
	vec4 matDiff = uni_MaterialDiffuse*uni_MaterialDiffuse;

    out_FragColor.xyz = sqrt((intensity + uni_MaterialAmbient) * matDiff + spec).xyz;
    out_FragColor.w = 1;
    // min(computeFresnelFactor(abs(dot(vertexShader_EyeNormal, e)), fresValue), 1.0);
}
