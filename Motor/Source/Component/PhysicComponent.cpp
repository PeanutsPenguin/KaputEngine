#include "Component/PhysicComponent.h"

#include "Component/Component.hpp"
#include "GameObject/Camera.h"
#include "Physics/PrivateBulletWrapper.h"
#include "Rendering/Primitive.h"
#include "Resource/Manager.hpp"
#include "Resource/ShaderProgram.h"
#include "Text/Xml/Context.hpp"
#include "Utils/RemoveVector.hpp"

#include <LibMath/Angle/Degree.h>
#include <btBulletDynamicsCommon.h>
#include <glad/glad.h>

using namespace LibMath;
using namespace KaputEngine;
using namespace KaputEngine::Rendering;
using namespace KaputEngine::Resource;
using namespace KaputEngine::Text::Xml;

using KaputEngine::Inspector::Property;

using std::cerr;
using std::string;

COMPONENT_IMPL(PhysicComponent)

PhysicComponent::PhysicComponent(GameObject& parent)
	: PhysicComponent(parent, s_nextId++) { }

PhysicComponent::PhysicComponent(GameObject& parent, const Id& id)
	: PhysicComponent(parent, id, E_NO_SHAPE, 0.f) { }

PhysicComponent::PhysicComponent(GameObject& parent, const ePhysicShape shape, const float mass)
	: PhysicComponent(parent, s_nextId++, shape, mass) { }

PhysicComponent::PhysicComponent(GameObject& parent, const Id& id, const ePhysicShape shape, const float mass)
	: Component(parent, id), m_shape(shape)
{
	this->m_program = ResourceManager::get<ShaderProgramResource>("Kaput/Shader/Picking/PickingProgram.kasset")->dataPtr();

	this->m_body = std::make_unique<RigidBody>();
	this->m_collisionShape = std::make_unique<CollisionShape>();

	/*RIGIBODY CREATION*/
	const Transform& trans = this->parentObject()->getWorldTransform();
	this->getRightShape(shape, trans.scale);

	btTransform transConverted;
	transConverted.setIdentity();
	transConverted.setOrigin({trans.position.x(),trans.position.y(),trans.position.z()});
	transConverted.setRotation({trans.rotation.x(),trans.rotation.y(),trans.rotation.z(),trans.rotation.w() == 0 ? 1 : trans.rotation.w()});

	const bool isDynamic = mass != 0.f;

	btVector3 localInertia(0,0,0);

	if (this->m_shape != E_NO_SHAPE && isDynamic)
		this->m_collisionShape->m_btShape->calculateLocalInertia(mass, localInertia);

	///Provides a common implementation to synchronize world transforms with offsets.
	btDefaultMotionState* myMotionState = new btDefaultMotionState(transConverted);

	//Need a mass, a motion State, a shape and an inertia to create a RigidBody
	const btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState, m_collisionShape->m_btShape, localInertia);
	this->m_body->m_btBody = new btRigidBody(rbInfo);
	/*RIGIBODY CREATION*/

	this->setCanPhysicUpdate(true);

	if (shape == E_NO_SHAPE)
		this->setCanPhysicUpdate(false);

	if (this->m_collisionShape->m_btShape)
		this->m_collisionShape->m_btShape->setUserPointer(this);

	this->m_body->m_btBody->setUserPointer(this);

	for (int i = 0; i < this->m_fixTranslation.size(); ++i)
	{
		this->m_fixTranslation[i] = false;
		this->m_fixRotation[i] = false;
	}

	this->setTag("None");
}

PhysicComponent::~PhysicComponent()
{
	this->m_parentObject.setPhysicComponentPtr(nullptr);
}

void PhysicComponent::destroy()
{
	this->m_parentObject.setPhysicComponentPtr(nullptr);
	this->setCanUpdate(false);
	this->setCanPhysicUpdate(false);
	Component::destroy();
}

void PhysicComponent::setTransform(const Transform& trans)
{
	btTransform transConverted;
	transConverted.setOrigin({ trans.position.x(), trans.position.y(), trans.position.z() });
	transConverted.setRotation({ trans.rotation.x(), trans.rotation.y(), trans.rotation.z(), trans.rotation.w() });

	this->m_body->m_btBody->setWorldTransform(transConverted);
	this->m_body->m_btBody->getMotionState()->setWorldTransform(transConverted);
}

