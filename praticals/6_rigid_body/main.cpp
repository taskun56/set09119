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
static cParticle* ballP = NULL;
static bool norms = false;
double xpos1;
double ypos1;
static double cursor_x;
static double cursor_y;
geometry ramp_b_base;				// base of top ramp
material mat;
effect simple_eff;

unique_ptr<Entity> CreateParticle()
{
	unique_ptr<Entity> ent(new Entity());
	ent->SetPosition(vec3(-2.0, 5.0 + (double)(rand() % 200) / 5.0, 2.0));
	ballP = new cParticle();
	unique_ptr<Component> physComponent(ballP);
	//unique_ptr<Component> physComponent(new cParticle());
	unique_ptr<cShapeRenderer> renderComponent(new cShapeRenderer(cShapeRenderer::SPHERE, phys::RandomColour()));
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
	unique_ptr<cShapeRenderer> renderComponent(new cShapeRenderer(cShapeRenderer::BOX, phys::RandomColour()));
	renderComponent->SetColour(phys::RandomColour());
	ent->AddComponent(physComponent);
	ent->SetName("Cuby");
	ent->AddComponent(unique_ptr<Component>(new cBoxCollider()));
	ent->AddComponent(unique_ptr<Component>(move(renderComponent)));
	return ent;
}
unique_ptr<Entity> CreatePlane(vec3 pos)
{
	unique_ptr<Entity> ent(new Entity());
	ent->SetPosition(pos);
	ent->setSize(50, 50);
	ent->setTexture("resources\\wood.png");
	unique_ptr<Component> physComponent(new cRigidPlane());
	unique_ptr<cShapeRenderer> renderComponent(new cShapeRenderer(cShapeRenderer::PLANE, phys::RandomColour()));
	renderComponent->SetColour(phys::RandomColour());
	ent->AddComponent(physComponent);
	ent->AddComponent(unique_ptr<Component>(new cPlaneCollider(glm::dvec3(0.0f, 1.0f, 0.0f))));
	ent->AddComponent(unique_ptr<Component>(move(renderComponent)));
	ent->SetName("Plane");
	return ent;
}
unique_ptr<Entity> CreateRamp(vec3 pos, phys::RGBAInt32 col)
{
	unique_ptr<Entity> ent(new Entity());
	ent->SetPosition(pos);
	ent->setColor(col);
	ent->setSize(12, 12);
	unique_ptr<Component> physComponent(new cRigidPlane());
	unique_ptr<cShapeRenderer> renderComponent(new cShapeRenderer(cShapeRenderer::RAMP, col));
	renderComponent->SetColour(phys::RandomColour());
	ent->AddComponent(physComponent);
	ent->AddComponent(unique_ptr<Component>(new cPlaneCollider(glm::dvec3(0.0f, 1.0f, 0.0f))));
	ent->AddComponent(unique_ptr<Component>(move(renderComponent)));
	ent->SetName("Plane");
	return ent;
}


