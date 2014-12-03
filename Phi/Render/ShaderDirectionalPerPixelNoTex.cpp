#include <PreCompile.h>
#include <Render/ShaderDirectionalPerPixelNoTex.h>

namespace Render
{
	void ShaderDirectionalPerPixelNoTex::create()
	{
		mShader.create();
		mShader.addFromFile("Shaders/ShaderDirectionalPerPixelNoTex.vert", GL_VERTEX_SHADER);
		mShader.addFromFile("Shaders/ShaderDirectionalPerPixelNoTex.frag", GL_FRAGMENT_SHADER);
		mShader.link();
		
		mVsPosition = mShader.getAttribute("in_Position");
		mVsNormal = mShader.getAttribute("in_Normal");

        mCoeff = mShader.getUniform("uni_Coeff");
		mMatrixView = mShader.getUniform("uni_ModelViewMatrix");
		mMatrixNormal = mShader.getUniform("uni_NormalMatrix");
		mMatrixProjection = mShader.getUniform("uni_ProjectionMatrix");
		
		mMaterialDiffuse = mShader.getUniform("uni_MaterialDiffuse");
		mMaterialAmbient = mShader.getUniform("uni_MaterialAmbient");
		mMaterialSpecular = mShader.getUniform("uni_MaterialSpecular");
		mMaterialShininess = mShader.getUniform("uni_MaterialShininess");
		mLightDirection = mShader.getUniform("uni_LightDirection");
		
	}
		
    void ShaderDirectionalPerPixelNoTex::setZPlane(float near, float far)
    {
        mShader.setUniform(mCoeff, 2.0f / glm::log2(far + 1.0f));
    }

    void  ShaderDirectionalPerPixelNoTex::setMaterial(Material& material)
	{
		mShader.setUniform(mMaterialShininess, material.MaterialShininess);
		mShader.setUniform(mMaterialDiffuse, material.MaterialDiffuse);
		mShader.setUniform(mMaterialSpecular, material.MaterialSpecular);
		mShader.setUniform(mMaterialAmbient, material.MaterialAmbient);
	}
	
	void  ShaderDirectionalPerPixelNoTex::setLightDirection(glm::vec3& direction)
	{
		mShader.setUniform(mLightDirection, direction);
	}
	
	void  ShaderDirectionalPerPixelNoTex::setMatrices(glm::mat4& projection, glm::mat4& view, glm::mat4& model)
	{
		glm::mat4 modelView = view * model;
		mShader.setUniform(mMatrixView, modelView);
		mShader.setUniform(mMatrixProjection, projection);
		glm::mat3 matrixNormal(glm::extractMatrixRotation(model));
		mShader.setUniform(mMatrixNormal, matrixNormal);
	}

}