#pragma once

#include <Render/IShader.h>

namespace Render
{

    struct Material {
        glm::vec4       MaterialDiffuse;
        glm::vec4       MaterialAmbient;
        glm::vec4       MaterialSpecular;
        float           MaterialShininess;
    };
    
    struct IShaderDirectionalLight : public IShaderPositionNormalUV {
        virtual void    setMaterial(Material& material) = 0;
        virtual void    setLightDirection(glm::vec3& direction) = 0;        
    };

    struct IShaderDirectionalLightNoTex : public IShaderPositionNormal {
        
        
        virtual void    setMaterial(Material& material) = 0;
        virtual void    setLightDirection(glm::vec3& direction) = 0;
    };

};