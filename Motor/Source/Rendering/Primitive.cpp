#include "Rendering/Buffer/VertexAttributeBuffer.hpp"
#include "Rendering/Primitive.h"
#include "Rendering/Vertex.h"

#include <LibMath/Trigonometry.h>

#include <vector>

using namespace KaputEngine::Rendering;

using LibMath::g_pi;
using LibMath::Radian;

Sphere::Sphere(float radius, int sectors, int stacks)
{
	std::vector<Vertex> vertices;

	float sectorStep = 2 * g_pi / sectors;
	float stackStep = g_pi / stacks;
	float	sectorAngle,
		stackAngle,
		xy;

	for (int i = 0; i <= stacks; ++i)
	{
		Vertex toadd;
		stackAngle = g_pi / 2 - i * stackStep;			// starting from pi/2 to -pi/2
		xy = radius * LibMath::cos(Radian(stackAngle));							// r * cos(u)
		toadd.position.z() = radius * LibMath::sin(Radian(stackAngle));         // r * sin(u)

		for (int j = 0; j <= sectors; ++j)
		{
			sectorAngle = j * sectorStep;						// starting from 0 to 2pi

			toadd.position.x() = xy * LibMath::cos(Radian(sectorAngle));        // r * cos(u) * cos(v)
			toadd.position.y() = xy * LibMath::sin(Radian(sectorAngle)); 		// r * cos(u) * sin(v)
			vertices.push_back(toadd);
		}
	}

	std::vector<uint32_t> indices;
	int k1, k2;
	for (int i = 0; i < stacks; ++i)
	{
		k1 = i * (sectors + 1);     // beginning of current stack
		k2 = k1 + sectors + 1;      // beginning of next stack

		for (int j = 0; j < sectors; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			// k1+1 => k2 => k2+1
			if (i != (stacks - 1))
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}

	this->m_radius = radius;
	this->vertices().create(vertices.data(), (vertices.size() - 1) * sizeof(Vertex));
	this->elements().create(indices);
	this->attributes().create();
	this->attributes().defineAttribute(0, &Vertex::albedo);
	this->attributes().defineAttribute(1, &Vertex::position);
	this->attributes().defineAttribute(2, &Vertex::textureUV);
	this->attributes().defineAttribute(3, &Vertex::normal);
}

float Sphere::getRadius() const noexcept
{
	return this->m_radius;
}

Capsule::Capsule(float radius, float height, int sectors, int stacks)
{
	std::vector<Vertex> vertices;

	float sectorStep = 2 * g_pi / sectors;
	float stackStep = g_pi / stacks;
	float	sectorAngle,
		stackAngle,
		xy;

	for (int i = 0; i <= stacks; ++i)
	{
		if (i == stacks / 2)
			height = -height;

		Vertex toadd;
		stackAngle = g_pi / 2 - i * stackStep;										// starting from pi/2 to -pi/2
		xy = radius * LibMath::cos(Radian(stackAngle));							// r * cos(u)
		toadd.position.z() = radius * LibMath::sin(Radian(stackAngle)) + height;			// r * sin(u)

		for (int j = 0; j <= sectors; ++j)
		{
			sectorAngle = j * sectorStep;													// starting from 0 to 2pi

			toadd.position.x() = xy * LibMath::cos(Radian(sectorAngle));			// r * cos(u) * cos(v)
			toadd.position.y() = (xy * LibMath::sin(Radian(sectorAngle)));	// r * cos(u) * sin(v)
			vertices.push_back(toadd);
		}
	}

	std::vector<uint32_t> indices;
	int k1, k2;
	for (int i = 0; i < stacks; ++i)
	{
		k1 = i * (sectors + 1);     // beginning of current stack
		k2 = k1 + sectors + 1;      // beginning of next stack

		for (int j = 0; j < sectors; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			// k1+1 => k2 => k2+1
			if (i != (stacks - 1))
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}

	this->m_height = height;
	this->m_radius = radius;

	this->vertices().create(vertices.data(), (vertices.size() - 1) * sizeof(Vertex));
	this->elements().create(indices);
	this->attributes().create();
	this->attributes().defineAttribute(0, &Vertex::albedo);
	this->attributes().defineAttribute(1, &Vertex::position);
	this->attributes().defineAttribute(2, &Vertex::textureUV);
	this->attributes().defineAttribute(3, &Vertex::normal);
}

float Capsule::getRadius() const noexcept
{
	return this->m_radius;
}

float Capsule::getHeight() const noexcept
{
	return this->m_height;
}

SecondCube::SecondCube(LibMath::Vector3f size)
{
	Vertex vertarray[8];
	vertarray[0].position = { -size.x(), size.y(), -size.z() };	//BackUpLeft
	vertarray[1].position = { size.x(), size.y(), -size.z() };	//BackUpRight
	vertarray[2].position = { -size.x(), -size.y(), -size.z() };	//BackDownLeft
	vertarray[3].position = { size.x(), -size.y(), -size.z() };	//BackDownRight
	vertarray[4].position = { -size.x(), size.y(), size.z() };	//FrontUpLeft
	vertarray[5].position = { size.x(), size.y(), size.z() };	//FrontUpRight
	vertarray[6].position = { -size.x(), -size.y(), size.z() };	//FrontDownLeft
	vertarray[7].position = { size.x(), -size.y(), size.z() };	//FrontDownRight

	std::vector<uint32_t> indices =
	{
		//BackFace
		0, 1, 2,
		1, 3, 2,
		//Front
		4, 5, 6,
		5, 7, 6,
		//Right
		5, 1, 7,
		5, 3, 7,
		//Left
		4, 0, 2,
		2, 6, 4,
		//Top
		0, 1, 4,
		4, 5, 1,
		//Bottom
		6, 2, 3,
		3, 7, 6
	};

	this->m_size = size;
	this->vertices().create(&vertarray, 8 * sizeof(Vertex));
	this->elements().create(indices);
	this->attributes().create();
	this->attributes().defineAttribute(0, &Vertex::albedo);
	this->attributes().defineAttribute(1, &Vertex::position);
	this->attributes().defineAttribute(2, &Vertex::textureUV);
	this->attributes().defineAttribute(3, &Vertex::normal);
}
