#pragma once

#include "Resource.h"

#include "Rendering/Material.h"
#include "Rendering/Mesh.h"

namespace KaputEngine::Resource
{
	class MeshResource final :
		public Resource,
		public std::enable_shared_from_this<MeshResource>
	{
		RESOURCE_DEFS(MeshResource, "Mesh")

	public:
		MeshResource(ConstructorBlocker, Bind<std::filesystem::path> auto&& path)
			: Resource(std::forward<decltype(path)>(path)), m_root(*this) { }

		~MeshResource() final;

		std::future<void>& load(std::string&& content, eMultiThreadPolicy policy = eMultiThreadPolicy::MONO_THREAD);
		void unload() final;

		std::shared_ptr<const Rendering::Mesh> root() const noexcept;

	private:
		_NODISCARD _Success_(return) bool deserializeMap(_In_ const Text::Xml::XmlNode::Map& map) final;
		void serializeValues(Text::Xml::XmlSerializeContext& context) const final;

		std::filesystem::path m_modelPath;

		std::vector<Rendering::Mesh> m_meshes;
		std::vector<Rendering::Material> m_materials;

		Rendering::Mesh m_root;
	};
}
