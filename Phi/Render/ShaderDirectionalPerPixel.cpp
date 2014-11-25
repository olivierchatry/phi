#include <PreCompile.h>
#include <Render/ShaderDirectionalPerPixel.h>

namespace Render
{
	void ShaderDirectionalPerPixel::create()
	{
		mShader.create();
		mShader.addFromFile("Shaders/ShaderDirectionalPerPixel.vert", GL_VERTEX_SHADER);
		mShader.addFromFile("Shaders/ShaderDirectionalPerPixel.frag", GL_FRAGMENT_SHADER);
		mShader.link();
		
		mVsPosition = mShader.getAttribute("in_Position");
		mVsNormal = mShader.getAttribute("in_Normal");
		mVsUV = mShader.getAttribute("in_UV");

		mMatrixView = mShader.getUniform("uni_ModelViewMatrix");
		mMatrixNormal = mShader.getUniform("uni_NormalMatrix");
		mMatrixProjection = mShader.getUniform("uni_ProjectionMatrix");
		
		mMaterialDiffuse = mShader.getUniform("uni_MaterialDiffuse");
		mMaterialAmbient = mShader.getUniform("uni_MaterialAmbient");
		mMaterialSpecular = mShader.getUniform("uni_MaterialSpecular");
		mMaterialShininess = mShader.getUniform("uni_MaterialShininess");
		mLightDirection = mShader.getUniform("uni_LightDirection");
		mTexture = mShader.getUniform("uni_Texture");
		
	}
	
	void ShaderDirectionalPerPixel::bind()
	{
		mShader.bind();
	}
	
	void  ShaderDirectionalPerPixel::setMaterial(Material& material)
	{
		mShader.setUniform(mMaterialShininess, material.MaterialShininess);
		mShader.setUniform(mMaterialDiffuse, material.MaterialDiffuse);
		mShader.setUniform(mMaterialSpecular, material.MaterialSpecular);
		mShader.setUniform(mMaterialAmbient, material.MaterialAmbient);
	}
	
	void  ShaderDirectionalPerPixel::setLightDirection(glm::vec3& direction)
	{
		mShader.setUniform(mLightDirection, direction);
	}
	
	void  ShaderDirectionalPerPixel::setCamera(Camera& camera)
	{
		mShader.setUniform(mMatrixView, camera.mView);
		mShader.setUniform(mMatrixProjection, camera.mProjection);
		glm::mat3 matrixNormal(1.f);

		mShader.setUniform(mMatrixNormal, matrixNormal);
		
	}

}