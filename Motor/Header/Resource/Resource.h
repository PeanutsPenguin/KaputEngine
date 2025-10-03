#pragma once

#include "RegistryDefs.h"
#include "Text/Xml/Parser.h"
#include "Text/Xml/Serializer.h"
#include "Utils/Bind.h"
#include "Utils/Policy.h"

#include <filesystem>
#include <future>
#include <stop_token>

#define RESOURCE_DEFS(type, name) \
public: static constexpr const char* TypeHeader = name; \
private: _NODISCARD _Ret_notnull_ const char* xmlTypeName() const noexcept final; \
REGISTER_FINAL_SIG(type);

#define RESOURCE_IMPL(type) \
_Ret_notnull_ const char* KaputEngine::Resource::##type##::xmlTypeName() const noexcept \
{ \
	return TypeHeader; \
} \
REGISTER_FINAL_IMPL(type, ResourceManager::registerType<type>())

namespace KaputEngine::Resource
{
	class Resource :
		protected Text::Xml::IXmlMapParser,
		public Text::Xml::IXmlPolymorphicSerializer
	{
	public:
		enum class eLoadState : uint8_t
		{
			/// <summary>
			/// Loaded or in the process of being loaded (flag)
			/// </summary>
			LOAD = 1 << 1,
			/// <summary>
			/// Load or unload operation in progress (flag)
			/// </summary>
			PROCESSING = 1 << 2,
			/// <summary>
			/// Unloaded with no load or unload operation in progress
			/// </summary>
			UNLOADED = 0,
			/// <summary>
			/// Loaded with no load or unload operation in progress
			/// </summary>
			LOADED = LOAD,
			/// <summary>
			/// Load operation in progress
			/// </summary>
			LOADING = LOAD | PROCESSING,
			/// <summary>
			/// Unload operation in progress
			/// </summary>
			UNLOADING = PROCESSING
		};

		Resource(const Resource&) = delete;
		Resource(Resource&&) = delete;

		_NODISCARD bool isKasset() const noexcept;

		virtual ~Resource();

		template <std::derived_from<Resource> T, typename... Args>
		_NODISCARD static std::shared_ptr<T> create(Bind<std::filesystem::path> auto&& path, Args&&... args)
		{
			return std::make_shared<T>(ConstructorBlocker(), FORWARD(path), std::forward<Args>(args)...);
		}

		_NODISCARD const std::optional<std::filesystem::path>& path() const noexcept;

		_NODISCARD eLoadState loadState() const noexcept;
		_NODISCARD bool processing() const noexcept;

		std::future<void>& loadExisting(eMultiThreadPolicy policy = eMultiThreadPolicy::MONO_THREAD);
		virtual std::future<void>& load(std::string&& content, eMultiThreadPolicy policy = eMultiThreadPolicy::MONO_THREAD) = 0;

		virtual void unload() = 0;
		void cancelLoad();

		void waitLoad() const;

	protected:
		struct ConstructorBlocker
		{
			explicit ConstructorBlocker() = default;
		};

		_NODISCARD bool startLoad() noexcept;
		_NODISCARD bool startUnload() noexcept;
		_NODISCARD bool cancelForDestroy() noexcept;
		_NODISCARD std::future<void>& noFuture() noexcept;

		explicit Resource() = default;
		explicit Resource(Bind<std::filesystem::path> auto&& path) : m_path(FORWARD(path)) { }

		_NODISCARD _Success_(return) bool deserializeForLoad(const std::string& content);

		std::atomic<eLoadState> m_loadState = eLoadState::UNLOADED;
		std::optional<std::filesystem::path> m_path;
		std::stop_source m_stopSource;
		std::future<void> m_loadFuture;
	};
}
