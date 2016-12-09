#include "game.h"
#include "physics.h"
#include "cPhysicsComponents.h"
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <graphics_framework.h>
#include <phys_utils.h>
#include <thread>

using namespace std;
using namespace graphics_framework;
using namespace glm;
#define physics_tick 1.0 / 60.0

static vector<unique_ptr<Entity>> SceneList;
static unique_ptr<Entity> floorEnt;
static cParticle* ballP = NULL;
static bool norms = false;

unique_ptr<Entity> CreateParticle()
{
	unique_ptr<Entity> ent(new Entity());
	ent->SetPosition(vec3(-2.0, 5.0 + (double)(rand() % 200) / 5.0, 2.0));
	ballP = new cParticle();
	unique_ptr<Component> physComponent(ballP);
	//unique_ptr<Component> physComponent(new cParticle());
	unique_ptr<cShapeRenderer> renderComponent(new cShapeRenderer(cShapeRenderer::SPHERE));
	renderComponent->SetColour(phys::RandomColour());
	ent->AddComponent(physComponent);
	ent->SetName("BALL");
	ent->AddComponent(unique_ptr<Component>(new cSphereCollider()));
	ent->AddComponent(unique_ptr<Component>(move(renderComponent)));
	return ent;
}
unique_ptr<Entity> CreateBox(const vec3 &position)
{
	unique_ptr<Entity> ent(new Entity());
	ent->SetPosition(position);
	ent->SetRotation(angleAxis(-45.0f, vec3(1, 0, 0)));
	unique_ptr<Component> physComponent(new cRigidCube());
	unique_ptr<cShapeRenderer> renderComponent(new cShapeRenderer(cShapeRenderer::BOX));
	renderComponent->SetColour(phys::RandomColour());
	ent->AddComponent(physComponent);
	ent->SetName("Cuby");
	ent->AddComponent(unique_ptr<Component>(new cBoxCollider()));
	ent->AddComponent(unique_ptr<Component>(move(renderComponent)));
	return ent;
}

unique_ptr<Entity> CreatePlane()
{
	unique_ptr<Entity> ent(new Entity());
	ent->SetPosition(vec3(0.0f, 0.0f, 0.0f));
	unique_ptr<Component> physComponent(new cRigidPlane());
	unique_ptr<cShapeRenderer> renderComponent(new cShapeRenderer(cShapeRenderer::PLANE));
	renderComponent->SetColour(phys::RandomColour());
	ent->AddComponent(physComponent);
	ent->AddComponent(unique_ptr<Component>(new cPlaneCollider()));
	ent->AddComponent(unique_ptr<Component>(move(renderComponent)));
	ent->SetName("Plane");
	return ent;
}

bool update(double delta_time)
{
	static double t = 0.0;
	static double accumulator = 0.0;
	accumulator += delta_time;

	// Ball "movment" controls. Add impulse in the given axis
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_W))		{ ballP->AddLinearForce(vec3(-20.0f, 0.0f, 0.0f));	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_S))		{ ballP->AddLinearForce(vec3(20.0f, 0.0f, 0.0f));	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_A))		{ ballP->AddLinearForce(vec3(0.0f, 0.0f, 20.0f));	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_D))		{ ballP->AddLinearForce(vec3(0.0f, 0.0f, -20.0f));	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_SPACE))	{ ballP->AddLinearForce(vec3(0.0f, 20.0f, 0.0f));	}

	// Toggle normals for all "plane" objects
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_BACKSPACE))
	{
		(norms == true ? norms = false : norms = true);
	}

	while (accumulator > physics_tick)
	{
		UpdatePhysics(t, physics_tick); // here is where the collisions are eventually detected as the physics are updated each interation
		accumulator -= physics_tick;
		t += physics_tick;
	}

	for (auto &e : SceneList)
	{
		e->Update(delta_time);
	}

	phys::Update(delta_time);
	cout << endl << endl << endl << SceneList.at(0)->GetPosition().y << endl;
	cout << SceneList.at(0)->GetName() << endl;
	return true;
}

bool load_content()
{
	phys::Init();
	SceneList.push_back(move(CreateParticle()));
	SceneList.push_back(CreatePlane());
	phys::SetCameraPos(vec3(20.0f, 10.0f, 20.0f));
	phys::SetCameraTarget(vec3(0, 10.0f, 0));
	InitPhysics();
	return true;
}

void renderAxis()
{
	// TODO add arrows on the end - unnecessary inclusion for later
	phys::DrawLine(vec3(0.0f, 15.0f, 0.0f), vec3(5.0f, 15.0f, 0.0f), true, RED);	// X AXIS
	phys::DrawLine(vec3(0.0f, 15.0f, 0.0f), vec3(0.0f, 20.0f, 0.0f), true, GREEN);	// Y AXIS
	phys::DrawLine(vec3(0.0f, 15.0f, 0.0f), vec3(0.0f, 15.0f, 5.0f), true, BLUE);	// Z AXIS
}

bool render()
{
	for (auto &e : SceneList)
	{
		e->Render();
	}
	phys::DrawScene();
	if (norms == true)
	{
		for (auto &e : SceneList)
		{
			if (e->GetName() == "Plane")
			{
				vec3 normal = e->getCompatibleComponent<cPlaneCollider>()->normal;
				vec3 wp = e->getCompatibleComponent<cPlaneCollider>()->GetParent()->GetPosition();
				vec3 temp_lp = wp + (normal * vec3(10));
				phys::DrawLine(wp, temp_lp, 30, RED);
				//e->getCompatibleComponent<cPlaneCollider>()->GetParent()->
			}
		}
	}

	// Render visual axis indicator
	renderAxis();

	return true;
}

void main()
{
	// Create application
	app application;
	// Set load content, update and render methods
	application.set_load_content(load_content);
	application.set_update(update);
	application.set_render(render);
	// Run application
	application.run();
}

