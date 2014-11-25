#pragma once

#include <Render/Engine.h>
#include <Render/Camera.h>

namespace Render {
	
	struct IShader {
        virtual void setCamera(Camera& camera) = 0;
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
