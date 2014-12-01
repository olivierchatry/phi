#pragma once

#include <Game/GameEntity.h>
#include <Render/Engine.h>
#include <Render/IShaderDirectionalLight.h>

namespace Game
{
	class MousePointOnTrack : public GameEntity
	{
	public:
		void initialize(Initialize &initialize) override;
		void update(Update& update) override;
		void destroy(Destroy& destroy) override;
		void render(RenderArg& render) override;
		
		void setShader(Render::IShaderDirectionalLightNoTex* shader);
	public:
		struct Physic
		{
			glm::vec3	position;
			glm::vec3	previousPosition;
			glm::vec3	direction;
			
			glm::vec3	sideBoostVelocity;
			float		sideBoostForce;
			
			glm::vec3	gravityVelocity;
			float		gravityForce;
			
			glm::vec3	gravity;
			glm::vec3	normal;
			glm::vec3	right;
			
			glm::vec3	boosterVelocity;
			float		boosterForce;
		};
		
	public:
		Physic& getPhysic() { return mPhysic; }
	
	private:
		struct Renderable
		{
			Engine::VertexBuffer				vertexBuffer;
			int									count;
			Engine::VertexArray					vertexArray;
		};
		

	private:
		glm::vec3		mDirection;
		Renderable      mRenderable;
		Physic          mPhysic;
		glm::mat4       mMatrix;
		Render::IShaderDirectionalLightNoTex* mShader;
		float           mPreviousDeltaOnSpline;
	};
};


