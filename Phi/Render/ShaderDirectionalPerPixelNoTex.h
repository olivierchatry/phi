#pragma once

#include <Render/IShaderDirectionalLight.h>

namespace Render
{
    struct ShaderDirectionalPerPixelNoTex : public IShaderDirectionalLightNoTex
    {
        void create();
        void bind();
        
        
        void    setMaterial(Material& material);
        void    setLightDirection(glm::vec3& direction);
        void    setCamera(Camera& camera);
        
        int             mMatrixView;
        int             mMatrixNormal;
        int             mMatrixProjection;
        
        int             mMaterialDiffuse;
        int             mMaterialAmbient;
        int             mMaterialSpecular;
        int             mMaterialShininess;
        
        int             mLightDirection;

        Engine::Shader	mShader;
    };
};