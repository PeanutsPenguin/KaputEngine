#pragma once

#include "Resource/Resource.h"

#include "Rendering/Shader.h"

namespace KaputEngine::Resource
{
	class ShaderResource final :
		public Resource,
		public std::enable_shared_from_this<ShaderResource>
	{
		RESOURCE_DEFS(ShaderResource, "Shader")

	public:
		explicit ShaderResource(ConstructorBlocker, Bind<std::filesystem::path> auto&& path) :
			Resource(std::forward<decltype(path)>(path)), m_shaderType(GL_INVALID_ENUM),
			m_data(*this) { }

		~ShaderResource() final;

		std::future<void>& load(std::string&& content, eMultiThreadPolicy policy = eMultiThreadPolicy::MONO_THREAD);

		void unload() final;

		_NODISCARD const std::filesystem::path& shaderPath() const noexcept;

		const Rendering::Shader& data() const noexcept;
		std::shared_ptr<const Rendering::Shader> dataPtr() const noexcept;

		void serializeValues(Text::Xml::XmlSerializeContext& context) const final;

	private:
		static GLenum mapShaderType(const std::string& name);

		_NODISCARD _Success_(return) bool deserializeMap(_In_ const Text::Xml::XmlNode::Map& map) final;

		GLenum m_shaderType;
		Rendering::Shader m_data;
		std::filesystem::path m_shaderPath;
	};
}