void PhysicComponent::setMass(const float mass)
{
	btVector3 localInertia(0, 0, 0);

	if (m_collisionShape->m_btShape && mass > 0)
		this->m_collisionShape->m_btShape->calculateLocalInertia(mass, localInertia);

	this->m_body->m_btBody->setMassProps(mass, localInertia);
}

void PhysicComponent::setScale(const Vector3f& size)
{
	if (this->m_shape == E_NO_SHAPE)
		return;

	this->m_collisionShape->m_btShape->setLocalScaling({ size.x(), size.y(), size.z() });
}

float PhysicComponent::getFriction() const noexcept
{
	return this->m_body->m_btBody->getFriction();
}

std::bitset<3>& PhysicComponent::getFreezeTranslation() noexcept
{
	return this->m_fixTranslation;
}
const std::bitset<3>& PhysicComponent::getFreezeTranslation() const noexcept
{
	return this->m_fixTranslation;
}

std::bitset<3>& PhysicComponent::getFreezeRotation() noexcept
{
	return this->m_fixRotation;
}
const std::bitset<3>& PhysicComponent::getFreezeRotation() const noexcept
{
	return this->m_fixRotation;
}

void PhysicComponent::setFreezeTranslation(int axe, bool value)
{
	this->m_fixTranslation[axe] = value;
}

void PhysicComponent::setFreezeRotation(int axe, bool value)
{
	this->m_fixRotation[axe] = value;

	if (!value)
	{
		btVector3 freeze = this->m_body->m_btBody->getAngularFactor();
		freeze[axe] = 1.f;
		this->m_body->m_btBody->setAngularFactor(freeze);
	}
	else
	{
		btVector3 freeze = this->m_body->m_btBody->getAngularFactor();
		freeze[axe] = 0.f;
		this->m_body->m_btBody->setAngularFactor(freeze);
	}
}

void PhysicComponent::setBounciness(float value)
{
	this->m_body->m_btBody->setRestitution(value);
}

float PhysicComponent::getBounciness()
{
	return this->m_body->m_btBody->getRestitution();
}

void PhysicComponent::setRigidBodyPosition(const Vector3f& vec)
{
	btTransform transConverted;
	this->m_body->m_btBody->getMotionState()->getWorldTransform(transConverted);
	transConverted.setOrigin({ vec.x(), vec.y(), vec.z() });

	this->m_body->m_btBody->setWorldTransform(transConverted);
	this->m_body->m_btBody->getMotionState()->setWorldTransform(transConverted);
}

void PhysicComponent::resetVelocity()
{
	this->setLinearVelocity({ 0 });
	this->setPushVelocity({ 0 });
}

void PhysicComponent::setFriction(float fric)
{
	this->m_body->m_btBody->setFriction(fric);
}

ePhysicShape PhysicComponent::getShape()
{
	return this->m_shape;
}

void PhysicComponent::setShape(ePhysicShape shape)
{
	Vector3f scale = this->getScale();

	if (scale == Vector3f::Dir::zero())
		scale = Vector3f::Dir::one();

	if (this->m_collisionShape->m_btShape != nullptr)
		delete this->m_collisionShape->m_btShape;

	this->getRightShape(shape, scale);
	this->m_shape = shape;
	this->setScale(scale);

	if (this->m_body->m_btBody)
		this->m_body->m_btBody->setCollisionShape(this->m_collisionShape->m_btShape);

	if (this->m_collisionShape->m_btShape)
		this->m_collisionShape->m_btShape->setUserPointer(this);
}

void PhysicComponent::updatePhysics(const double deltaTime)
{
	btTransform physicTrans;
	this->m_body->m_btBody->getMotionState()->getWorldTransform(physicTrans);
	Transform newTrans = this->m_parentObject.getWorldTransform();

	for (int i = 0; i < this->m_fixTranslation.size() ; ++i)
		if (!this->m_fixTranslation[i])
			newTrans.position[i] = physicTrans.getOrigin()[i];

	physicTrans.setOrigin({ newTrans.position .x(), newTrans.position.y(), newTrans.position.z() });

	newTrans.rotation = { physicTrans.getRotation().x(), physicTrans.getRotation().y(), physicTrans.getRotation().z(), physicTrans.getRotation().w() };

	this->m_parentObject.setWorldTransformWithoutPhysic(newTrans);
}

