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
	unique_ptr<Component> physComponent(new cRigidPlane());
	unique_ptr<cShapeRenderer> renderComponent(new cShapeRenderer(cShapeRenderer::PLANE, phys::RandomColour()));
	renderComponent->SetColour(phys::RandomColour());
	ent->AddComponent(physComponent);
	ent->AddComponent(unique_ptr<Component>(new cPlaneCollider()));
	ent->AddComponent(unique_ptr<Component>(move(renderComponent)));
	ent->SetName("Plane");
	return ent;
}
unique_ptr<Entity> CreateRamp(vec3 pos)
{
	unique_ptr<Entity> ent(new Entity());
	ent->SetPosition(pos);
	unique_ptr<Component> physComponent(new cRigidPlane());
	unique_ptr<cShapeRenderer> renderComponent(new cShapeRenderer(cShapeRenderer::RAMP, phys::RandomColour()));
	renderComponent->SetColour(phys::RandomColour());
	ent->AddComponent(physComponent);
	ent->AddComponent(unique_ptr<Component>(new cPlaneCollider()));
	ent->AddComponent(unique_ptr<Component>(move(renderComponent)));
	ent->SetName("Plane");
	return ent;
}

void addNormals(geometry geom, vec3 normal, int vertices)
{
	// need to create a looping method that adds the passed through
	// normal to the buffer for every vertex on the given quad
	vector<vec3> normals;

	// for every vertex on the given geom
	for (int i = 0; i < vertices; i++)
	{
		// add the given normal to a vector of normals
		normals.push_back(normal);
	}

	// then add those normals to the given geometry's normal buffer
	geom.add_buffer(normals, BUFFER_INDEXES::NORMAL_BUFFER);
}

