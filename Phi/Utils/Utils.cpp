#include <PreCompile.h>

#include <Utils/Utils.h>

namespace Utils
{
	void GenerateNormals(GLfloat* vs, int stride, int count, int offsetVertex, int offsetNormal)
	{

		while (count)
		{
			glm::fvec3* v1 = (glm::fvec3*) (vs + offsetVertex);
			glm::fvec3* v2 = (glm::fvec3*) (vs + stride + offsetVertex);
			glm::fvec3* v3 = (glm::fvec3*) (vs + stride + stride + offsetVertex);


			glm::fvec3 normal = glm::triangleNormal(*v1, *v2, *v3);

			*((glm::fvec3*) (vs + offsetNormal)) = normal;
			*((glm::fvec3*) (vs + stride + offsetNormal)) = normal;
			*((glm::fvec3*) (vs + stride + stride + offsetNormal)) = normal;

			vs += stride + stride + stride;
			count -= 3;
		}
	}

	void GenerateNormals(GLushort* index, GLfloat* vs, int stride, int count, int offsetVertex, int offsetNormal, bool zeroNormalsBefore)
	{
		if (zeroNormalsBefore)
		{
			int         temp = count;
			GLushort*	tempIndex = index;

			while (temp--)
				*((glm::fvec3*) (vs + *(tempIndex++) * stride + offsetNormal)) = glm::vec3(0);
		}
		{
			int         temp = count / 3;
			GLushort*	tempIndex = index;

			while (temp--)
			{
				int i1 = *(tempIndex++) * stride;
				int i2 = *(tempIndex++) * stride;
				int i3 = *(tempIndex++) * stride;

				glm::fvec3* v1 = (glm::fvec3*) (vs + i1 + offsetVertex);
				glm::fvec3* v2 = (glm::fvec3*) (vs + i2 + offsetVertex);
				glm::fvec3* v3 = (glm::fvec3*) (vs + i3 + offsetVertex);

				glm::fvec3 normal = glm::triangleNormal(*v1, *v2, *v3);

				*((glm::fvec3*) (vs + i1 + offsetNormal)) += normal;
				*((glm::fvec3*) (vs + i2 + offsetNormal)) += normal;
				*((glm::fvec3*) (vs + i3 + offsetNormal)) += normal;
			}
		}
		{
			int			temp = count;
			GLushort*	tempIndex = index;

			while (temp--)
			{
				glm::vec3* p = ((glm::vec3*) (vs + *(tempIndex++) * stride + offsetNormal));
				*p = glm::normalize(*p);
			}
		}
	}

	void CopyBuffer(const GLfloat* source, GLfloat* destination, int sourceStride, int sourceOffset, int destinationStride, int destinationOffset, int count, int elementSize)
	{
		count /= elementSize;
		while (count--)
		{
			for (int i = 0; i < elementSize; ++i)
			{
				*(destination + destinationOffset + i) = *(source + sourceOffset + i);
			}
			destination += destinationStride;
			source += sourceStride;
		}
	}

	glm::vec3 ExtractCameraPos(const glm::mat4& modelView)
	{
		// Get the 3 basis vector planes at the camera origin and transform them into model space.
		//  
		// NOTE: Planes have to be transformed by the inverse transpose of a matrix
		//       Nice reference here: http://www.opengl.org/discussion_boards/showthread.php/159564-Clever-way-to-transform-plane-by-matrix
		//
		//       So for a transform to model space we need to do:
		//            inverse(transpose(inverse(MV)))
		//       This equals : transpose(MV) - see Lemma 5 in http://mathrefresher.blogspot.com.au/2007/06/transpose-of-matrix.html
		//
		// As each plane is simply (1,0,0,0), (0,1,0,0), (0,0,1,0) we can pull the data directly from the transpose matrix.
		//  
		glm::mat4 modelViewT = glm::transpose(modelView);

		// Get plane normals 
		glm::vec3 n1(modelViewT[0]);
		glm::vec3 n2(modelViewT[1]);
		glm::vec3 n3(modelViewT[2]);

		// Get plane distances
		float d1(modelViewT[0].w);
		float d2(modelViewT[1].w);
		float d3(modelViewT[2].w);

		// Get the intersection of these 3 planes 
		// (uisng math from RealTime Collision Detection by Christer Ericson)
		glm::vec3 n2n3 = glm::cross(n2, n3);
		float denom = glm::dot(n1, n2n3);

		glm::vec3 top = (n2n3 * d1) + glm::cross(n1, (d3*n2) - (d2*n3));
		return top / -denom;
	}

	void GenerateCube(Math::AABB& aabb, std::vector<GLfloat>& v)
	{
		const int cubeIndices[] = {
			// front
			0, 1, 2,
			2, 3, 0,
			// top
			3, 2, 6,
			6, 7, 3,
			// back
			7, 6, 5,
			5, 4, 7,
			// bottom
			4, 5, 1,
			1, 0, 4,
			// left
			4, 0, 3,
			3, 7, 4,
			// right
			1, 5, 6,
			6, 2, 1,
		};

		for (int i = 0; i < 36; ++i)
		{
			glm::vec3 corner = aabb.corner(cubeIndices[i]);

			v.push_back(corner.x);
			v.push_back(corner.y);
			v.push_back(corner.z);

			v.push_back(0.f);
			v.push_back(0.f);
			v.push_back(0.f);
		}
	}

	bool RayIntersectBoundingBox(const glm::vec3& origin, const glm::vec3& dir, const Math::AABB& aabb, glm::vec3& result)
	{
		const int RIGHT = 0;
		const int LEFT = 1;
		const int MIDDLE = 2;

		bool inside = true;
		char quadrant[3];
		register int i;
		int whichPlane;
		float maxT[3];
		float candidatePlane[3];

		/* Find candidate planes; this loop can be avoided if
		rays cast all from the eye(assume perpsective view) */
		for (i = 0; i < 3; i++)
			if (origin[i] < aabb.min[i]) {
				quadrant[i] = LEFT;
				candidatePlane[i] = aabb.min[i];
				inside = false;
			}
			else if (origin[i] > aabb.max[i]) {
				quadrant[i] = RIGHT;
				candidatePlane[i] = aabb.max[i];
				inside = false;
			}
			else	{
				quadrant[i] = MIDDLE;
			}

			/* Ray origin inside bounding box */
			if (inside)	{
				result = origin;
				return true;
			}


			/* Calculate T distances to candidate planes */
			for (i = 0; i < 3; i++)
				if (quadrant[i] != MIDDLE && dir[i] != 0.)
					maxT[i] = (candidatePlane[i] - origin[i]) / dir[i];
				else
					maxT[i] = -1.;

			/* Get largest of the maxT's for final choice of intersection */
			whichPlane = 0;
			for (i = 1; i < 3; i++)
				if (maxT[whichPlane] < maxT[i])
					whichPlane = i;

			/* Check final candidate actually inside box */
			if (maxT[whichPlane] < 0.) return false;
			for (i = 0; i < 3; i++)
				if (whichPlane != i) {
					result[i] = origin[i] + maxT[whichPlane] * dir[i];
					if (result[i] < aabb.min[i] || result[i] > aabb.max[i])
						return false;
				}
				else {
					result[i] = candidatePlane[i];
				}
				return true;				/* ray hits box */
	}

}