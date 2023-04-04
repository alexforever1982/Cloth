
#include "Constraint.h"

//==============================================================================

Constraint::Constraint(Particle *p1, Particle *p2, float stiffness) noexcept :
particle1(p1),
particle2(p2),
compliance(1.0f / stiffness)
{
}

//==============================================================================

Particle *Constraint::GetParticle1() const noexcept
{
	return particle1;
}

//==============================================================================

Particle *Constraint::GetParticle2() const noexcept
{
	return particle2;
}

//==============================================================================

void Constraint::SetStiffness(float value) noexcept
{
	compliance = 1.0f / value;
}

//==============================================================================

DistanceConstraint::DistanceConstraint(Particle *p1, Particle *p2, float stiffness) noexcept :
	Constraint(p1, p2, stiffness)
{
	const auto &A = p1->GetPosition();
	const auto &B = p2->GetPosition();

	const auto AB = B - A;
	distance = sqrt(glm::dot(AB, AB));
}

//==============================================================================

void DistanceConstraint::Project(float dt, float inv_mass) const noexcept
{
	const auto inv_mass_sum = inv_mass + inv_mass;

	const auto &p1 = particle1->GetPosition();
	const auto &p2 = particle2->GetPosition();

	const auto L = p1 - p2;
	const auto length = sqrt(glm::dot(L, L));
	const auto constraint = length - distance;

	const auto alpha = compliance / (dt * dt);

	const auto delta_lambda = -constraint / (inv_mass_sum + alpha);

	const auto dp = delta_lambda * L / (length + 1e-30f);

	particle1->Move(+inv_mass * dp);
	particle2->Move(-inv_mass * dp);
}

//==============================================================================

float BendConstraint::GetAngle() const noexcept
{
	return angle;
}

//==============================================================================

void BendConstraint::SetAngle(float value) noexcept
{
	angle = value;
}

//==============================================================================

void BendConstraint::Project(float dt, float inv_mass) const noexcept
{
	const auto &P1 = particle1->GetPosition();
	const auto &P2 = particle2->GetPosition();
	const auto &P3 = particle3->GetPosition();
	const auto &P4 = particle4->GetPosition();

	const auto e = P2 - P1;
	const auto elen = sqrt(glm::dot(e, e));

	if (elen < 1e-6)
	{
		return;
	}

	const auto inv_elen = 1.0f / elen;

	auto n1 = glm::cross(P1 - P3, P2 - P3);
	auto n2 = glm::cross(P2 - P4, P1 - P4);

	const auto n1_length2 = glm::dot(n1, n1);
	const auto n2_length2 = glm::dot(n2, n2);

	if ((n1_length2 < 1e-10) ||
		(n2_length2 < 1e-10))
	{
		return;
	}

	n1 /= n1_length2;
	n2 /= n2_length2;

	const auto d3 = elen * n1;
	const auto d4 = elen * n2;
	const auto d1 = (glm::dot(P3 - P2, e) * n1 + glm::dot(P4 - P2, e) * n2) * inv_elen;
	const auto d2 = (glm::dot(P1 - P3, e) * n1 + glm::dot(P1 - P4, e) * n2) * inv_elen;

	n1 = glm::normalize(n1);
	n2 = glm::normalize(n2);

	auto dot = glm::dot(n1, n2);
	if (dot < -1.0f) dot = -1.0f;
	if (dot >  1.0f) dot =  1.0f;

	const auto phi = acos(dot);

	const auto sum = inv_mass * (glm::dot(d1, d1) + glm::dot(d2, d2) + 
		                         glm::dot(d3, d3) + glm::dot(d4, d4));

	const auto alpha = compliance / (dt * dt);

	constexpr auto PI = 3.1415927f;
	const auto a = PI - angle;

	const auto constraint = phi - a;
	auto delta_lambda = -constraint / (sum + alpha);

	if (((phi - fabs(a)) > 0.0f) && (glm::dot(glm::cross(n1, n2), e) > 0.0f))
	{
		delta_lambda = -delta_lambda;
	}

	const auto dp1 = inv_mass * delta_lambda * d1;
	const auto dp2 = inv_mass * delta_lambda * d2;
	const auto dp3 = inv_mass * delta_lambda * d3;
	const auto dp4 = inv_mass * delta_lambda * d4;

	particle1->Move(dp1);
	particle2->Move(dp2);
	particle3->Move(dp3);
	particle4->Move(dp4);
}

//==============================================================================
