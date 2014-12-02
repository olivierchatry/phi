#pragma once

#include <Game/GameEntity.h>
#include <Render/Engine.h>
#include <Render/IShaderDirectionalLight.h>
#include <Render/AABB.h>
#include <Math/Plane.h>

namespace Game
{
	class TrackControlPoint : public GameEntity
	{
	public:
		void initialize(Initialize &initialize) override;
		void update(Update& update) override;
		void destroy(Destroy& destroy) override;
		void render(RenderArg& render) override;
		
		void setShader(Render::IShaderDirectionalLightNoTex* shader);
	
	private:
		enum Type  {
			POINT = 0,
			RIGHT,
			UP,
			FRONT
		};

		struct Renderable
		{
			Engine::VertexBuffer				vertexBuffer;
			int									count;
			Engine::VertexArray					vertexArray;
			glm::vec4							color;
            Math::AABB							aabb;
            glm::vec3                           direction;
			glm::vec3							normal;			
		};
		
	private:
		glm::vec3 getPlaneNormal(Update& update, Type type);

	private:
		Renderable                              mRenderableHelper[4];
        Render::IShaderDirectionalLightNoTex*   mShader;
        Renderable*                             mSelected;
		Math::Plane								mMovePlane;

        bool                                    mButtonWasPressed;
        int                                     mSelectedTrackPoint;
        glm::dvec2                              mPreviousMousePosition;
		glm::vec3								mDistanceWithInitialClick;
		glm::vec3								mInitialClick;

	};
};


