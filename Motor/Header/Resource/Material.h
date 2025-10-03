#pragma once

#include "Rendering/Material.h"
#include "Resource/Resource.h"
#include "Utils/Policy.h"

namespace KaputEngine::Resource
{
	class MaterialResource final :
		public Resource,
		public std::enable_shared_from_this<MaterialResource>
	{
		RESOURCE_DEFS(MaterialResource, "Material")

	public:
		MaterialResource(ConstructorBlocker, Bind<std::filesystem::path> auto&& path) :
			Resource(std::forward<decltype(path)>(path)),
			m_data(*this) { }

		~MaterialResource() final;

		std::future<void>& load(std::string&& content, eMultiThreadPolicy policy = eMultiThreadPolicy::MONO_THREAD) final;

		void unload() final;

		_NODISCARD Rendering::Material& data() noexcept;
		_NODISCARD const Rendering::Material& data() const noexcept;

		_NODISCARD std::shared_ptr<Rendering::Material> dataPtr() noexcept;
		_NODISCARD std::shared_ptr<const Rendering::Material> dataPtr() const noexcept;

		_NODISCARD const static std::shared_ptr<const MaterialResource> defaultMaterial() noexcept;

		void serializeValues(Text::Xml::XmlSerializeContext& context) const final;

	private:
		_NODISCARD _Success_(return) bool deserializeMap(_In_ const Text::Xml::XmlNode::Map& map) final;

		template <typename TData>
		_NODISCARD _Success_(return) bool deserializeSampler(Rendering::Sampler<TData>& sampler, const Text::Xml::XmlNode& node);

		template <typename TData>
		void serializeSampler(const Rendering::Sampler<TData>& sampler, Text::Xml::XmlSerializeContext& context) const;

		_NODISCARD _Success_(return) bool deserializeSamplerTexture(Rendering::TextureSample& sample, const Text::Xml::XmlNode& node);

		Rendering::Material m_data;
	};
}

#include "Material.hpp"
