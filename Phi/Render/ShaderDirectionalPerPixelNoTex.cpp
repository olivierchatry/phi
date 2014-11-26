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

		mMatrixView = mShader.getUniform("uni_ModelViewMatrix");
		mMatrixNormal = mShader.getUniform("uni_NormalMatrix");
		mMatrixProjection = mShader.getUniform("uni_ProjectionMatrix");
		
		mMaterialDiffuse = mShader.getUniform("uni_MaterialDiffuse");
		mMaterialAmbient = mShader.getUniform("uni_MaterialAmbient");
		mMaterialSpecular = mShader.getUniform("uni_MaterialSpecular");
		mMaterialShininess = mShader.getUniform("uni_MaterialShininess");
		mLightDirection = mShader.getUniform("uni_LightDirection");
		
	}
	
	void ShaderDirectionalPerPixelNoTex::bind()
	{
		mShader.bind();
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
	
	void  ShaderDirectionalPerPixelNoTex::setCamera(Camera& camera)
	{
		mShader.setUniform(mMatrixView, camera.mView);
		mShader.setUniform(mMatrixProjection, camera.mProjection);
		glm::mat3 matrixNormal(1.f);
		mShader.setUniform(mMatrixNormal, matrixNormal);
	}

}