#pragma once

#include <Render/Engine.h>
#include <Render/Camera.h>

namespace Render {
	
	struct IShader {
        virtual void setMatrices(glm::mat4& projection, glm::mat4& view, glm::mat4& model) = 0;
        virtual void bind() { mShader.bind(); }

        
        Engine::Shader mShader;
    };


    
    struct IShaderPositionNormal : public IShader {
        int             getVsPosition() { return mVsPosition; }
        int             getVsNormal()   { return mVsNormal; }
        
        int             mVsPosition;
        int             mVsNormal;
    };

	struct IShaderPositionNormalUV : public IShaderPositionNormal {
		int             getVsUV()   { return mVsUV; }
		int             mVsUV;
	};

};
