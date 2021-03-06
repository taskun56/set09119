#include "physics.h"
#include "cPhysicsComponents.h"
#include "collision.h"

#include <glm/glm.hpp>
using namespace std;
using namespace glm;
static vector<cParticle *> physicsScene;
static vector<cCollider *> colliders;

static dvec3 gravity = dvec3(0, -15.8, 0);
const double coef = 0.5;
const double rigidcoef = 0.0;

void ResolveRB(cRigidBody *const b, const collisionInfo &ci, bool which)
{
	// TODO: Fix.
	const double w = (which ? -1.0 : 1.0);

	dvec3 dv = b->position - b->prev_position;
	dvec3 r0 = b->position - ci.position;
	dvec3 v0 = dv + cross(b->angVelocity, r0);

	// I've butchered this formula pretty bad.
	double j = -1.0 * (rigidcoef)+dot(dv, ci.normal) / (dot(ci.normal, ci.normal) * (b->getInverseMass() * 2.0) + dot(ci.normal, (cross(r0, ci.normal))));

	// stop sinking
	j = j - (ci.depth * 0.1);

	// linear impulse
	dvec3 newVel = dv + (b->getInverseMass() * ci.normal * j);
	b->AddLinearImpulse(-newVel);


	// angular impulse
	auto gg = cross(r0, ci.normal);
	b->angVelocity += b->worldInvInertia * cross(r0, ci.normal * j);
}

void ResolveP(cParticle *const b, const collisionInfo &ci, bool which)
{
	const double w = (which ? -1.0 : 1.0);
	b->position += (w * ci.normal) * (ci.depth * 0.5);
	const double currentSpeed = glm::length(b->position - b->prev_position);		
	b->prev_position = b->position + dvec3((-w * ci.normal) * currentSpeed * coef); // TODO
	// The above line needs to be changed. Currently "bounces" the ball in the direction of the plane's normal on collision.
	// Needs to be changed to bounce the ball on the mirror angle to the angle of reflection from the plane's normal
	// e.g. if the ball hits a plane with a normal of 1 (90 degrees), at an angle of 45 degrees the initial angle of deflection should be 45 degrees to the normal of the plane
	// Gravity will round out the curve


}

void Resolve(const collisionInfo &ci)
{

	const auto r1 = ci.c1->GetParent()->getCompatibleComponent<cRigidBody>();
	const auto r2 = ci.c2->GetParent()->getCompatibleComponent<cRigidBody>();
	const auto p1 = ci.c1->GetParent()->getCompatibleComponent<cParticle>();
	const auto p2 = ci.c2->GetParent()->getCompatibleComponent<cParticle>();
	const auto pl1 = ci.c1->GetParent()->getCompatibleComponent<cPlaneCollider>(); // TODO need to refer to pl1 and check the plane's normal for the angle of reflection

	if (r1 != NULL)
	{
		ResolveRB(r1, ci, false);
	}
	else if (p1 != NULL)
	{
		ResolveP(p1, ci, false);
	}

	if (r2 != NULL)
	{
		ResolveRB(r2, ci, true);
	}
	else if (p2 != NULL)
	{
		ResolveP(p2, ci, true);
	}

	if (pl1 != NULL)
	{
		ResolveP(p1, ci, true);
	}
}

void UpdatePhysics(const double t, const double dt) {
	std::vector<collisionInfo> collisions;
	// check for collisions
	{
		for (size_t i = 0; i < colliders.size(); ++i)
		{
			for (size_t j = i + 1; j < colliders.size(); ++j)
			{
				collision::IsColliding(collisions, *colliders[i], *colliders[j]);
			}
		}
	}
	// handle collisions
	{
		for (auto &c : collisions)
		{
			// TODO need to resolve collisions detected here for the plane
			Resolve(c);
		}
	}
	// Integrate
	for (auto &e : physicsScene) {
		e->Integrate(t, dt);
	}
}

std::vector<cParticle *> &GetPhysicsScene() { return physicsScene; }

std::vector<cCollider *> &GetColliders() { return colliders; }

const glm::dvec3 &GetGravity() { return gravity; }

void InitPhysics() {}

void ShutdownPhysics() {}