bool update(double delta_time)
{
	//ballP->AddLinearForce(dvec3(0.0, -15.0, 0.0));
	static double t = 0.0;
	static double accumulator = 0.0;
	accumulator += delta_time;

	// screen space ratio
	static double ratio_width = quarter_pi<float>() / static_cast<float>(renderer::get_screen_width());
	static double ratio_height = (quarter_pi<float>() * (static_cast<float>(renderer::get_screen_height()) / static_cast<float>(renderer::get_screen_width()))) / static_cast<float>(renderer::get_screen_height());

	double current_x = 0;
	double current_y = 0;

	// get cursor position
	glfwGetCursorPos(renderer::get_window(), &current_x, &current_y);

	// calculate delta of cursor positions from last frame
	double delta_x = current_x - xpos1;
	double delta_y = current_y - ypos1;

	// multiply deltas by ratios gets the actual change in orientation on screen space
	delta_x *= ratio_width;
	delta_y *= ratio_height;

	//cout << "\r" << current_x << "   " << current_y << "   " << delta_x << "   " << delta_y << std::flush;
	if ((current_x != xpos1) || (current_y != ypos1))
	{
		phys::getCamera()->rotate((float)delta_x, (float)-delta_y);
		phys::CameraTarget();
	}


	// Ball "movment" controls. Add impulse in the given axis
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_W)) { ballP->AddLinearForce(vec3(0.0f, 0.0f, -20.0f)); }
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_S)) { ballP->AddLinearForce(vec3(0.0f, 0.0f, 20.0f)); }
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_A)) { ballP->AddLinearForce(vec3(-20.0f, 0.0f, 0.0f)); }
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_D)) { ballP->AddLinearForce(vec3(20.0f, 0.0f, 0.0f)); }
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_SPACE)) { ballP->AddLinearForce(vec3(0.0f, 20.0f, 0.0f)); }

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_PAGE_UP))
	{  
		dquat rot(SceneList.at(1)->GetRotation());
		SceneList.at(1)->SetRotation(rot += dquat(vec3(0.0f, 0.0f, 1.0f)));
		cout << "New rotation is " << SceneList.at(1)->GetRotation().x << "   " << SceneList.at(1)->GetRotation().y << "   " << SceneList.at(1)->GetRotation().z << endl;
	}

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_I)) { phys::getCamera()->move(vec3(0.0f, 0.0f, 1.0f)); };
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_J)) { phys::getCamera()->move(vec3(-1.0f, 0.0f, 0.0f)); }
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_K)) { phys::getCamera()->move(vec3(0.0f, 0.0f, -1.0f)); }
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_L)) { phys::getCamera()->move(vec3(1.0f, 0.0f, 0.0f)); }
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_RIGHT_ALT)) { phys::getCamera()->move(vec3(0.0f, -1.0f, 0.0f)); }
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_RIGHT_CONTROL)) { phys::getCamera()->move(vec3(0.0f, 1.0f, 0.0f)); }


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
	phys::getCamera()->update((float)delta_time);
	xpos1 = current_x;
	ypos1 = current_y;

	return true;
}

bool load_content()
{
	phys::Init();
	//createGraphics();
	glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwGetCursorPos(renderer::get_window(), &xpos1, &ypos1);


	// BALL PARTICLE
	SceneList.push_back(move(CreateParticle()));

	// FLOOR PLANE
	SceneList.push_back(CreatePlane(vec3(0.0f, 0.0f, 0.0f)));

	// RAMP PLANES
	SceneList.push_back(CreateRamp(vec3(15.0f, 10.0f, 0.0f), BLUE));
	SceneList.push_back(CreateRamp(vec3(-12.0f, 20.0f, 0.0f), GREEN));
	SceneList.push_back(CreateRamp(vec3(3.0f, 30.0f, 0.0f), YELLOW));


	phys::SetCameraPos(vec3(-10.0f, 10.0f, 40.0f));
	phys::SetCameraTarget(vec3(-2.0f, 10.0f, 2.0f));
	auto aspect = static_cast<float>(renderer::get_screen_width()) / static_cast<float>(renderer::get_screen_height());
	phys::getCamera()->set_projection(quarter_pi<float>(), aspect, 2.414f, 1000.0f);
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
	//phys::DrawScene();
	if (norms == true)
	{
		for (auto &e : SceneList)
		{
			if (e->GetName() == "Plane")
			{
				vec3 normal = e->getCompatibleComponent<cPlaneCollider>()->normal;
				vec3 wp = e->getCompatibleComponent<cPlaneCollider>()->GetParent()->GetPosition();
				vec3 temp_lp = wp + (normal * vec3(10));
				phys::DrawLine(wp, temp_lp, true, BLUE);
				//e->getCompatibleComponent<cPlaneCollider>()->GetParent()->
			}
		}
	}

	// Render visual axis indicator
	renderAxis();
	//renderGraphics();

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

