    #include <PreCompile.h>
#include <Game/Level.h>
#include <Game/TrackControlPoint.h>
#include <Render/AABB.h>
#include <Utils/Utils.h>

namespace Game
{
	
	void TrackControlPoint::initialize(Initialize &initialize)
	{

        float helperScale = 10.f;
        float arrowLen = 40.f;

        glm::vec3 direction[] = {
            glm::vec3(0.5f, 0.5f, 0.5f),
            glm::vec3(1.f, 0.f, 0.f),
            glm::vec3(0.f, 1.f, 0.f),
            glm::vec3(0.f, 0.f, 1.f)};
        for (int i = 0; i < 4 ; ++i)
        {
            Render::AABB aabb;

            aabb.reset();
            aabb.add(-direction[i] * arrowLen - glm::vec3(1.f));
            aabb.add(direction[i] * arrowLen + glm::vec3(1.f));
            aabb.expand(glm::vec3(helperScale));
            
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
	
	void TrackControlPoint::update(Update& update)
	{
		if (update.level == NULL)
			return;
        Level* level = (Level*)update.level;
        bool pressed = glfwGetMouseButton(update.window, GLFW_MOUSE_BUTTON_1);
        mSelectedTrackPoint = 0;

        glm::dvec2 currentMousePosition;
        glfwGetCursorPos(update.window, &currentMousePosition.x, &currentMousePosition.y);

        //if (pressed && !mButtonWasPressed)
        {
            mSelected = NULL;
            mPreviousMousePosition = currentMousePosition;
            int trackPoint = 0;
            for (auto& p : level->mTrack.points)
            {
                glm::mat4 matrix = glm::translate(p);
                for (int i = 0; i < 4; ++i)
                {
                    Renderable& renderable = mRenderableHelper[i];
                    Render::AABB aabb =  matrix * renderable.aabb;
                    if (Utils::RayIntersectBoundingBox(update.mouseProjectedPosition, update.mouseProjectedDirection, aabb))
                    {
                        mSelected = &renderable;
                        mSelectedTrackPoint = trackPoint;
                    }
                }
                trackPoint ++;
            }
        }
        mButtonWasPressed = pressed;
        if (mButtonWasPressed && mSelected)
        {
            glm::dvec2 delta = mPreviousMousePosition - currentMousePosition;
            level->mTrack.points[mSelectedTrackPoint] += mSelected->direction * (float) glm::length(delta);
        }
	}
};