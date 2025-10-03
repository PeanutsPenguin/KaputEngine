#pragma once

#include "Scene/Transform/MatrixSource.h"

#include "Rendering/Buffer/ElementBuffer.h"
#include "Rendering/Buffer/VertexAttributeBuffer.h"
#include "Rendering/Buffer/VertexBuffer.h"

#include <vector>

namespace KaputEngine::Resource
{
    class MeshResource;
}

class aiMesh;

namespace KaputEngine::Rendering
{
    class Mesh : public MatrixTransformSource
    {
    public:
        Mesh() = default;
        explicit Mesh(const Mesh& parent);
        Mesh(Resource::MeshResource& resource);
        Mesh(Mesh&&) noexcept = default;

        ~Mesh() = default;

        Mesh& operator=(Mesh&&) noexcept = default;

        _Success_(return) bool init(const aiMesh& mesh, _In_opt_ const std::shared_ptr<class Material>& mat);

        void destroy();

        _NODISCARD Buffer::VertexBuffer& vertices() noexcept;
        _NODISCARD const Buffer::VertexBuffer& vertices() const noexcept;

        _NODISCARD Buffer::VertexAttributeBuffer& attributes() noexcept;
        _NODISCARD const Buffer::VertexAttributeBuffer& attributes() const noexcept;

        _NODISCARD Buffer::ElementBuffer& elements() noexcept;
        _NODISCARD const Buffer::ElementBuffer& elements() const noexcept;

        /// <summary>
        /// Legacy draw
        /// </summary>
        void draw() const;

        void draw(const TransformSource& parent, const class Material& material, const class ShaderProgram& program) const;

		_NODISCARD _Ret_maybenull_ Resource::MeshResource* parentResource() noexcept;
		_NODISCARD _Ret_maybenull_ const Resource::MeshResource* parentResource() const noexcept;

        /// <summary>
        /// Returns the parent transform.
        /// </summary>
        /// <returns>If root mesh, either null or the provided parent if called during a draw operation.</returns>
        _NODISCARD _Ret_maybenull_ const TransformSource* getParentTransform() const noexcept final;

    private:
        /// <summary>
        /// Parent mesh, object or component
        /// </summary>
        _Maybenull_ mutable const TransformSource* m_parent = nullptr;

        std::vector<std::reference_wrapper<Mesh>> m_children;
        _Maybenull_ std::shared_ptr<class Material> m_material = nullptr;

        Buffer::VertexBuffer m_vertexBuffer;
        Buffer::VertexAttributeBuffer m_vertexAttributeBuffer;
        Buffer::ElementBuffer m_elementBuffer;

		Resource::MeshResource* m_resource = nullptr;
    };
}
