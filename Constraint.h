
#pragma once

//==============================================================================

#include "Particle.h"

//==============================================================================

typedef unsigned int uint;

//==============================================================================

class Constraint
{
protected:
	Particle *particle1;
	Particle *particle2;
	float compliance;

public:
	Constraint(Particle *p1, Particle *p2, float stiffnes) noexcept;

	Particle *GetParticle1() const noexcept;
	Particle *GetParticle2() const noexcept;

	void SetStiffness(float value) noexcept;

	virtual void Project(float dt, float inv_mass) const noexcept = 0;
};

//==============================================================================

class DistanceConstraint : public Constraint
{
protected:
	float distance;

public:
	DistanceConstraint(Particle *p1, Particle *p2, float stiffness) noexcept;

	void Project(float dt, float inv_mass) const noexcept override;
};

//==============================================================================

class BendConstraint : public Constraint
{
protected:
	Particle *particle3;
	Particle *particle4;
	float angle;

public:
	BendConstraint(Particle* p1, Particle* p2, Particle* p3, Particle* p4,
		float stiffness, float angle) :
		Constraint(p1, p2, stiffness),
		particle3(p3),
		particle4(p4),
		angle(angle)
	{
	}

	float GetAngle() const noexcept;

	void SetAngle(float value) noexcept;

	void Project(float dt, float inv_mass) const noexcept override;
};

//==============================================================================
