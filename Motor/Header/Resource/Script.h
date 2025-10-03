#pragma once

#include "Resource/Resource.h"

#include "GameObject/Script.h"
#include "Utils/Bind.h"

#include <filesystem>

namespace KaputEngine::Resource
{
	class ScriptResource final :
		public Resource,
		public std::enable_shared_from_this<ScriptResource>
	{
		RESOURCE_DEFS(ScriptResource, "Script")

	public:
		explicit ScriptResource(ConstructorBlocker, Bind<std::filesystem::path> auto&& path):
			Resource(std::forward<decltype(path)>(path)),
			m_data(*this) { }

		~ScriptResource() final;

		std::future<void>& load(std::string&& content, eMultiThreadPolicy policy = eMultiThreadPolicy::MONO_THREAD) final;

		void unload() final;

		_NODISCARD const KaputEngine::Script& data() const noexcept;
		_NODISCARD std::shared_ptr<const KaputEngine::Script> dataPtr() const noexcept;

		void serializeValues(Text::Xml::XmlSerializeContext& context) const final;

	private:
		_NODISCARD _Success_(return) bool deserializeMap(_In_ const Text::Xml::XmlNode::Map& map) final;

		KaputEngine::Script m_data;
		std::filesystem::path m_luaPath { };
	};
}
