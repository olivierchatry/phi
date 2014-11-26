#include <PreCompile.h>
#include <Game/Level.h>
#include <Utils/Utils.h>
namespace Game {


	Level::Level()
	{
		mTrack.points.push_back(glm::vec3(-2000, -2000, 2000));
		mTrack.radius.push_back(100);
		mTrack.points.push_back(glm::vec3(-2000, 2000, 800));
		mTrack.radius.push_back(200);
		mTrack.points.push_back(glm::vec3(2000, 2000, 3000));
		mTrack.radius.push_back(300);
		mTrack.points.push_back(glm::vec3(2000, -2000, 4000));
		mTrack.radius.push_back(200);

	}

	bool Level::findNearestDelta(glm::vec3& position, float& delta, int steps)
	{		
		float distance = std::numeric_limits<float>::max();
		
		TrackChunkRenderable* selectedChunk = NULL;
		
		// get the nearest chunk.
		for (auto chunk : mTrackChunks)
		{
			float chunkDistance = chunk->aabb.distanceFrom(position);
			if (distance > chunkDistance)
			{
				distance = chunkDistance;
				selectedChunk = chunk;
			}
		}

		if (selectedChunk == NULL)
			return false;
		
		float currentLength = FLT_MAX;

		float deltaStart = selectedChunk->deltaStart;
		float deltaEnd = selectedChunk->deltaEnd;
		float deltaDelta = mSmallestDelta;
		

		while (steps--)
		{
			for (float d = selectedChunk->deltaStart; d < selectedChunk->deltaEnd; d += mSmallestDelta)
			{
				float length = glm::distance2(getPosition(d), position);
				if (length < currentLength)
				{
					currentLength = length;
					delta = d;
				}
			}
			
			deltaStart = delta - mSmallestDelta;
			deltaEnd = delta + mSmallestDelta;
			deltaDelta = mSmallestDelta / 2.0f;
		}

		return true;
	}

