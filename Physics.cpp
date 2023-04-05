
#include "Physics.h"

#include "Cloth.h"
#include "Particle.h"

//==============================================================================

Physics::Physics() noexcept :
	time_step(0.001f),
	gravity(0.0f, -9.8f, 0.0f),
	cloth(nullptr)
{
}

//==============================================================================

Physics::~Physics() noexcept
{
	delete cloth;
}

//==============================================================================

void Physics::SetGravity(const glm::vec3 &value) noexcept
{
	gravity = value;
}

//==============================================================================

void Physics::GetCloth(std::vector<float> &vertices,
	                   std::vector<float> &normals,
	                   std::vector<float> &uvs,
	                   std::vector<uint>  &indices) const noexcept
{
	vertices = cloth->GetVertices();
	normals  = cloth->GetNormals();
	uvs      = cloth->GetUVs();
	indices  = cloth->GetIndices();
}

//==============================================================================

void Physics::AddCloth(float width, float height, float step)
{
	if (cloth)
	{
		delete cloth;
	}

	cloth = new Cloth(width, height, step);
}

//==============================================================================

bool Physics::Raycast(const Ray &ray, uint &point, glm::vec3 &P) const noexcept
{
	if (cloth)
	{
		return cloth->Raycast(ray, point, P);
	}

	return false;
}

//==============================================================================

void Physics::FixClothPoint(uint index) const noexcept
{
	if (cloth)
	{
		cloth->FixParticle(index);
	}
}

//==============================================================================

void Physics::FreeClothPoint(uint index) const noexcept
{
	if (cloth)
	{
		cloth->FreeParticle(index);
	}
}

//==============================================================================

void Physics::MoveClothPoint(uint index, const glm::vec3 &translation) noexcept
{
	if (cloth)
	{
		cloth->MoveFixedParticle(index, translation);
	}
}

//==============================================================================

void Physics::Simulate() noexcept
{
	if (!cloth)
	{
		return;
	}

	cloth->ClearForces();
	cloth->AddGravity(gravity);

	const auto iterations = 5;
	const auto dt = time_step / iterations;
	for (uint i = 0; i < iterations; i++)
	{
		cloth->PredictPosition(dt);

		cloth->ProjectConstraints(dt);

		cloth->UpdateVelocity(dt, 0.999f);
		cloth->UpdatePosition();
	}

	cloth->CalculateNormals();
}

//==============================================================================
