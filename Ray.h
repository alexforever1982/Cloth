
#pragma once

//==============================================================================

#include <glm/glm.hpp>

//==============================================================================

class Ray
{
private:
	glm::vec3 origin;
	glm::vec3 end;

public:
	Ray(const glm::vec3 &origin, const glm::vec3 &end) noexcept;

	const glm::vec3 &GetOrigin() const noexcept;
	const glm::vec3 &GetEnd()    const noexcept;

	bool TriangleIntersection(const glm::vec3 &A,
		                      const glm::vec3 &B,
		                      const glm::vec3 &C,
		                      float &u,
		                      float &v,
		                      float &t) const;

	bool PlaneIntersection(const glm::vec3 &M, const glm::vec3 &N, glm::vec3 &P) const noexcept;
};

//==============================================================================
