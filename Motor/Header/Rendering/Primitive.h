#pragma once

#include "Rendering/Mesh.h"

#include <LibMath/Vector/Vector3.h>

namespace KaputEngine::Rendering
{
	//Interface Class to regroup every form, it's an easy way to create a mesh that as a primitive form
	class IPrimitives : public Mesh
	{
	public:
		IPrimitives() = default;

		~IPrimitives() = default;
	};

	class SecondCube : public IPrimitives
	{
	public:
		SecondCube() = delete;

		SecondCube(LibMath::Vector3f size);

	private:
		LibMath::Vector3f m_size;
	};

	class Sphere : public IPrimitives
	{
	public:
		Sphere() = delete;

		Sphere(float radius, int sectors, int stacks);

		_NODISCARD float getRadius() const noexcept;

	private:
		float m_radius;
	};

	class Capsule : public IPrimitives
	{
	public:

		Capsule() = delete;

		Capsule(float radius, float height, int sectors, int stacks);

		_NODISCARD float getRadius() const noexcept;

		_NODISCARD float getHeight() const noexcept;
	private:
		float m_radius;
		float m_height;
	};
}
