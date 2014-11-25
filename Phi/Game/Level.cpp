#include <PreCompile.h>
#include <Game/Level.h>
#include <Utils/Utils.h>
namespace Game {


	Level::Level()
	{
		mTrack.points.push_back(glm::vec3(-2000, 0, 1000));
		mTrack.radius.push_back(10);
		mTrack.points.push_back(glm::vec3(0, 2000, -1000));
		mTrack.radius.push_back(20);
		mTrack.points.push_back(glm::vec3(2000, 0, 0));
		mTrack.radius.push_back(30);
		mTrack.points.push_back(glm::vec3(0, -1000, 500));
		mTrack.radius.push_back(40);

		mTrack.points.push_back(glm::vec3(-2000, -1000, 800));
		mTrack.radius.push_back(50);

		mTrack.points.push_back(glm::vec3(-2000, 0, 3000));
		mTrack.radius.push_back(40);

		mTrack.points.push_back(glm::vec3(-2500, 0, 2500));
		mTrack.radius.push_back(30);

		mTrack.points.push_back(glm::vec3(-1500, 0, 1500));
		mTrack.radius.push_back(20);

	}


	void Level::generate(Render::IShaderPositionNormalUV& shader)
	{
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
				texture[offset + 3] = ((x > 4) && (x < 28) && (y > 4) && (y < 28)) ? 0 : 255;
			}
		}
		mTexture.createFromBuffer(&(texture[0]), 32, 32, GL_TEXTURE_2D, 4);
		mTexture.bind(0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		// compute len;
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
		std::vector<glm::vec3>	splines;
		std::vector<float>		radius;
		// we subdivide every meter.
		mSmallestDelta = 2.f / mTotalLength;
		float current = 0;

		while (current < 1)
		{
			splines.push_back(get<glm::vec3>(current, mTrack.points));
			radius.push_back(get<float>(current, mTrack.radius));
			current += mSmallestDelta;
		}

		//////////////////////////////////////////////////////////////////////////
		// compute angles to generate the circles for each part of the tube.
		//////////////////////////////////////////////////////////////////////////

		std::vector<glm::vec2> angles;
		float pi = glm::pi<float>();
		float twoPi = pi * 2.0f;
		float count = 40;
		float deltaAngle = twoPi / count;
		current = 0;
		while (current < twoPi)
		{
			angles.push_back(glm::vec2(cos(current), sin(current)));
			current += deltaAngle;
		}

		size_t circleSize = angles.size();
		size_t trackChunkSize = 100;


		size_t					currentPointInSpline = 0;

		mTrackChunks.reserve((splines.size() * 2) / trackChunkSize + 1);

		while (currentPointInSpline < splines.size())
		{
			std::vector<unsigned short>	indices;
			std::vector<float>			vs;

			vs.reserve(trackChunkSize * (circleSize + 1) * 8);
			indices.reserve(trackChunkSize * circleSize * 6);

			Render::AABB chunkAABB;
			chunkAABB.reset();
			float v = 0;
			float vDeltaRepeat = 2.f / trackChunkSize;
			for (size_t i = 0; (i < trackChunkSize) && (currentPointInSpline < splines.size() + 1); ++i)
			{

				bool	generateIndices = (i != trackChunkSize - 1) && (currentPointInSpline < splines.size());

				size_t	actualCurrentPointInSpline = currentPointInSpline % splines.size();
				size_t	nextPointInSpline = (currentPointInSpline + 1) % splines.size();


				glm::vec3& v1 = splines[actualCurrentPointInSpline];
				glm::vec3& v2 = splines[nextPointInSpline];

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
					glm::vec3 position = v1 + (right * angles[actualAngle].x * radius[actualCurrentPointInSpline] + up * angles[actualAngle].y * radius[actualCurrentPointInSpline]);

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
				if (currentPointInSpline < (splines.size() + 1))
					currentPointInSpline--;
				else
					currentPointInSpline += 0;

				TrackChunkRenderable* chunk = new TrackChunkRenderable();

				chunk->aabb = chunkAABB;


				chunk->count = (int)indices.size();
				Utils::GenerateNormals(&indices[0], &vs[0], 8, chunk->count, 0, 3, false);


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