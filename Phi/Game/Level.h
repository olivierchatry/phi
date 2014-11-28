#pragma once

#include <Render/Engine.h>
#include <Render/AABB.h>
#include <Render/IShaderDirectionalLight.h>
#include <Render/Camera.h>

namespace Game {
    class Level {
    public:
        Level();
	public:
		struct GenerateArgument
		{
			GenerateArgument()
			{
				// generate a point every 2 meters.
				generatePointEvery = 2.f;
				// size of a track chunk in meters.
				chunkSize = 40;
				// number of subdivision of each ring around a point.
				circleSubDivice = 40;
			}

			float generatePointEvery;
			float chunkSize;
			float circleSubDivice;
		};

    public:
		void generate(Render::IShaderPositionNormalUV& shader, GenerateArgument& arguments);
        void render();
		void destroy();
	
	public:
		void computeChunkDistanceToCamera(const glm::vec3& position, const glm::vec3& direction);
		bool findNearestDelta(glm::vec3& position, float& deltaFound, int steps = 1);
	public:
		glm::vec3 getPosition(float delta)
		{
			return get(delta, mTrack.points);
		}

		float getRadius(float delta)
		{
			return get(delta, mTrack.radius);
		}

		glm::vec3 getNormal(float delta)
		{
			glm::vec3 v1 = get(delta, mTrack.points);
			glm::vec3 v2 = get(delta + mSmallestDelta, mTrack.points);

			glm::vec3 direction = glm::normalize(v2 - v1);
			glm::vec3 up(0, 0, 1);
			glm::vec3 right = glm::normalize(glm::cross(direction, up));
			return glm::normalize(glm::cross(right, direction));
		}

		template <typename T>        
        T get(float delta, std::vector<T>& points)
		{
			int size = (int)points.size();
			int p0, p1, rp0, rp1;
			T tmp0, tmp1;
			T r0, r1;
			float pathpos = delta * (size);
			float t0 = 1.0;
			float t1 = glm::clamp((float)(pathpos - floor(pathpos)), 0.0f, 1.0f);
			float t2 = t1 * t1;
			float t3 = t1 * t2;

			p0 = (int)glm::floor(pathpos) % size;
			p1 = (p0 + 1) % size;
			rp0 = (p0 + (size - 1)) % size;
			rp1 = (p0 + 2) % size;

			glm::vec4  bernstein;

			bernstein.x = t3 *  2.0f + t2 * -3.0f + t1 *  0.0f + t0 *  1.0f;
			bernstein.y = t3 * -2.0f + t2 *  3.0f + t1 *  0.0f + t0 *  0.0f;
			bernstein.z = t3 *  1.0f + t2 * -2.0f + t1 *  1.0f + t0 *  0.0f;
			bernstein.w = t3 *  1.0f + t2 * -1.0f + t1 *  0.0f + t0 *  0.0f;

			r0 = (points[p1] - points[rp0]) * 0.5f;
			r1 = (points[rp1] - points[p0]) * 0.5f;


			tmp0 = points[p0] * bernstein.x;
			tmp1 = points[p1] * bernstein.y;

			r0 = r0 * bernstein.z;
			r1 = r1 * bernstein.w;
			return tmp0 + tmp1 + r0 + r1;
		}
    
	public:

		struct Track
		{
			std::vector<glm::vec3>  points;
			std::vector<float>		radius;
			
		};

		struct TrackChunkRenderable
		{
			Engine::VertexBuffer				vertexBuffer;
			Engine::IndexBuffer					indexBuffer;
			int									count;
			Engine::VertexArray					vertexArray;			
			Render::AABB						aabb;
			float								distance;
			
			float								deltaStart;
			float								deltaEnd;
		};

		GenerateArgument					mGenerationArguments;
		std::vector<TrackChunkRenderable*>	mTrackChunks;
		Track								mTrack;
		Engine::Texture						mTexture;
		float								mSmallestDelta;
        float                               mTotalLength;
		Track								mGeneratedTrack;
        Render::AABB                        mLevelAABB;
	public:
		static bool sortByDistance(TrackChunkRenderable* a, TrackChunkRenderable* b)
		{
			return a->distance > b->distance;
		}
	};
};