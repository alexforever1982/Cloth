
#pragma once

//==============================================================================

#include <vector>

#include <glm/glm.hpp>

#include "Ray.h"

//==============================================================================

class Cloth;

typedef unsigned int uint;

//==============================================================================

class Physics
{
private:
	float time_step;
	glm::vec3 gravity;
	Cloth *cloth;

public:
	Physics() noexcept;
	~Physics() noexcept;

	void SetGravity(const glm::vec3 &value) noexcept;

	void GetCloth(std::vector<float> &vertices,
		          std::vector<float> &normals,
		          std::vector<uint>  &indices) const noexcept;

	void AddCloth(float width, float height, float step);

	bool Raycast(const Ray &ray, uint &point, glm::vec3 &P) const noexcept;

	void FixClothPoint(uint index) const noexcept;
	void FreeClothPoint(uint index) const noexcept;
	void MoveClothPoint(uint index, const glm::vec3 &translation) noexcept;

	void Simulate() noexcept;
};

//==============================================================================
