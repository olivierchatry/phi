#include <PreCompile.h>
#include <Utils/Utils.h>

namespace Utils
{
	void GenerateNormals(GLfloat* vs, int stride, int count, int offsetVertex, int offsetNormal)
	{

		while (count--)
		{
			glm::fvec3* v1 = (glm::fvec3*) (vs + offsetVertex);
			glm::fvec3* v2 = (glm::fvec3*) (vs + stride + offsetVertex);
			glm::fvec3* v3 = (glm::fvec3*) (vs + stride + stride + offsetVertex);

			glm::fvec3 v21 = *v2 - *v1;
			glm::fvec3 v31 = *v3 - *v1;
			glm::fvec3 normal = glm::normalize(glm::cross(v21, v31));

			*((glm::fvec3*) (vs + offsetNormal)) = normal;
			*((glm::fvec3*) (vs + stride + offsetNormal)) = normal;
			*((glm::fvec3*) (vs + stride + stride + offsetNormal)) = normal;

			vs += stride + stride + stride;
		}
	}

	void GenerateNormals(unsigned short* index, GLfloat* vs, int stride, int count, int offsetVertex, int offsetNormal, bool zeroNormalsBefore)
	{
		if (zeroNormalsBefore)
		{
			int		temp = count;
			unsigned short*	tempIndex = index;

			while (temp--)
				*((glm::vec3*) (vs + *(tempIndex++) * stride + offsetNormal)) = glm::vec3(0);
		}
		{
			int		temp = count / 3;
			unsigned short*	tempIndex = index;

			while (temp--)
			{
				int i1 = *(tempIndex++) * stride;
				int i2 = *(tempIndex++) * stride;
				int i3 = *(tempIndex++) * stride;

				glm::vec3* v1 = (glm::vec3*) (vs + i1 + offsetVertex);
				glm::vec3* v2 = (glm::vec3*) (vs + i2 + offsetVertex);
				glm::vec3* v3 = (glm::vec3*) (vs + i3 + offsetVertex);

				glm::vec3 v21 = *v2 - *v1;
				glm::vec3 v31 = *v3 - *v1;
				glm::vec3 normal = glm::normalize(glm::cross(v21, v31));

				*((glm::vec3*) (vs + i1 + offsetNormal)) += normal;
				*((glm::vec3*) (vs + i2 + offsetNormal)) += normal;
				*((glm::vec3*) (vs + i3 + offsetNormal)) += normal;
			}
		}
		{
			int				temp = count;
			unsigned short*	tempIndex = index;

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
			source		+= sourceStride;
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
}