	void Level::generate(Render::IShaderPositionNormalUV& shader, GenerateArgument& arguments)
	{
		//////////////////////////////////////////////////////////////////////////
		// generate crappy texture for testing.
		//////////////////////////////////////////////////////////////////////////
		std::vector<GLubyte> texture;
		texture.resize(32 * 32 * 4);

		for (int y = 0; y < 32; ++y)
		{
			for (int x = 0; x < 32; ++x)
			{
				int offset = (y * 32 + x) * 4;
				texture[offset + 0] = 255;
				texture[offset + 1] = 255;
				texture[offset + 2] = 255;
				texture[offset + 3] = ((x > 4) && (x < 28) && (y > 4) && (y < 28)) ? 32 : 255;
			}
		}
		mTexture.createFromBuffer(&(texture[0]), 32, 32, GL_TEXTURE_2D, 4);
		mTexture.bind(0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//////////////////////////////////////////////////////////////////////////
		// compute total len of the track, in meters.
		//////////////////////////////////////////////////////////////////////////
		mTotalLength = 0;
		for (size_t i = 0; i < mTrack.points.size(); ++i)
		{
			size_t j = i + 1;
			if (j >= mTrack.points.size())
				j = 0;

			glm::vec3& v1 = mTrack.points[i];
			glm::vec3& v2 = mTrack.points[j];
			glm::vec3 v = v2 - v1;
			mTotalLength += glm::length(v);
		}
		
		//////////////////////////////////////////////////////////////////////////
		// generate track position.
		// we subdivide every meter.
		//////////////////////////////////////////////////////////////////////////
		mSmallestDelta = arguments.generatePointEvery / mTotalLength;
		float current = 0;
		
		size_t genratedPointCount = (size_t) glm::ceil( (1 / mSmallestDelta)  );
		mGeneratedTrack.points.reserve(genratedPointCount);
		mGeneratedTrack.radius.reserve(genratedPointCount);

		while (current < 1)
		{
			mGeneratedTrack.points.push_back(get<glm::vec3>(current, mTrack.points));
			mGeneratedTrack.radius.push_back(get<float>(current, mTrack.radius));
			current += mSmallestDelta;
		}

		//////////////////////////////////////////////////////////////////////////
		// compute angles to generate the circles for each part of the tube.
		//////////////////////////////////////////////////////////////////////////
		std::vector<glm::vec2> angles;
		float twoPi = glm::pi<float>() * 2.0f;
		float deltaAngle = twoPi / arguments.circleSubDivice;
		current = 0;
		while (current < twoPi)
		{
			angles.push_back(glm::vec2(cos(current), sin(current)));
			current += deltaAngle;
		}


		//////////////////////////////////////////////////////////////////////////
		// actual generation of the vertex and indices. 
		// we are chunking the track so that we can do some nice transparency rendering
		// without to much rendering error.
		//////////////////////////////////////////////////////////////////////////
		size_t					circleSize = angles.size();
		size_t					trackChunkCount = (size_t) glm::ceil(mTotalLength / arguments.chunkSize);
		size_t					trackChunkSize  = (mGeneratedTrack.points.size() / trackChunkCount) * 2;
		size_t					currentPointInSpline = 0;

		mTrackChunks.reserve(trackChunkCount);

        float v = 0;
        mLevelAABB.reset();
		while (currentPointInSpline < mGeneratedTrack.points.size())
		{
			std::vector<unsigned short>	indices;
			std::vector<float>			vs;

			vs.reserve(trackChunkSize * (circleSize + 1) * 8);
			indices.reserve(trackChunkSize * circleSize * 6);

			Render::AABB chunkAABB;
			chunkAABB.reset();
			float vDeltaRepeat = 1.f / trackChunkSize;
			
			size_t chunkStartPointInSpline = currentPointInSpline;
			for (size_t i = 0; (i < trackChunkSize) && (currentPointInSpline < mGeneratedTrack.points.size() + 1); ++i)
			{

				bool	generateIndices = (i != trackChunkSize - 1) && (currentPointInSpline < mGeneratedTrack.points.size());

				size_t	actualCurrentPointInSpline = currentPointInSpline % mGeneratedTrack.points.size();
				size_t	nextPointInSpline = (currentPointInSpline + 1) % mGeneratedTrack.points.size();


				glm::vec3& v1 = mGeneratedTrack.points[actualCurrentPointInSpline];
				glm::vec3& v2 = mGeneratedTrack.points[nextPointInSpline];

				glm::vec3 direction = glm::normalize(v2 - v1);
				glm::vec3 up(0, 0, 1);
				glm::vec3 right = glm::normalize(glm::cross(direction, up));
				up = glm::normalize(glm::cross(right, direction));

				size_t s = i * (circleSize + 1);

				float uDeltaRepeat = 5.f / (float)circleSize;
				float u = 0;

				for (size_t a = 0; a <= circleSize; ++a)
				{
					size_t actualAngle = a % circleSize;
					float radius = mGeneratedTrack.radius[actualCurrentPointInSpline];
					glm::vec2& angle = angles[actualAngle];
					glm::vec3 position = v1 + (right * angle.x * radius + up * angle.y * radius);

					vs.push_back(position.x);
					vs.push_back(position.y);
					vs.push_back(position.z);

					vs.push_back(0);
					vs.push_back(0);
					vs.push_back(0);

					vs.push_back(u);
					vs.push_back(v);

					u += uDeltaRepeat;

					chunkAABB.add(position);

					if (generateIndices && (actualAngle == a))
					{
						size_t b = (a + 1);

						unsigned short i1 = (unsigned short)(s + a);
						unsigned short i2 = (unsigned short)(i1 + circleSize + 1);
						unsigned short i3 = (unsigned short)(s + b);
						unsigned short i4 = (unsigned short)(i3 + circleSize + 1);

						indices.push_back(i1);
						indices.push_back(i2);
						indices.push_back(i3);

						indices.push_back(i2);
						indices.push_back(i4);
						indices.push_back(i3);
					}
				}
				v += vDeltaRepeat;
				currentPointInSpline++;
			}

			if (!indices.empty())
			{

                mLevelAABB.add(chunkAABB);
				TrackChunkRenderable* chunk = new TrackChunkRenderable();
				chunk->aabb = chunkAABB;
				chunk->count = (int)indices.size();
				Utils::GenerateNormals(&indices[0], &vs[0], 8, chunk->count, 0, 3, false);

				chunk->deltaStart = chunkStartPointInSpline * mSmallestDelta;
				chunk->deltaEnd = currentPointInSpline* mSmallestDelta;

				chunk->indexBuffer.create(GL_STATIC_DRAW, indices.size() * sizeof(unsigned short));
				chunk->indexBuffer.update(&indices[0], 0, indices.size() * sizeof(unsigned short));
				chunk->vertexBuffer.create(GL_STATIC_DRAW, vs.size() * sizeof(float));
				chunk->vertexBuffer.update(&vs[0], 0, vs.size() * sizeof(float));
				chunk->vertexArray.create();

				{
					Engine::VertexArray::Binder     bind1(chunk->vertexArray);
					Engine::VertexBuffer::Binder    bind2(chunk->vertexBuffer);

					chunk->vertexArray.attrib(shader.getVsPosition(), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
					chunk->vertexArray.attrib(shader.getVsNormal(), 3, GL_FLOAT, GL_TRUE, 8 * sizeof(float), 3 * sizeof(float));
					chunk->vertexArray.attrib(shader.getVsUV(), 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 6 * sizeof(float));
				}
				mTrackChunks.push_back(chunk);
				if (currentPointInSpline < (mGeneratedTrack.points.size() + 1))
					currentPointInSpline--;
			}

		}
	}
	void Level::computeChunkDistanceToCamera(const glm::vec3& position, const glm::vec3& direction)
	{
		for (auto chunk : mTrackChunks)
		{
			glm::vec3 nearestPoint = chunk->aabb.nearestPoint(position);
			chunk->distance = chunk->aabb.distanceFrom(position);
			if (glm::dot(glm::normalize(nearestPoint - position), direction) < 0)
				chunk->distance = -chunk->distance;
		}

	}

	void Level::destroy()
	{
		for (auto chunk : mTrackChunks)
		{
			delete chunk;
		}
		mTrackChunks.clear();
	}
};