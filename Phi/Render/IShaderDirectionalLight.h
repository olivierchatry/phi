#pragma once

#include <Render/IShader.h>

namespace Render
{
    struct IShaderDirectionalLight : public IShaderPositionNormalUV {
        
        struct Material {
            glm::vec4       MaterialDiffuse;
            glm::vec4       MaterialAmbient;
            glm::vec4       MaterialSpecular;
            float           MaterialShininess;
        };
        
        virtual void    setMaterial(Material& material) = 0;
        virtual void    setLightDirection(glm::vec3& direction) = 0;        
    };
};