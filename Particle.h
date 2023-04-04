
#pragma once

//==============================================================================

#include <glm/glm.hpp>

//==============================================================================

class Particle
{
private:
	glm::vec3 previous_position;
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	bool fixed;

public:
	explicit Particle(const glm::vec3 &position) noexcept;

	const glm::vec3 &GetPreviousPosition() const noexcept;
	const glm::vec3 &GetPosition()         const noexcept;
	const glm::vec3 &GetVelocity()         const noexcept;
	const glm::vec3 &GetAcceleration()     const noexcept;
	bool  IsFixed()                        const noexcept;

	void SetPreviousPosition (const glm::vec3 &value) noexcept;
	void SetPosition         (const glm::vec3 &value) noexcept;
	void SetVelocity         (const glm::vec3 &value) noexcept;
	void SetAcceleration     (const glm::vec3 &value) noexcept;
	void SetFixed            (bool value)             noexcept;

	void AddAcceleration(const glm::vec3 &value) noexcept;

	void PredictPosition(float dt) noexcept;
	void UpdateVelocity(float dt, float damping) noexcept;
	void UpdatePosition() noexcept;

	void Move(const glm::vec3 &step) noexcept;
	void MoveFixed(const glm::vec3 &step) noexcept;
};

//==============================================================================
