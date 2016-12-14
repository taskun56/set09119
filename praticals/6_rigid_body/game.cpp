#include "game.h"
#include <algorithm>
#include <glm/gtx/transform.hpp>

using namespace glm;
using namespace std;
Component::Component(const string &token)
{
	token_ = token;
	Ent_ = nullptr;
	active_ = false;
}

Component::~Component() {}

bool Component::IsActive() { return active_; }

void Component::SetActive(bool b) { active_ = b; }

void Component::SetParent(Entity *p) { Ent_ = p; }

Entity *Component::GetParent() const { return Ent_; }

//############## Entity ###################

Entity::Entity()
{
	visible_ = true;
	changed_ = true;
	scale_ = vec3(1.0f, 1.0f, 1.0f);
	position_ = vec3(0.0f, 0.0f, 0.0f);
	rotation_ = quat();
	colour = BLACK;
	width_ = NULL;
	depth_ = NULL;
	normal_ = vec3(0.0f, 1.0f, 0.0f);
	tex_ = graphics_framework::texture("resources\\tile.png");
}

Entity::~Entity() {}

const dvec3 Entity::GetScale() const { return scale_; }

const dvec3 Entity::GetPosition() const { return position_; }

const dquat Entity::GetRotation() const { return rotation_; }

const dvec3 Entity::GetRotationV3() const { return glm::eulerAngles(GetRotation()); }

void Entity::setTexture(const string &filename)
{
	tex_ = graphics_framework::texture(filename);
}

graphics_framework::texture Entity::getTexture()
{
	return tex_;
}

void Entity::setSize(unsigned width, unsigned depth)
{
	width_ = width;
	depth_ = depth;
}

void Entity::setColor(phys::RGBAInt32 col)
{
	colour = col;
}

int Entity::getDepth()
{
	return depth_;
}

int Entity::getWidth()
{
	return width_;
}

const dmat4 Entity::GetTransform()
{
	if (changed_) {
		mat4 scl = scale(scale_);
		mat4 rot = mat4_cast(rotation_);
		mat4 trn = translate(position_);
		transform_ = (trn * rot * scl);
		changed_ = false;
	}
	return transform_;
}

const bool Entity::IsVisible() const { return false; }

const string Entity::GetName() const { return name_; }

const glm::vec3 Entity::getNormal() const
{
	return normal_;
}

void Entity::SetTransform(const dmat4 m4) { assert(false); }

void Entity::SetScale(const dvec3 &v3) {
	scale_ = v3;
	changed_ = true;
}

void Entity::SetPosition(const dvec3 &v3) {
	position_ = v3;
	changed_ = true;
}

void Entity::SetRotation(const dvec3 &v3) {
	rotation_ = glm::quat(v3);
	// need to rotate the normal as well
	changed_ = true;
}

void Entity::SetRotation(const dquat &q) {
	rotation_ = q;
	changed_ = true;
}

void Entity::SetVisibility(const bool b) {}

void Entity::SetName(string const &name) { name_ = name; }

void Entity::Update(const double delta) {
	for (auto &c : components_) {
		c->Update(delta);
	}
}

void Entity::Render() {
	for (auto &c : components_) {
		c->Render();
	}
}

void Entity::AddComponent(unique_ptr<Component> &c) {
	c->SetParent(this);
	components_.push_back(move(c));
}

void Entity::RemoveComponent(Component &c) {
	// Todo: Test This
	auto position =
		find_if(components_.begin(), components_.end(), [c](unique_ptr<Component> &p) { return p.get() == &c; });
	if (position != components_.end()) {
		components_.erase(position);
	}
}

// be careful what you do with this function...
vector<Component *> Entity::GetComponents(string const &name) const {
	vector<Component *> list;
	if (components_.size() < 1) {
		return list;
	}
	for (auto &c : components_) {
		if (c->token_ == name) {
			list.push_back(c.get()); // It's not like we want to make safe programs anyway...
		}
	}
	return list;
}

const vector<unique_ptr<Component>> *Entity::GetComponents() const { return &components_; }

//############## Shape Renderer ###################

void cShapeRenderer::SetColour(const phys::RGBAInt32 c) { col_ = c; }

cShapeRenderer::cShapeRenderer(SHAPES s, phys::RGBAInt32 color) : shape(s), col_(color), Component("ShapeRenderer") {}

cShapeRenderer::~cShapeRenderer() {}

void cShapeRenderer::Update(double delta) {}

void cShapeRenderer::Render() {
	const mat4 m = glm::translate(Ent_->GetPosition()) * mat4_cast(Ent_->GetRotation());
	vec3 pos = Ent_->GetPosition();
	vec3 norm = Ent_->getNormal();
	switch (shape) {
	case SPHERE:
		phys::DrawSphere(m, col_);
		break;
	case BOX:
		phys::DrawCube(m, col_);
		break;
	case PLANE:
		phys::DrawPlane(pos, norm, vec3(1.0f), Ent_->colour);
		break;
	case RAMP:
		//phys::DrawPlane(pos, norm, vec3(1.0f), Ent_->getWidth(), Ent_->getDepth(), Ent_->colour);
		phys::DrawPlane(pos, norm, vec3(1.0f), Ent_->getWidth(), Ent_->getDepth(), Ent_->getTexture(), Ent_->colour);
		break;
	default:
		cout << "No renderable shape for object type 'shape': " << shape << endl;
		break;
	}
}


// TODO - floor does not move by gravity, good
// need to rotate the plane and test the angle of reflection when the ball hits.
// then i need to make the ball "slide" down a plane it is already in contact with



// TODO first check if on plane
// next cvheck distance from right and forward vectors to see if still on plane