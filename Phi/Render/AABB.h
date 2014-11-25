#pragma once

namespace Render {

	struct AABB 
	{
		void reset()
		{
			min = glm::vec3(std::numeric_limits<float>::max());
			max = glm::vec3(-std::numeric_limits<float>::max());
		}

		glm::vec3 center() 
		{
			return (max - min) * 0.5f + min;
		}

		
		void add(const glm::vec3& p) 
		{
			min = glm::min(min, p);
			max = glm::max(max, p);
		}

		glm::vec3 nearestPoint(const glm::vec3 &p) const
		{
			glm::vec3 r;

			if (p.x < min.x) {
				r.x = min.x;
			}
			else if (p.x > max.x) {
				r.x = max.x;
			}
			else {
				r.x = p.x;
			}

			if (p.y < min.y) {
				r.y = min.y;
			}
			else if (p.y > max.y) {
				r.y = max.y;
			}
			else {
				r.y = p.y;
			}

			if (p.z < min.z) {
				r.z = min.z;
			}
			else if (p.z > max.z) {
				r.z = max.z;
			}
			else {
				r.z = p.z;
			}
			return r;
		}

		float distanceFrom(const glm::vec3 &p) const
		{
			glm::vec3 r = nearestPoint(p);
			float distance = glm::length(r - p);
			if (distance < std::numeric_limits<float>::epsilon())
				distance = std::numeric_limits<float>::epsilon();
			return distance;
		}


		glm::vec3 min;
		glm::vec3 max;
	};

}