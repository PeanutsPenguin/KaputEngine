#include "Queue/Context.h"
#include "Rendering/Buffer/VertexAttributeBuffer.hpp"
#include "Rendering/Material.h"
#include "Rendering/Mesh.h"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/Vertex.h"

#include <assimp/mesh.h>
#include <glad/glad.h>

using KaputEngine::Rendering::Mesh;

using KaputEngine::TransformSource;
using KaputEngine::Rendering::Buffer::ElementBuffer;
using KaputEngine::Rendering::Buffer::VertexAttributeBuffer;
using KaputEngine::Rendering::Buffer::VertexBuffer;
using KaputEngine::Resource::MeshResource;

using LibMath::Matrix4f;
using KaputEngine::Queue::ContextQueue;

using std::cerr;

Mesh::Mesh(const Mesh& parent) : m_parent(&parent) { }

Mesh::Mesh(MeshResource& resource) : m_resource(&resource) { }

_Success_(return) bool Mesh::init(const aiMesh& mesh, _In_opt_ const std::shared_ptr<Material>& mat)
{
	m_material = mat;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	vertices.reserve(mesh.mNumVertices);
	indices.reserve(static_cast<size_t>(mesh.mNumFaces) * 3);

	for (size_t i = 0; i < mesh.mNumVertices; ++i)
	{
		const aiVector3D
			&pos       = mesh.mVertices[i],
			&normal    = mesh.mNormals[i],
			&uv        = mesh.mTextureCoords[0][i],
			&tangent   = mesh.mTangents[i],
			&bitangent = mesh.mBitangents[i];

		vertices.push_back(Vertex
		{
			.position  = { pos.x, pos.y, pos.z },
			.textureUV = { uv.x, uv.y },
			.normal    = { normal.x, normal.y, normal.z },
			.tangent   = { tangent.x, tangent.y, tangent.z },
			.bitangent = { bitangent.x, bitangent.y, bitangent.z }
		});
	}

	for (size_t i = 0; i < mesh.mNumFaces; ++i)
	{
		const aiFace& face = mesh.mFaces[i];

		if (face.mNumIndices != 3)
		{
			cerr << __FUNCTION__": Mesh has an incorrect number of points in face.\n";
			return false;
		}

		indices.emplace_back(face.mIndices[0]);
		indices.emplace_back(face.mIndices[1]);
		indices.emplace_back(face.mIndices[2]);
	}

	ContextQueue::instance().push([this, &vertices, &indices]()
	{
		m_vertexBuffer.create(vertices.data(), vertices.size() * sizeof(Vertex));
		m_elementBuffer.create(indices);

		m_vertexAttributeBuffer.create();
		m_vertexAttributeBuffer.defineAttribute(0, &Vertex::albedo);
		m_vertexAttributeBuffer.defineAttribute(1, &Vertex::position);
		m_vertexAttributeBuffer.defineAttribute(2, &Vertex::textureUV);
		m_vertexAttributeBuffer.defineAttribute(3, &Vertex::normal);
		m_vertexAttributeBuffer.defineAttribute(4, &Vertex::tangent);
		m_vertexAttributeBuffer.defineAttribute(5, &Vertex::bitangent);
	}).wait();

	return true;
}

void Mesh::destroy()
{
	m_vertexBuffer.destroy();
	m_vertexAttributeBuffer.destroy();
	m_elementBuffer.destroy();
}

VertexBuffer& Mesh::vertices() noexcept
{
	return m_vertexBuffer;
}
const VertexBuffer& Mesh::vertices() const noexcept
{
	return m_vertexBuffer;
}

VertexAttributeBuffer& Mesh::attributes() noexcept
{
	return m_vertexAttributeBuffer;
}
const VertexAttributeBuffer& Mesh::attributes() const noexcept
{
	return m_vertexAttributeBuffer;
}

ElementBuffer& Mesh::elements() noexcept
{
	return m_elementBuffer;
}
const ElementBuffer& Mesh::elements() const noexcept
{
	return m_elementBuffer;
}

void Mesh::draw() const
{
	if (!m_vertexBuffer.valid())
		return;

	m_vertexAttributeBuffer.bind();
	m_vertexBuffer.bind();
	m_elementBuffer.bind();

	ContextQueue::instance().push([this]
	{
		glDrawElements(GL_TRIANGLES, m_elementBuffer.count(), GL_UNSIGNED_INT, nullptr);
	}).wait();
}

void Mesh::draw(const TransformSource& parent, const Material& material, const ShaderProgram& program) const
{
	if (!program.use())
		return;

	bool isRoot = !m_parent;

	if (isRoot)
	{
		m_parent = &parent;
		setTransformDirty();
	}

	program.setUniform("model", getWorldTransformMatrix());

	MaterialLayer layer
	{
		.primary  = material,
		.fallback = std::to_address(m_material)
	};

	program.setUniform("materialSampler.albedo.mode", layer);

	draw();

	for (const Mesh& child : m_children)
		child.draw(*this, material, program);

	if (isRoot)
	{
		m_parent = nullptr;
		setTransformDirty();
	}
}

_Ret_maybenull_ MeshResource* Mesh::parentResource() noexcept
{
	return m_resource;
}
_Ret_maybenull_ const MeshResource* Mesh::parentResource() const noexcept
{
	return m_resource;
}

_Ret_maybenull_ const TransformSource* Mesh::getParentTransform() const noexcept
{
	return m_parent;
}