void createGraphics()
{
	ramp_b_base.set_type(GL_QUADS);

	// Create quad data
	// Positions
	vector<vec3> positions
	{
		vec3(-1.0f, 1.0f, 0.0f),
		vec3(-1.0f, -1.0f, 0.0f),
		vec3(1.0f, -1.0f, 0.0f),
		vec3(1.0f, 1.0f, 0.0f)
	};
	// Colours
	vector<vec4> colours
	{
		vec4(1.0f, 0.0f, 0.0f, 1.0f),
		vec4(0.0f, 1.0f, 0.0f, 1.0f),
		vec4(0.0f, 0.0f, 1.0f, 1.0f),
		vec4(1.0f, 1.0f, 0.0f, 1.0f)
	};
	// Add to the geometry
	ramp_b_base.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
	ramp_b_base.add_buffer(colours, BUFFER_INDEXES::COLOUR_BUFFER);

	// Load in shaders
	simple_eff.add_shader(
		"shaders/basic.vert", // filename
		GL_VERTEX_SHADER); // type
	simple_eff.add_shader(
		"shaders/basic.frag", // filename
		GL_FRAGMENT_SHADER); // type
							 // Build effect

	simple_eff.build();

	//vector<vec3> tex_coords
	//{
	//	vec3(-1.0f, 1.0f, 0.0f),
	//	vec3(-1.0f, -1.0f, 0.0f),
	//	vec3(1.0f, -1.0f, 0.0f),
	//	vec3(1.0f, 1.0f, 0.0f)
	//};

	//// TOP RAIL
	//geometry ramp_t_base;				// base of top ramp
	//ramp_t_base.set_type(GL_QUADS);
	//
	//vector<vec3> rtb_positions
	//{
	//	// vec3 world space positions for the top ramp base quad
	//};

	//geometry ramp_t_rail_l;				// left rail of top ramp
	//vector<vec3> rtl_positions
	//{
	//	// vec3 world space positions for the top ramp base quad
	//};

	//geometry ramp_t_rail_r;				// right rail of top ramp
	//vector<vec3> rtr_positions
	//{
	//	// vec3 world space positions for the top ramp base quad
	//};





	//// MIDDLE RAIL
	//geometry ramp_m_base;				// base of top ramp
	//ramp_m_base.set_type(GL_QUADS);

	//vector<vec3> rmb_positions
	//{
	//	// vec3 world space positions for the top ramp base quad
	//};

	//geometry ramp_m_rail_l;				// left rail of top ramp
	//vector<vec3> rml_positions
	//{
	//	// vec3 world space positions for the top ramp base quad
	//};

	//geometry ramp_m_rail_r;				// right rail of top ramp
	//vector<vec3> rmr_positions
	//{
	//	// vec3 world space positions for the top ramp base quad
	//};





	// BOTTOM RAIL
	
	//ramp_b_base.set_type(GL_QUADS);


	//vector<vec3> positions
	//{
	//	vec3(1.0f, 1.0f, 0.0f),
	//	vec3(-1.0f, 1.0f, 0.0f),
	//	vec3(-1.0f, -1.0f, 0.0f),
	//	vec3(1.0f, -1.0f, 0.0f)
	//};
	//vector<vec3> rbb_positions
	//{
	//	// vec3 world space positions for the top ramp base quad
	//	vec3(-3.0f, 1.0f, 0.0f),	// TOP LEFT
	//	vec3(-3.0f, 0.0f, 0.0f),	// BOTTOM LEFT
	//	vec3(3.0f, -1.0f, 0.0f),	// BOTTOM RIGHT	
	//	vec3(3.0f, 0.0f, 0.0f)		// TOP RIGHT
	//};

	//geometry ramp_b_rail_l;				// left rail of top ramp
	//vector<vec3> rbl_positions
	//{
	//	// vec3 world space positions for the top ramp base quad
	//	vec3(-3.0f, 0.0f, 0.0f),	// TOP LEFT
	//	vec3(-3.0f, -1.0f, 0.0f),	// BOTTOM LEFT
	//	vec3(3.0f, 0.0f, 0.0f),		// BOTTOM RIGHT	
	//	vec3(3.0f, 1.0f, 0.0f)		// TOP RIGHT
	//};

	//geometry ramp_b_rail_r;				// right rail of top ramp
	//vector<vec3> rbr_positions
	//{
	//	// vec3 world space positions for the top ramp base quad
	//	// base quad is viewed from the top and its depth is measured on the Z axis instead of leftright/updown of the railings
	//	vec3(-3.0f, 0.0f, 1.0f),	// TOP LEFT
	//	vec3(-3.0f, 0.0f, -1.0f),	// BOTTOM LEFT
	//	vec3(3.0f, 0.0f, -1.0f),	// BOTTOM RIGHT	
	//	vec3(3.0f, 0.0f, 1.0f)		// TOP RIGHT
	//};





	//// UNIVERSAL COLOR VECTOR FOR ALL RAMP QUADS
	//vector<vec4> colors
	//{
	//	vec4(1.0f, 0.0F, 0.0f, 1.0f),	// RED CORNER
	//	vec4(0.0f, 1.0F, 0.0f, 1.0f),	// GREEN CORNER
	//	vec4(0.0f, 0.0F, 1.0f, 1.0f),	// BLUE CORNER
	//	vec4(1.0f, 1.0F, 0.0f, 1.0f)	// YELLOW CORNER
	//};

	//// Ramp bottom base buffer indexes for vector and color
	//ramp_b_base.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
	//ramp_b_base.add_buffer(colors, BUFFER_INDEXES::COLOUR_BUFFER);
	////addNormals(ramp_b_base, vec3(0.0f, 1.0f, 0.0f), 4);
	////ramp_b_base.add_buffer(tex_coords, BUFFER_INDEXES::TEXTURE_COORDS_0);


	//// shader file path -> ..\\..\\..\\new\\res\\shaders
	////ramp_b_base = geometry_builder::create_plane(6, 2);

	//simple_eff.add_shader("shaders/phys_basic.vert", GL_VERTEX_SHADER);
	//simple_eff.add_shader("shaders/phys_basic.frag", GL_FRAGMENT_SHADER);
	//simple_eff.build();
	//renderer::bind(simple_eff);
	//auto M = glm::scale(mat4(1.0f), vec3(10.0f, 5.0, 10.0f));
	//mat3 N(1.0f);
	//phys::RGBAInt32 col = GREEN;
	//mat.set_diffuse(col.tovec4());
	//renderer::bind(mat, "mat");
	//renderer::bind(phys::getLight(), "light");
	//glUniformMatrix4fv(simple_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(phys::getPV() * M));
	//glUniformMatrix4fv(simple_eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
	//glUniformMatrix3fv(simple_eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));
	//glDisable(GL_CULL_FACE);
	//renderer::render(ramp_b_base);
	//glEnable(GL_CULL_FACE);
}

void renderGraphics()
{
	// Bind effect
	renderer::bind(simple_eff);
	// Create MVP matrix
	auto M = glm::translate(mat4(1.0f), vec3(10.0f, 15.0f, -20.0f)) * glm::scale(mat4(1.0f), vec3(1.0f)) * mat4_cast(glm::rotation(vec3(0, 1.0, 0), vec3(0.0f, 1.0f, 0.0f)));
	auto V = phys::getCamera()->get_view();
	auto P = phys::getCamera()->get_projection();
	auto MVP = P * V * M;
	mat3 N(1.0f);
	mat.set_diffuse(vec4(0.0f, 1.0f, 0.0f, 1.0f));
	renderer::bind(phys::getMaterial(), "mat");
	renderer::bind(phys::getLight(), "light");
	glUniformMatrix4fv(simple_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(P* V * M));
	glUniformMatrix4fv(simple_eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
	glUniformMatrix3fv(simple_eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));
	glDisable(GL_CULL_FACE);
	renderer::render(ramp_b_base);
	//glEnable(GL_CULL_FACE);
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
	SceneList.push_back(CreateRamp(vec3(5.0f, 10.0f, 0.0f)));
	//SceneList.push_back(CreatePlane(vec3(2.0f, 20.0f, 0.0f)));
	//SceneList.push_back(CreatePlane(vec3(3.0f, 30.0f, 0.0f)));


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

