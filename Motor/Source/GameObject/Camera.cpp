#include "GameObject/Camera.h"

#include "Application.h"
#include "GameObject/GameObject.hpp"
#include "Inspector/Property.hpp"
#include "Window/WindowConfig.h"

using namespace KaputEngine;

using KaputEngine::Inspector::Property;

using LibMath::Vector2f;
using LibMath::Vector3f;
using LibMath::Matrix4f;
using LibMath::Quaternionf;

GAMEOBJECT_IMPL(Camera)

Camera::Camera(ConstructorBlocker) : Camera(ConstructorBlocker(), s_nextId++) { }

Camera::Camera(ConstructorBlocker, const Id& id) : GameObject(ConstructorBlocker(), id),
	m_worldUp(Vector3f::Dir::up())
{
	Vector2f size = Application::getWindow().getSize();
	this->m_aspect = size.x() / size.y();
	updateWorldTransform();
}

void Camera::updateWorldTransform() const noexcept
{
	GameObject::updateWorldTransform();

	Quaternionf Xrot = m_worldTransform.fromAxis(Vector3f::Dir::up(), this->m_yaw);
	Quaternionf Yrot = m_worldTransform.fromAxis(Vector3f::Dir::right(), this->m_pitch);

	m_worldTransform.rotation = Xrot * Yrot;

	m_front          = m_worldTransform.rotation.rotate(Vector3f::Dir::back());
	m_right          = m_front.cross(m_worldUp).normalize();
	m_up             = m_right.cross(m_front).normalize();
	m_viewProjection = getProjectionMatrix() * getViewMatrix();
}

void Camera::defineLuaMembers(sol::usertype<Camera>& type)
{
	GameObject::defineLuaMembers<Camera>(type);

	type["yaw"] = sol::property(&getYaw, &setYaw);
	type["pitch"] = sol::property(&getPitch, &setPitch);

	type["position"] = sol::property(
	[](Camera& cam) -> Vector3f
	{
		return cam.getWorldTransform().position.as<LibMath::Vector>();
	}, [](Camera& cam, Vector3f pos)
	{
		Transform newTrans = cam.getWorldTransform();
		newTrans.position = pos;
		cam.setWorldTransformWithoutPhysic(newTrans);
	});

	type["right"] = sol::readonly_property([](Camera& cam) -> Vector3f
	{
		return cam.getRight();
	});

	type["front"] = sol::readonly_property([](Camera& cam) -> Vector3f
	{
		return cam.getFront();
	});
}

Matrix4f Camera::getViewMatrix() const noexcept
{
	Matrix4f view = Matrix4f::Identity();

	const Vector3f vecs[] =
	{
		m_right,
		m_up,
		-m_front
	};

	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			view.raw2D()[i][j] = vecs[j].raw()[i];

	for (int i = 0; i < 3; ++i)
		view.raw2D()[3][i] = -vecs[i].dot(m_worldTransform.position);

	view.raw2D()[3][3] = 1.f;

	return view;
}

Matrix4f Camera::getProjectionMatrix() const noexcept
{
	Matrix4f projection = Matrix4f::Identity();

	const float
		fov    = WindowConfig::DEFAULT_FOV,
		near   = WindowConfig::DEFAULT_NEAR,
		far    = WindowConfig::DEFAULT_FAR,
		distanceDelta = far - near,
		tanHalfFovy   = tan(fov / 2.f);

	float (&data)[4][4] = projection.raw2D();

	data[0][0] = 1.f / (this->m_aspect * tanHalfFovy);
	data[1][1] = 1.f / tanHalfFovy;
	data[2][2] = -(far + near) / distanceDelta;
	data[2][3] = -1.f;
	data[3][2] = -(2.f * far * near) / distanceDelta;
	data[3][3] = 0.f;

	return projection;
}

const Vector3f& Camera::getFront() const noexcept
{
	if (m_dirtyTransform)
		updateWorldTransform();

	return m_front;
}

const Matrix4f& Camera::getViewProjectionMatrix() const noexcept
{
	if (m_dirtyTransform)
		updateWorldTransform();

	return m_viewProjection;
}

void Camera::setAspect(const float aspect)
{
	this->m_aspect = aspect;
	setTransformDirty();
}

void Camera::setYaw(float yaw)
{
	this->m_yaw = yaw;
	setTransformDirty();
}

void Camera::setPitch(float pitch)
{
	this->m_pitch = pitch;
	setTransformDirty();
}

void Camera::setFront(const Vector3f& front)
{
	this->m_front = front;
	setTransformDirty();
}

float Camera::getYaw() const noexcept
{
	return this->m_yaw;
}

float Camera::getPitch() const noexcept
{
	return this->m_pitch;
}

const Vector3f& Camera::getRight() const noexcept
{
	return this->m_right;
}

void Camera::getProperties(std::vector<Property>& out) noexcept
{
    Base::getProperties(out);

    out.emplace_back("Is Primary Camera",
    [this]() -> bool
    {
        const Scene* const scene = parentScene();

		if (!scene)
			return false;

		std::weak_ptr<const Camera> primary = scene->getPrimaryCamera();

		if (primary.expired())
			return false;

        return primary.lock()->id() == id();
    },
    [this](const bool& value)
    {
        Scene* const scene = parentScene();

		if (!scene)
			return;

        if (value)
            scene->setPrimaryCamera(*this);
        else
            scene->setPrimaryCamera(nullptr);
    });
}
