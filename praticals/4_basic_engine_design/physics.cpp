#include "physics.h"
#include <glm/glm.hpp>
using namespace std;
using namespace glm;
static vector<cPhysics *> physicsScene;

static dvec3 gravity = dvec3(0, -10.0, 0);

cPhysics::cPhysics() : pm(POINT), Component("Physics")
{ 
	physicsScene.push_back(this); 
}

cPhysics::~cPhysics() 
{
  auto position = std::find(physicsScene.begin(), physicsScene.end(), this);
  if (position != physicsScene.end()) 
  {
    physicsScene.erase(position);
  }
}

void cPhysics::Update(double delta) 
{
  for (auto &e : physicsScene) 
  {
    e->GetParent()->SetPosition(e->position);
  }
}

void cPhysics::SetParent(Entity *p) {
  Component::SetParent(p);
  position = Ent_->GetPosition();
  prev_position = position;
}

void UpdatePhysics(const double t, const double dt) 
{
	int physItem = 0;

	for (auto &e : physicsScene) 
	{
		e->Render();
		// calculate velocity from current and previous position
		dvec3 velocity = e->position - e->prev_position;
		// set previous position to current position
		e->prev_position = e->position;
		// position += v + a * (dt^2)
		e->position += velocity + gravity * pow(dt, 2);

		if (e->position.y <= 1.0f)
		{
			e->prev_position = e->position + (e->position - e->prev_position);
		}

		// If the current item y position is too close to the previous item y position in the scenelist
		// Only works in this example, but a simple test
		/*if ((physItem != 0) && (e->position.y <= physicsScene.at(physItem)->position.y))
		{
			e->prev_position = e->position + (e->position - e->prev_position);
		}
		physItem += 1;*/
	}
}

void InitPhysics() {}

void ShutdownPhysics() {}
