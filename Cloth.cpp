
#include "Cloth.h"

#include "Particle.h"
#include "Topology.h"

//==============================================================================

void Cloth::AddNoise(float value) noexcept
{
	for (auto &particle : particles)
	{
		auto p = particle->GetPosition();
		p.z += value * ((static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) - 0.5f);
		particle->SetPosition(p);
		particle->SetPreviousPosition(p);
	}
}

//==============================================================================

void Cloth::GenerateConstraints() noexcept
{
	GenerateDistanceConstraints();
	GenerateBendConstraints();
}

//==============================================================================

void Cloth::GenerateDistanceConstraints() noexcept
{
	distance_constraints.clear();

	const auto &edges = topology->GetEdges();
	for (const auto &edge : edges)
	{
		const auto ind1 = edge.ind1;
		const auto ind2 = edge.ind2;

		const auto p1 = GetParticle(ind1);
		const auto p2 = GetParticle(ind2);

		distance_constraints.emplace_back(p1, p2, 1.0f);
	}
}

//==============================================================================

void Cloth::GenerateBendConstraints() noexcept
{
	bend_constraints.clear();

	const auto &edges = topology->GetEdges();
	for (const auto &edge : edges)
	{
		if (!edge.boundary)
		{
			const auto p1 = GetParticle(edge.ind1);
			const auto p2 = GetParticle(edge.ind2);
			const auto p3 = GetParticle(edge.ind3);
			const auto p4 = GetParticle(edge.ind4);

			constexpr auto PI = 3.1415927f;
			bend_constraints.emplace_back(p1, p2, p3, p4, 1.0f, PI);
		}
	}
}

//==============================================================================

Cloth::Cloth(float width, float height, float step) noexcept
{
	const auto nx = static_cast<uint>(width  / step);
	const auto ny = static_cast<uint>(height / step);

	const auto size = (nx + 1) * (ny + 1);
	particles.reserve(size);

	for (uint i = 0; i < nx + 1; i++)
	{
		for (uint j = 0; j < ny + 1; j++)
		{
			const auto x = i * step - width / 2.0f;
			const auto y = j * step;

			particles.push_back(new Particle(glm::vec3(x, y, 0.0f)));
		}
	}

	const auto triangles = 2 * (nx * ny);
	indices.reserve(3 * triangles);

	for (uint i = 0; i < nx; i++)
	{
		for (uint j = 0; j < ny; j++)
		{
			if (i < nx / 2)
			{
				indices.push_back((i + 0) * (ny + 1) + (j + 0));
				indices.push_back((i + 1) * (ny + 1) + (j + 0));
				indices.push_back((i + 0) * (ny + 1) + (j + 1));

				indices.push_back((i + 1) * (ny + 1) + (j + 1));
				indices.push_back((i + 0) * (ny + 1) + (j + 1));
				indices.push_back((i + 1) * (ny + 1) + (j + 0));
			}
			else
			{
				indices.push_back((i + 0) * (ny + 1) + (j + 0));
				indices.push_back((i + 1) * (ny + 1) + (j + 0));
				indices.push_back((i + 1) * (ny + 1) + (j + 1));

				indices.push_back((i + 0) * (ny + 1) + (j + 1));
				indices.push_back((i + 0) * (ny + 1) + (j + 0));
				indices.push_back((i + 1) * (ny + 1) + (j + 1));
			}
		}
	}

	uvs.reserve(particles.size());
	for (const auto particle : particles)
	{
		const auto &P = particle->GetPosition();
		uvs.emplace_back(P.x, P.y);
	}
	
	const auto vertices_size = static_cast<uint>(particles.size());
	topology = new Topology(vertices_size, indices);

	AddNoise(0.001f);

	CalculateNormals();
	GenerateConstraints();

	SetMass(1.0f);
	SetStiffness(1.0e3f);
	SetBend(0.0001f);
}

//==============================================================================

Cloth::Cloth(const std::vector<float> &vertices, const std::vector<uint> &indices) noexcept :
	indices(indices)
{
	particles.reserve(vertices.size() / 3);

	for (size_t i = 0; i < vertices.size(); i += 3)
	{
		const auto x = vertices[i + 0];
		const auto y = vertices[i + 1];
		const auto z = vertices[i + 2];

		particles.push_back(new Particle(glm::vec3(x, y, z)));
	}

	uvs.reserve(particles.size());
	for (const auto particle : particles)
	{
		const auto &P = particle->GetPosition();
		uvs.emplace_back(P.x, P.y);
	}

	const auto vertices_size = static_cast<uint>(particles.size());
	topology = new Topology(vertices_size, indices);

	AddNoise(0.01f);

	SetMass(1.0f);
	SetStiffness(1.0e3f);
	SetBend(0.001f);

	CalculateNormals();
	GenerateConstraints();
}

//==============================================================================

Cloth::~Cloth() noexcept
{
	for (const auto particle : particles)
	{
		delete particle;
	}

	delete topology;
}

//==============================================================================

std::vector<Particle*> &Cloth::GetParticles()
{
	return particles;
}

//==============================================================================

Particle *Cloth::GetParticle(const uint index) const
{
	return particles[index];
}

//==============================================================================

std::vector<float> Cloth::GetVertices() const noexcept
{
	std::vector<float> V;
	V.reserve(3 * particles.size());

	for (const auto particle : particles)
	{
		const auto &p = particle->GetPosition();

		V.push_back(p.x);
		V.push_back(p.y);
		V.push_back(p.z);
	}

	return V;
}

