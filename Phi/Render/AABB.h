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
        
        glm::vec3 corner(int i)
        {
            switch (i)
            {
                case 4:
                    return min;
                case 5:
                    return glm::vec3(max.x, min.y, min.z);
                case 6:
                    return glm::vec3(max.x, max.y, min.z);
                case 7:
                    return glm::vec3(min.x, max.y, min.z);
                case 0:
                    return glm::vec3(min.x, min.y, max.z);
                case 1:
                    return glm::vec3(max.x, min.y, max.z);
                case 2:
                    return max;
                case 3:
                    return glm::vec3(min.x, max.y, max.z);
                default:
                    return glm::vec3(0);
            }
        }
		void expand(glm::vec3 times)
		{
			glm::vec3 cent = center();
			glm::vec3 dir = max - cent;

			max = max + dir * times;
			min = min - dir * times;
		}

        void add(const AABB& aabb)
        {
            min = glm::min(min, aabb.min);
            max = glm::max(max, aabb.min);
            
            min = glm::min(min, aabb.max);
            max = glm::max(max, aabb.max);
        }
        
		void add(const glm::vec3& p) 
		{
			min = glm::min(min, p);
			max = glm::max(max, p);
		}

        glm::vec3 size()
        {
            glm::vec3 ret = (max - min);
            ret.x = glm::sqrt(ret.x * ret.x);
            ret.y = glm::sqrt(ret.y * ret.y);
            ret.z = glm::sqrt(ret.z * ret.z);
            
            return ret;
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