
#pragma once

//==============================================================================

#include <vector>

#include <glm/glm.hpp>

#include "Constraint.h"
#include "Ray.h"

//==============================================================================

typedef unsigned int uint;

class Particle;
class Topology;

//==============================================================================

class Cloth
{
private:
	std::vector<Particle*> particles;
	std::vector<uint> indices;
	std::vector<glm::vec3> normals;
	float inv_mass;

	Topology *topology;

	std::vector<DistanceConstraint> distance_constraints;
	std::vector<BendConstraint> bend_constraints;

private:
	void AddNoise(float value) noexcept;

	void GenerateConstraints() noexcept;
	void GenerateDistanceConstraints() noexcept;
	void GenerateBendConstraints() noexcept;

public:
	Cloth(float width, float height, float step) noexcept;
	Cloth(const std::vector<float> &vertices, const std::vector<uint> &indices) noexcept;
	~Cloth() noexcept;

	std::vector<Particle*> &GetParticles();
	Particle *GetParticle(const uint index) const;

	std::vector<float> GetVertices()      const noexcept;
	std::vector<float> GetNormals()       const noexcept;
	const std::vector<uint> &GetIndices() const noexcept;

	void SetMass(float value) noexcept;
	void SetStiffness(float value) noexcept;
	void SetBend(float value) noexcept;

	void CalculateNormals() noexcept;
	void ClearForces() noexcept;
	void AddGravity(const glm::vec3 &value) noexcept;

	void PredictPosition(float dt) noexcept;
	void UpdateVelocity(float dt, float damping) noexcept;
	void UpdatePosition() noexcept;

	void ProjectConstraints(float dt) noexcept;

	bool Raycast(const Ray &ray, uint &point, glm::vec3 &P) const noexcept;

	void FixParticle(uint index) noexcept;
	void FreeParticle(uint index) noexcept;
	void MoveParticle(uint index, const glm::vec3 &translation) noexcept;
	void MoveFixedParticle(uint index, const glm::vec3 &translation) noexcept;
};

//==============================================================================
