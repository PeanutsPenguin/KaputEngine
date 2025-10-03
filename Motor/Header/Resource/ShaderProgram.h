#pragma once

#include "Resource/Resource.h"

#include "Rendering/ShaderProgram.h"

namespace KaputEngine::Resource
{
	class ShaderProgramResource final:
		public Resource,
		public std::enable_shared_from_this<ShaderProgramResource>
	{
		RESOURCE_DEFS(ShaderProgramResource, "ShaderProgram")

	public:
		explicit ShaderProgramResource(ConstructorBlocker, Bind<std::filesystem::path> auto&& path) :
			Resource(std::forward<decltype(path)>(path)),
			m_data(*this) { }

		~ShaderProgramResource() final;

		std::future<void>& load(std::string&& content, eMultiThreadPolicy policy = eMultiThreadPolicy::MONO_THREAD) final;

		void unload() override;

		_NODISCARD const std::vector<std::filesystem::path>& shaderPaths() const noexcept;

		_NODISCARD const Rendering::ShaderProgram& data() const noexcept;
		_NODISCARD std::shared_ptr<const Rendering::ShaderProgram> dataPtr() const noexcept;

		static std::shared_ptr<const ShaderProgramResource> defaultProgram();

		void serializeValues(Text::Xml::XmlSerializeContext& context) const final;

	private:
		_NODISCARD _Success_(return) bool deserializeMap(_In_ const Text::Xml::XmlNode::Map& map) final;

		Rendering::ShaderProgram m_data;
		std::vector<std::filesystem::path> m_shaderPaths;
	};
}