//==============================================================================

std::vector<float> Cloth::GetNormals() const noexcept
{
	std::vector<float> N;
	N.reserve(3 * normals.size());

	for (const auto &n : normals)
	{
		N.push_back(n.x);
		N.push_back(n.y);
		N.push_back(n.z);
	}

	return N;
}

//==============================================================================

std::vector<float> Cloth::GetUVs() const noexcept
{
	std::vector<float> UV;
	UV.reserve(2 * uvs.size());

	for (const auto &uv : uvs)
	{
		UV.push_back(uv.x);
		UV.push_back(uv.y);
	}

	return UV;
}

//==============================================================================

const std::vector<uint> &Cloth::GetIndices() const noexcept
{
	return indices;
}

//==============================================================================

void Cloth::SetMass(float value) noexcept
{
	const auto mass = value / static_cast<float>(particles.size());
	inv_mass = 1.0f / mass;
}

//==============================================================================

void Cloth::SetStiffness(float value) noexcept
{
	for (auto &constraint : distance_constraints)
	{
		constraint.SetStiffness(value);
	}
}

//==============================================================================

void Cloth::SetBend(float value) noexcept
{
	for (auto &constraint : bend_constraints)
	{
		constraint.SetStiffness(value);
	}
}

//==============================================================================

void Cloth::CalculateNormals() noexcept
{
	normals.resize(particles.size());
	std::fill(normals.begin(), normals.end(), glm::vec3(0.0f, 0.0f, 0.0f));

	for (size_t i = 0; i < indices.size(); i += 3)
	{
		const auto ind1 = indices[i + 0];
		const auto ind2 = indices[i + 1];
		const auto ind3 = indices[i + 2];

		const auto particle1 = particles[ind1];
		const auto particle2 = particles[ind2];
		const auto particle3 = particles[ind3];

		const auto &p1 = particle1->GetPosition();
		const auto &p2 = particle2->GetPosition();
		const auto &p3 = particle3->GetPosition();

		const auto n = glm::cross(p2 - p1, p3 - p1);

		normals[ind1] += n;
		normals[ind2] += n;
		normals[ind3] += n;
	}

	for (size_t i = 0; i < normals.size(); i++)
	{
		auto &n = normals[i];
		n = glm::normalize(n);
	}
}

//==============================================================================

void Cloth::ClearForces() noexcept
{
	for (auto &particle : particles)
	{
		particle->SetAcceleration(glm::vec3(0.0f, 0.0f, 0.0f));
	}
}

//==============================================================================

void Cloth::AddGravity(const glm::vec3 &value) noexcept
{
	for (auto particle : particles)
	{
		particle->AddAcceleration(value);
	}
}

//==============================================================================

void Cloth::PredictPosition(float dt) noexcept
{
	for (auto particle : particles)
	{
		particle->PredictPosition(dt);
	}
}

//==============================================================================

void Cloth::UpdateVelocity(float dt, float damping) noexcept
{
	for (auto particle : particles)
	{
		particle->UpdateVelocity(dt, damping);
	}
}

//==============================================================================

void Cloth::UpdatePosition() noexcept
{
	for (auto particle : particles)
	{
		particle->UpdatePosition();
	}
}

//==============================================================================

void Cloth::ProjectConstraints(float dt) noexcept
{
	for (auto &constraint : distance_constraints)
	{
		constraint.Project(dt, inv_mass);
	}

	for (auto &constraint : bend_constraints)
	{
		constraint.Project(dt, inv_mass);
	}
}

//==============================================================================

bool Cloth::Raycast(const Ray &ray, uint &point, glm::vec3 &P) const noexcept
{
	auto find = false;
	auto tmin = FLT_MAX;

	for (uint i = 0; i < indices.size(); i += 3)
	{
		const auto ind1 = indices[i + 0];
		const auto ind2 = indices[i + 1];
		const auto ind3 = indices[i + 2];

		const auto p1 = GetParticle(ind1);
		const auto p2 = GetParticle(ind2);
		const auto p3 = GetParticle(ind3);

		const auto &A = p1->GetPosition();
		const auto &B = p2->GetPosition();
		const auto &C = p3->GetPosition();

		float u, v, t;
		if (ray.TriangleIntersection(A, B, C, u, v, t))
		{
			if ((t < tmin) && (t > 0.0f))
			{
				find = true;
				tmin = t;

				const auto w = 1.0f - u - v;

				point = ind1;

				if ((u >= w) && (u >= v))
				{
					point = ind2;
				}
				else
				if ((v >= w) && (v >= u))
				{
					point = ind3;
				}

				P = w * A + u * B + v * C;
			}
		}
	}

	return find;
}

//==============================================================================

void Cloth::FixParticle(uint index) noexcept
{
	if (index < particles.size())
	{
		particles[index]->SetFixed(true);
	}
}

//==============================================================================

void Cloth::FreeParticle(uint index) noexcept
{
	if (index < particles.size())
	{
		particles[index]->SetFixed(false);
	}
}

//==============================================================================

void Cloth::MoveParticle(uint index, const glm::vec3 &translation) noexcept
{
	const auto particle = GetParticle(index);
	particle->Move(translation);
}

//==============================================================================

void Cloth::MoveFixedParticle(uint index, const glm::vec3 &translation) noexcept
{
	const auto particle = GetParticle(index);
	particle->MoveFixed(translation);
}

//==============================================================================