void PhysicComponent::render(const Camera& camera)
{
	if (!this->m_program->use() || m_shape == E_NO_SHAPE || !this->m_physicUpdate)
		return;

	Transform newTrans;
	btTransform btTrans;
	this->m_body->m_btBody->getMotionState()->getWorldTransform(btTrans);
	newTrans.position = { btTrans.getOrigin().x(), btTrans.getOrigin().y(), btTrans.getOrigin().z() };
	newTrans.rotation = { btTrans.getRotation().x(), btTrans.getRotation().y(), btTrans.getRotation().z(), btTrans.getRotation().w() };

	if (this->m_shape == E_CAPSULE_SHAPE)
		newTrans.rotation *= Rotor({ 1 }, Degree(90));

	const Matrix4f
		& projView = camera.getViewProjectionMatrix(),
		& model = newTrans.toMatrix();

	Matrix4f MVP = projView * model;

	this->m_program->setUniform("MVP", MVP);
	this->m_program->setUniform("PickingColor", Color::Red);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	this->renderRightForm();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void PhysicComponent::renderRightForm()
{
	switch (this->m_shape)
	{
	case E_BOX_SHAPE:
	this->drawBoxShapeMesh();
		break;
	case E_SPHERE_SHAPE:
		this->drawSphereShapeMesh();
		break;
	case E_CAPSULE_SHAPE:
		this->drawCapsuleShapeMesh();
		break;
	}
}

float PhysicComponent::getMass() const noexcept
{
	return this->m_body->m_btBody->getMass();
}

Vector3f PhysicComponent::getScale() const noexcept
{
	if (m_shape == E_NO_SHAPE)
		return { };

	btVector3 scale = this->m_collisionShape->m_btShape->getLocalScaling();
	return { scale.x(), scale.y(), scale.z() };
}

void PhysicComponent::setCanPhysicUpdate(const bool update)
{
	if (this->m_physicUpdate == update)
		return;

	this->m_physicUpdate = update;

	if (Scene* scene = parentScene(); scene)
	{
		if (this->m_physicUpdate)
			registerPhysics(*scene);
		else
			unregisterPhysics(*scene);
	}
}

void PhysicComponent::start()
{
	ObjectBase::start();

	this->setTag(this->m_collisionTag);
}

bool PhysicComponent::getCanPhysicUpdate()
{
	return this->m_physicUpdate;
}

void PhysicComponent::addImpulse(const Vector3f force)
{
	if (!this->m_body->m_btBody->isActive())
		this->m_body->m_btBody->setActivationState(ACTIVE_TAG);

 	this->m_body->m_btBody->applyCentralImpulse({ force.x(), force.y(), force.z() });
}

void PhysicComponent::addForce(const Vector3f force)
{
	if (!this->m_body->m_btBody->isActive())
		this->m_body->m_btBody->setActivationState(ACTIVE_TAG);

	this->m_body->m_btBody->applyCentralForce({ force.x(), force.y(), force.z() });
}

void PhysicComponent::setPushVelocity(const Vector3f force)
{
	if (!this->m_body->m_btBody->isActive())
		this->m_body->m_btBody->setActivationState(ACTIVE_TAG);

	this->m_body->m_btBody->setPushVelocity({ force.x(), force.y(), force.z() });
}

void PhysicComponent::setLinearVelocity(const Vector3f force)
{
	if (!this->m_body->m_btBody->isActive())
		this->m_body->m_btBody->setActivationState(ACTIVE_TAG);

	this->m_body->m_btBody->setLinearVelocity({ force.x(), force.y(), force.z() });
}

Vector3f PhysicComponent::getLinearVelocity() const
{
	btVector3 vel = this->m_body->m_btBody->getLinearVelocity();
	return { vel.x(), vel.y(), vel.z() };
}

bool PhysicComponent::compareTag(const string& tag)
{
	unsigned int flag = this->m_parentObject.parentScene()->getPhysicHandler().getTags()[tag];
	
	if (flag != 0)
		return this->m_body->m_btBody->getFlags() == flag;

	return false;
}

string PhysicComponent::getTag()
{
	std::unordered_map<string, unsigned int>& um = this->m_parentObject.parentScene()->getPhysicHandler().getTags();
	std::unordered_map<string, unsigned int>::const_iterator isExisting = um.find(this->m_collisionTag);

	if(isExisting == um.end())
	{
		this->m_parentObject.parentScene()->getPhysicHandler().addCollisionsTag(this->m_collisionTag);
		this->setTag(this->m_collisionTag);
	}

	return this->m_collisionTag;
}

void PhysicComponent::setTag(const std::string& tag)
{
	Scene* scene = this->m_parentObject.parentScene();
	this->m_collisionTag = tag;

	if (scene)
	{
		std::unordered_map<string, unsigned int>& um = scene->getPhysicHandler().getTags();
		std::unordered_map<string, unsigned int>::const_iterator isExisting = um.find(tag);

		if (isExisting == um.end())
			scene->getPhysicHandler().addCollisionsTag(tag);

		isExisting = um.find(tag);
		this->m_body->m_btBody->setFlags(isExisting->second);
	}
}

void PhysicComponent::serializeValues(XmlSerializeContext& context) const
{
	Component::serializeValues(context);

	context.value("Mass", getMass());
	context.value("Shape", static_cast<uint8_t>(m_shape));
	context.value("PhysicUpdate", m_physicUpdate);
	context.value("Scale", this->getScale());
	context.value("Tag", this->m_collisionTag);
	context.value("Friction", this->getFriction());

	for (int i = 0; i < 3; ++i)
	{
		std::string name = "Translation";
		std::string nameBis = "Rotation";

		switch (i)
		{
		case 0:
			name += "X";
			nameBis += "X";
			break;
		case 1:
			name += "Y";
			nameBis += "Y";
			break;
		case 2:
			name += "Z";
			nameBis += "Z";
			break;
		default:
			break;
		}

		context.value(name, this->m_fixTranslation[i]);
		context.value(nameBis, this->m_fixRotation[i]);
	}
}

void PhysicComponent::registerPhysics(Scene& scene)
{
	scene.getPhysicHandler().addToWorld(this->m_body.get());
	scene.getPhysicHandler().m_physicsQueue.push_back(*this);
}

void PhysicComponent::unregisterPhysics(Scene& scene)
{
	scene.getPhysicHandler().removeFromWorld(this->m_body.get());

	if (RemoveVector<IPhysicsUpdatable>* ptr = IPhysicsUpdatable::m_status.castVector<IPhysicsUpdatable>(); ptr)
		ptr->erase(*this);
}

void PhysicComponent::registerQueues(Scene& scene)
{
	Component::registerQueues(scene);

	if (this->m_physicUpdate)
		registerPhysics(scene);
}

void PhysicComponent::unregisterQueues()
{
	Component::unregisterQueues();
	unregisterPhysics(*this->m_parentObject.parentScene());
}

void PhysicComponent::drawSphereShapeMesh()
{
	const btSphereShape* sphere = static_cast<const btSphereShape*>(this->m_collisionShape->m_btShape);
	const float radius = sphere->getRadius();

	Sphere sphereDraw(radius,SECTORS,STACKS);
	sphereDraw.draw();
}

void PhysicComponent::drawCapsuleShapeMesh()
{
	const btCapsuleShape* capsule = static_cast<const btCapsuleShape*>(this->m_collisionShape->m_btShape);
	const float offset = capsule->getHalfHeight();
	const float radius = capsule->getRadius();

	Capsule cap(radius,offset,SECTORS,STACKS);
	cap.draw();
}

void PhysicComponent::drawBoxShapeMesh()
{
	const btBoxShape* box = static_cast<const btBoxShape*>(this->m_collisionShape->m_btShape);
	const Vector3f size = { box->getHalfExtentsWithMargin().x(), box->getHalfExtentsWithMargin().y(), box->getHalfExtentsWithMargin().z()};

	SecondCube cube(size);
	cube.draw();
}

void PhysicComponent::getRightShape(ePhysicShape shape, const Vector3f& size)
{
	switch (shape)
	{
		case E_BOX_SHAPE:
			this->m_collisionShape->m_btShape = new btBoxShape(btVector3(size.x()/2,size.y()/2,size.z()/2));
			break;
		case E_SPHERE_SHAPE:
			this->m_collisionShape->m_btShape = new btSphereShape(size.x());
			break;
		case E_CAPSULE_SHAPE:
			this->m_collisionShape->m_btShape = new btCapsuleShape(size.x(),size.y());
			break;
		default:
			break;
	}
}

void PhysicComponent::onCollisionEnter(_In_ IPhysicsUpdatable* collider)
{
	PhysicComponent* comp = static_cast<PhysicComponent*>(collider);
	this->parentObject()->onCollisionEnter(comp);
}

void PhysicComponent::onCollision(_In_ IPhysicsUpdatable* collider)
{
	PhysicComponent* comp = static_cast<PhysicComponent*>(collider);
	comp->parentObject()->onCollision(this);
}

void PhysicComponent::onCollisionExit()
{
	this->parentObject()->onCollisionExit();
}

_Success_(return) std::optional<ePhysicShape> PhysicComponent::parseShape(const XmlNode::Map& map)
{
	const auto shapeIt = map.find("Shape");

	if (shapeIt == map.end())
	{
		cerr << __FUNCTION__": Required property Shape missing.\n";
		return std::nullopt;
	}

	const std::optional<uint8_t> shapeOp = shapeIt->second->parse<uint8_t>();

	if (!shapeOp)
	{
		cerr << __FUNCTION__": Failed to deserialize Shape.\n";
		return std::nullopt;
	}

	if (*shapeOp > 3)
	{
		cerr << __FUNCTION__": Invalid shape index (" << *shapeOp << ").\n";
		return std::nullopt;
	}

	return static_cast<ePhysicShape>(*shapeOp);
}

_Success_(return) std::optional<string> PhysicComponent::parseTag(const XmlNode::Map& map)
{
	const auto tagIt = map.find("Tag");

	if (tagIt == map.end())
	{
		cerr << __FUNCTION__": Required property Tag missing.\n";
		return "None";
	}

	const std::optional<std::string> tagOp = tagIt->second->parse<std::string>();

	if (!tagOp)
	{
		cerr << __FUNCTION__": Failed to deserialize Tag.\n";
		return "None";
	}

	return *tagOp;
}

_Success_(return) std::optional<float> PhysicComponent::parseMass(const XmlNode::Map& map)
{
	const auto massIt = map.find("Mass");

	if (massIt == map.end())
	{
		cerr << __FUNCTION__": Required property Mass missing. Mass is set to 0.\n";
		return 0.f;
	}

	const std::optional<float> massOp = massIt->second->parse<float>();

	if (!massOp)
	{
		cerr << __FUNCTION__": Failed to deserialize Mass. Mass is set to 0\n";
		return 0.f;
	}

	return *massOp;
}

_Success_(return) std::optional<float> PhysicComponent::parseFriction(const Text::Xml::XmlNode::Map& map)
{
	const auto fricIt = map.find("Friction");

	if (fricIt == map.end())
	{
		cerr << __FUNCTION__": Required property Friction missing. Friction is set to 1.\n";
		return 1.f;
	}

	const std::optional<float> fricOp = fricIt->second->parse<float>();

	if (!fricOp)
	{
		cerr << __FUNCTION__": Failed to deserialize Friction. Friction is set to 1.\n";
		return 1.f;
	}

	return *fricOp;
}

_Success_(return) std::optional<bool> PhysicComponent::parsePhysicUpdate(const XmlNode::Map& map)
{
	const auto physicIt = map.find("PhysicUpdate");

	if (physicIt == map.end())
	{
		cerr << __FUNCTION__": Required property canPhysicUpdate missing. canPhysicUpdate is set to false.\n";
		return false;
	}

	const std::optional<bool> physicOp = physicIt->second->parse<bool>();

	if (!physicOp)
	{
		cerr << __FUNCTION__": Failed to deserialize canPhysicUpdate. canPhysicUpdate is set to false.\n";
		return false;
	}

	return *physicOp;
}

_Success_(return) std::optional<bool> PhysicComponent::parseFreezeTranslationX(const XmlNode::Map& map)
{
	const auto translationIt = map.find("TranslationX");

	if (translationIt == map.end())
	{
		cerr << __FUNCTION__": Required property TranslationX missing. TranslationX is set to false.\n";
		return false;
	}

	const std::optional<bool> translationOp = translationIt->second->parse<bool>();

	if (!translationOp)
	{
		cerr << __FUNCTION__": Failed to deserialize TranslationX. TranslationX is set to false.\n";
		return false;
	}

	return *translationOp;
}

_Success_(return) std::optional<bool> PhysicComponent::parseFreezeTranslationY(const XmlNode::Map& map)
{
	const auto translationIt = map.find("TranslationY");

	if (translationIt == map.end())
	{
		cerr << __FUNCTION__": Required property TranslationY missing. TranslationY is set to false.\n";
		return false;
	}

	const std::optional<bool> translationOp = translationIt->second->parse<bool>();

	if (!translationOp)
	{
		cerr << __FUNCTION__": Failed to deserialize TranslationY. TranslationY is set to false.\n";
		return false;
	}

	return *translationOp;
}

_Success_(return) std::optional<bool> PhysicComponent::parseFreezeTranslationZ(const XmlNode::Map& map)
{
	const auto translationIt = map.find("TranslationZ");

	if (translationIt == map.end())
	{
		cerr << __FUNCTION__": Required property TranslationZ missing. TranslationZ is set to false.\n";
		return false;
	}

	const std::optional<bool> translationOp = translationIt->second->parse<bool>();

	if (!translationOp)
	{
		cerr << __FUNCTION__": Failed to deserialize TranslationZ. TranslationZ is set to false.\n";
		return false;
	}

	return *translationOp;
}

_Success_(return) std::optional<bool> PhysicComponent::parseFreezeRotationX(const Text::Xml::XmlNode::Map& map)
{
	const auto rotationIt = map.find("RotationX");

	if (rotationIt == map.end())
	{
		cerr << __FUNCTION__": Required property RotationX missing. RotationX is set to false.\n";
		return false;
	}

	const std::optional<bool> rotationOp = rotationIt->second->parse<bool>();

	if (!rotationOp)
	{
		cerr << __FUNCTION__": Failed to deserialize RotationX. RotationX is set to false.\n";
		return false;
	}

	return *rotationOp;
}

_Success_(return) std::optional<bool> PhysicComponent::parseFreezeRotationY(const Text::Xml::XmlNode::Map& map)
{
	const auto rotationIt = map.find("RotationY");

	if (rotationIt == map.end())
	{
		cerr << __FUNCTION__": Required property RotationY missing. RotationY is set to false.\n";
		return false;
	}

	const std::optional<bool> rotationOp = rotationIt->second->parse<bool>();

	if (!rotationOp)
	{
		cerr << __FUNCTION__": Failed to deserialize RotationY. RotationY is set to false.\n";
		return false;
	}

	return *rotationOp;
}

_Success_(return) std::optional<bool> PhysicComponent::parseFreezeRotationZ(const Text::Xml::XmlNode::Map& map)
{
	const auto rotationIt = map.find("RotationZ");

	if (rotationIt == map.end())
	{
		cerr << __FUNCTION__": Required property RotationZ missing. RotationZ is set to false.\n";
		return false;
	}

	const std::optional<bool> rotationOp = rotationIt->second->parse<bool>();

	if (!rotationOp)
	{
		cerr << __FUNCTION__": Failed to deserialize RotationZ. RotationZ is set to false.\n";
		return false;
	}

	return *rotationOp;
}

_Success_(return) std::optional<Vector3f> PhysicComponent::parseScale(const XmlNode::Map& map)
{
	const auto scaleIt = map.find("Scale");

	if (scaleIt == map.end())
	{
		cerr << __FUNCTION__": Required property Scale missing. Scale is set to 1.\n";
		return Vector3f{ 1.f, 1.f, 1.f };
	}

	const std::optional<Vector3f> scaleOp = scaleIt->second->parse<Vector3f>();

	if (!scaleOp)
	{
		cerr << __FUNCTION__": Failed to deserialize scale. Scale is set to 1.\n";
		return  Vector3f { 1.f, 1.f, 1.f };
	}

	return *scaleOp;
}

_Success_(return) bool PhysicComponent::deserializeMap(_In_ const XmlNode::Map& map)
{
	if (!Component::deserializeMap(map))
		return false;

	if (std::optional<ePhysicShape> shapeOp = parseShape(map); shapeOp)
		this->setShape(*shapeOp);
	else
	{
		cerr << __FUNCTION__": Failed to deserialize Shape in PhysicComponent.\n";
		this->setShape(E_NO_SHAPE);
	}

	if (std::optional<float> massOp = parseMass(map); massOp)
		this->setMass(*massOp);
	else
	{
		cerr << __FUNCTION__": Failed to deserialize Mass in PhysicComponent.\n";
		this->setMass(0);
	}

	if (std::optional<bool> physicUpdateOp = parsePhysicUpdate(map); physicUpdateOp)
		this->setCanPhysicUpdate(*physicUpdateOp);
	else
	{
		cerr << __FUNCTION__": Failed to deserialize canPhysicUpdate in PhysicComponent.\n";
		this->setCanPhysicUpdate(false);
	}

	if (std::optional<Vector3f> scaleOp = parseScale(map); scaleOp)
		this->setScale(*scaleOp);
	else
	{
		cerr << __FUNCTION__": Failed to deserialize scale in PhysicComponent.\n";
		this->setScale({1, 1, 1 });
	}

	if (std::optional<std::string> tagOp = parseTag(map); tagOp)
		this->setTag(*tagOp);
	else
	{
		cerr << __FUNCTION__": Failed to deserialize tag in PhysicComponent.\n";
		this->setTag("None");
	}

	if (std::optional<float> fricOp = parseFriction(map); fricOp)
		this->setFriction(*fricOp);
	else
	{
		cerr << __FUNCTION__": Failed to deserialize friction in PhysicComponent.\n";
		this->setFriction(0);
	}

	if (std::optional<bool> transX = parseFreezeTranslationX(map); transX)
	{
		if (*transX)
			this->setFreezeTranslation(0, true);
	}
	else
		cerr << __FUNCTION__": Failed to deserialize Freeze Translation X in PhysicComponent.\n";

	if (std::optional<bool> transY = parseFreezeTranslationY(map); transY)
	{
		if (*transY)
			this->setFreezeTranslation(1, true);
	}
	else
		cerr << __FUNCTION__": Failed to deserialize Freeze Translation Y in PhysicComponent.\n";

	if (std::optional<bool> transZ = parseFreezeTranslationZ(map); transZ)
	{
		if (*transZ)
			this->setFreezeTranslation(2, true);
	}
	else
		cerr << __FUNCTION__": Failed to deserialize Freeze Translation Z in PhysicComponent.\n";

	if (std::optional<bool> transX = parseFreezeRotationX(map); transX)
	{
		if (*transX)
			this->setFreezeRotation(0, true);
	} else
		cerr << __FUNCTION__": Failed to deserialize Freeze Rotation X in PhysicComponent.\n";

	if (std::optional<bool> transY = parseFreezeRotationY(map); transY)
	{
		if (*transY)
			this->setFreezeRotation(1, true);
	} else
		cerr << __FUNCTION__": Failed to deserialize Freeze Rotation Y in PhysicComponent.\n";

	if (std::optional<bool> transZ = parseFreezeRotationZ(map); transZ)
	{
		if (*transZ)
			this->setFreezeRotation(2, true);
	} else
		cerr << __FUNCTION__": Failed to deserialize Freeze Rotation Z in PhysicComponent.\n";

	return true;
}

void PhysicComponent::defineLuaMembers(sol::usertype<PhysicComponent>& type)
{
	Component::defineLuaMembers(type);

	type["mass"]             = sol::property(&getMass, &setMass);
	type["scale"]            = sol::property(&getScale, &setScale);
	type["canPhysicsUpdate"] = sol::property(&getCanPhysicUpdate, &setCanPhysicUpdate);
	type["linearVelocity"]	 = sol::property(&getLinearVelocity, &setLinearVelocity);

	type["addForce"]		 = &addForce;
	type["addImpulse"]		 = &addImpulse;
	type["setPushVelocity"]	 = &setPushVelocity;
	type["compareTag"]		 = &compareTag;
	type["setPosition"]		 = &setRigidBodyPosition;
	type["resetVelocity"]	 = &resetVelocity;
}

void PhysicComponent::getProperties(std::vector<Property>& out) noexcept
{
	Base::getProperties(out);

	// TODO Potentially block in playing
	out.emplace_back("Physics Enabled",
	[this]() -> bool { return getCanPhysicUpdate(); },
	[this](const bool& value) { setCanPhysicUpdate(value); });

	// TODO Potentially block in playing
	out.emplace_back("Mass",
	[this]() -> float { return getMass(); },
	[this](const float& value) { setMass(value); });

	out.emplace_back("Friction",
	[this]() -> float { return getFriction(); },
	[this](const float& value) { setFriction(value); });
}
