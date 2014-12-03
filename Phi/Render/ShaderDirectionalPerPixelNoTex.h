#pragma once

#include <Render/IShaderDirectionalLight.h>

namespace Render
{
    struct ShaderDirectionalPerPixelNoTex : public IShaderDirectionalLightNoTex
    {
        void create();
        
        void    setMaterial(Material& material) override;
        void    setLightDirection(glm::vec3& direction) override;
        void    setMatrices(glm::mat4& projection, glm::mat4& view, glm::mat4& model) override;
        void    setZPlane(float near, float far);
        
        int             mMatrixView;
        int             mMatrixNormal;
        int             mMatrixProjection;
        int             mCoeff;

        int             mMaterialDiffuse;
        int             mMaterialAmbient;
        int             mMaterialSpecular;
        int             mMaterialShininess;
        
        int             mLightDirection;
    };
};