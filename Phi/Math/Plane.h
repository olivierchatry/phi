#pragma once


namespace Math
{
	struct Plane
	{
		void fromPointNormal(const glm::vec3 &pt, const glm::vec3 &normal)
		{
			n = normal;
			d = -glm::dot(normal, pt);
			normalize();
		}

		void normalize()
		{
			float length = 1.0f / glm::length(n);
			n *= length;
			d *= length;
		}

		bool intersects(const glm::vec3& start, const glm::vec3& direction, glm::vec3& result) const
		{
			float dotProduct = glm::dot(direction, n);
			float limit = std::numeric_limits<float>::epsilon();
			if (dotProduct < limit && dotProduct > -limit)
				return false;

			float t = (-d - glm::dot(start, n)) / dotProduct;
			if (t < 0)
				return false;

			result = (direction * t) + start;
			return true;
		}


		glm::vec3	n;
		float		d;
	};
}