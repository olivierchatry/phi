
#include <PreCompile.h>
#include <Game/Level.h>
#include <Game/TrackControlPoint.h>
#include <Utils/Utils.h>

namespace Game
{
	
	void TrackControlPoint::initialize(Initialize &initialize)
	{

        float arrowLen = 40.f;

        glm::vec3 direction[] = {
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(1.f, 0.f, 0.f),
            glm::vec3(0.f, 1.f, 0.f),
            glm::vec3(0.f, 0.f, 1.f)};

		float helperScale[] = {
			2.5f,
			10.f,
			10.f,
			10.f
		};

        for (int i = 0; i < 4 ; ++i)
        {
            Math::AABB aabb;

            aabb.reset();
            aabb.add(-direction[i] * arrowLen - glm::vec3(1.f));
            aabb.add(direction[i] * arrowLen + glm::vec3(1.f));
            aabb.expand(glm::vec3(helperScale[i]));
            
            std::vector<float> vs;
            Utils::GenerateCube(aabb, vs);
            Utils::GenerateNormals(&vs[0], 6, vs.size() / 6, 0, 3);

            mRenderableHelper[i].vertexBuffer.create(GL_STATIC_DRAW, vs.size() * sizeof(float));
            mRenderableHelper[i].vertexBuffer.update(&vs[0], 0, vs.size() * sizeof(float));
            mRenderableHelper[i].count = vs.size() / 6;
            mRenderableHelper[i].color = glm::vec4(direction[i], 1.f);
            mRenderableHelper[i].aabb = aabb;
            mRenderableHelper[i].direction = glm::normalize(direction[i]);
        }
        mSelectedTrackPoint = -1;
        mButtonWasPressed = false;
        mSelected = NULL;

	}
	
	void TrackControlPoint::destroy(Destroy &destroy)
	{
		
	}
	
	void TrackControlPoint::setShader(Render::IShaderDirectionalLightNoTex *shader)
	{
		mShader = shader;
		

		for (int i = 0; i < 4; ++i)
		{
			mRenderableHelper[i].vertexArray.destroy();
			mRenderableHelper[i].vertexArray.create();
			{
				Engine::VertexArray::Binder     bind1(mRenderableHelper[i].vertexArray);
				Engine::VertexBuffer::Binder    bind2(mRenderableHelper[i].vertexBuffer);

				mRenderableHelper[i].vertexArray.attrib(shader->getVsPosition(), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
				mRenderableHelper[i].vertexArray.attrib(shader->getVsNormal(), 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), 3 * sizeof(float));
			}

		}
	}

	
	void TrackControlPoint::render(RenderArg &render)
	{
		if (render.level == NULL)
			return;
		Level* level = (Level*)render.level;
		if (render.passElement == Engine::Solid)
		{

			mShader->bind();
			mShader->setLightDirection(render.sunDirection);

			
			Render::Material material;
			material.MaterialAmbient = glm::vec4(0.2f);
				
			material.MaterialSpecular = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			material.MaterialShininess = 64.f;

            int point = 0;
			for (auto& p : level->mTrack.points)
			{
				glm::mat4 matrix = glm::translate(p);
				mShader->setMatrices(render.projection, render.view, matrix);

				for (int i = 0; i < 4; ++i)
				{
                    if (mSelected == &mRenderableHelper[i] && mSelectedTrackPoint == point)
                        material.MaterialDiffuse = glm::vec4(1.f, 0.f, 1.f, 1.f);
                    else
                        material.MaterialDiffuse = mRenderableHelper[i].color;
					mShader->setMaterial(material);
                    
					Engine::VertexArray::Binder  bind1(mRenderableHelper[i].vertexArray);
					glEnable(GL_CULL_FACE);
					glCullFace(GL_BACK);
					glDrawArrays(GL_TRIANGLES, 0, mRenderableHelper[i].count);
				}
                point++;
			}
		}
	}
	glm::vec3 TrackControlPoint::getPlaneNormal(Update& update, Type type)
	{
		switch (type)
		{
		case POINT:
			return -update.centerProjectedDirection;
			break;
		case RIGHT:
			return -glm::vec3(0.f, 0.f, 1.f);
			break;
		case UP:
			return -glm::vec3(1.f, 0.f, 0.f);
			break;
		case FRONT:
			return -glm::vec3(0.f, 1.f, 0.f);
			break;
		}
	}
	
	void TrackControlPoint::update(Update& update)
	{
		if (update.level == NULL)
			return;
        Level* level = (Level*)update.level;
		bool pressed = glfwGetMouseButton(update.window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS;

        if (!pressed && !mButtonWasPressed)
        {
            mSelected = NULL;
            mSelectedTrackPoint = 0;

            int		trackPoint = 0;
			float	currentLength = std::numeric_limits<float>::max();            
			for (auto& p : level->mTrack.points)
            {
                glm::mat4 matrix = glm::translate(p);
                for (int i = 0; i < 4; ++i)
                {
                    Renderable&     renderable = mRenderableHelper[i];
                    Math::AABB		aabb =  matrix * renderable.aabb;
					glm::vec3		collision;
					if (Utils::RayIntersectBoundingBox(update.mouseProjectedPosition, update.mouseProjectedDirection, aabb, collision))
                    {
						float length = glm::distance2(collision, update.mouseProjectedPosition);						
						if (length < currentLength)
						{ 
							mSelected = &renderable;
							mSelectedTrackPoint = trackPoint;
							mMovePlane.fromPointNormal(collision, getPlaneNormal(update, (Type) i));
							mDistanceWithInitialClick = p - collision;
							mInitialClick = collision;
							currentLength = length;
						}
                    }
                }
                trackPoint ++;
            }
        }
        if (!pressed && mButtonWasPressed && mSelected)
        {
            Level::GenerateArgument arguments;
            level->generate();
            level->setShader(level->mShader);
			mSelected = NULL;
        }
		update.mouseTaken = false;
        mButtonWasPressed = pressed;
        if (mButtonWasPressed && mSelected)
        {			
			glm::vec3 p;
			if (mMovePlane.intersects(update.mouseProjectedPosition, update.mouseProjectedDirection, p))            
			{ 				
				p = (p + mDistanceWithInitialClick) - mInitialClick;
				p = p * mSelected->direction;
				p = mInitialClick + p;
				level->mTrack.points[mSelectedTrackPoint] = p ;
			}
        }
		update.mouseTaken = mSelected != NULL;
	}
};