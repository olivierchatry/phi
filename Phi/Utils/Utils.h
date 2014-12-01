#pragma once

#include <Render/AABB.h>

namespace Utils 
{
	void CopyBuffer(const GLfloat* source, GLfloat* destination, int sourceStride, int sourceOffset, int destinationStride, int destinationOffset, int count, int elementSize);
	void GenerateNormals(GLfloat* vs, int stride, int count, int offsetVertex, int offsetNormal);
	void GenerateNormals(GLushort* index, GLfloat* vs, int stride, int count, int offsetVertex, int offsetNormal, bool zeroNormalsBefore);
	glm::vec3 ExtractCameraPos(const glm::mat4& modelView);
	void GenerateCube(Render::AABB& aabb, std::vector<GLfloat>& v);
    bool RayIntersectBoundingBox(const glm::vec3& origin, const glm::vec3& dir, const Render::AABB& aabb);
};