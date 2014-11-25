#pragma once

namespace Utils 
{
	void CopyBuffer(const GLfloat* source, GLfloat* destination, int sourceStride, int sourceOffset, int destinationStride, int destinationOffset, int count, int elementSize);
	void GenerateNormals(GLfloat* vs, int stride, int count, int offsetVertex, int offsetNormal);
	void GenerateNormals(unsigned short* index, GLfloat* vs, int stride, int count, int offsetVertex, int offsetNormal, bool zeroNormalsBefore);
	glm::vec3 ExtractCameraPos(const glm::mat4& modelView);
};