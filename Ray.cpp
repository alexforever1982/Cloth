
#include "Ray.h"

//==============================================================================

Ray::Ray(const glm::vec3 &origin, const glm::vec3 &end) noexcept :
	origin(origin),
	end(end)
{
}

//==============================================================================

const glm::vec3 &Ray::GetOrigin() const noexcept
{
	return origin;
}

//==============================================================================

const glm::vec3 &Ray::GetEnd() const noexcept
{
	return end;
}

//==============================================================================

bool Ray::TriangleIntersection(const glm::vec3 &A,
	                           const glm::vec3 &B,
	                           const glm::vec3 &C,
	                           float &u,
	                           float &v,
	                           float &t) const
{
	const auto dir = end - origin;

	const auto AB = B - A;
	const auto AC = C - A;

	const auto pvec = glm::cross(dir, AC);
	const auto det = glm::dot(AB, pvec);

	t = FLT_MAX;
	const auto eps = 0.0f;

	if (det > eps)
	{
		const auto tvec = origin - A;

		u = glm::dot(tvec, pvec);
		if ((u < 0.0f) || (u > det))
		{
			return false;
		}

		const auto qvec = glm::cross(tvec, AB);

		v = glm::dot(dir, qvec);
		if ((v < 0.0f) || (u + v > det))
		{
			return false;
		}

		t = glm::dot(AC, qvec);

		if (t < 0.0)
		{
			return false;
		}
	}
	else
	if (det < -eps)
	{
		const auto tvec = origin - A;

		u = glm::dot(tvec, pvec);
		if ((u > 0.0f) || (u < det))
		{
			return false;
		}

		const auto qvec = glm::cross(tvec, AB);

		v = glm::dot(dir, qvec);
		if ((v > 0.0f) || (u + v < det))
		{
			return false;
		}

		t = glm::dot(AC, qvec);

		if (t > 0.0)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	const auto inv_det = 1.0f / det;
	
	u *= inv_det;
	v *= inv_det;
	t *= inv_det;

	return true;
}

//==============================================================================

bool Ray::PlaneIntersection(const glm::vec3 &M, const glm::vec3 &N, glm::vec3 &P) const noexcept
{
	const auto &A = origin;
	const auto &B = end;

	const auto d = glm::dot(M - A, N);
	const auto e = glm::dot(B - A, N);

	const auto eps = FLT_EPSILON;

	if ((e > -eps) && (e < eps))
	{
		if ((d > -eps) && (d < eps))
		{
			P = 0.5f * (A + B);
			return true;
		}

		return false;
	}

	const auto t = d / e;

	if ((t >= 0.0f) && (t <= 1.0f))
	{
		P = (B - A) * t + A;
		return true;
	}

	return false;
}

//==============================================================================
