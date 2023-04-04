
#include "Particle.h"

//==============================================================================

Particle::Particle(const glm::vec3 &position) noexcept :
	previous_position(position),
	position(position),
	velocity(0.0f, 0.0f, 0.0f),
	acceleration(0.0f, 0.0f, 0.0f),
	fixed(false)
{
}

//==============================================================================

const glm::vec3 &Particle::GetPreviousPosition() const noexcept
{
	return previous_position;
}

//==============================================================================

const glm::vec3 &Particle::GetPosition() const noexcept
{
	return position;
}

//==============================================================================

const glm::vec3 &Particle::GetVelocity() const noexcept
{
	return velocity;
}

//==============================================================================

const glm::vec3 &Particle::GetAcceleration() const noexcept
{
	return acceleration;
}

//==============================================================================

bool Particle::IsFixed() const noexcept
{
	return fixed;
}

//==============================================================================

void Particle::SetPreviousPosition(const glm::vec3 &value) noexcept
{
	previous_position = value;
}

//==============================================================================

void Particle::SetPosition(const glm::vec3 &value) noexcept
{
	position = value;
}

//==============================================================================

void Particle::SetVelocity(const glm::vec3 &value) noexcept
{
	velocity = value;
}

//==============================================================================

void Particle::SetAcceleration(const glm::vec3 &value) noexcept
{
	acceleration = value;
}

//==============================================================================

void Particle::SetFixed(bool value) noexcept
{
	fixed = value;
}

//==============================================================================

void Particle::AddAcceleration(const glm::vec3 &value) noexcept
{
	acceleration += value;
}

//==============================================================================

void Particle::PredictPosition(float dt) noexcept
{
	if (!fixed)
	{
		position = previous_position + (velocity + acceleration * dt) * dt;
	}
}

//==============================================================================

void Particle::UpdateVelocity(float dt, float damping) noexcept
{
	velocity = damping * (position - previous_position) / dt;
}

//==============================================================================

void Particle::UpdatePosition() noexcept
{
	previous_position = position;
}

//==============================================================================

void Particle::Move(const glm::vec3 &step) noexcept
{
	if (!fixed)
	{
		position += step;
	}
}

//==============================================================================

void Particle::MoveFixed(const glm::vec3 &step) noexcept
{
	position += step;
}

//==============================================================================
