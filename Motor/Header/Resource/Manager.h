#pragma once

#include "Resource.h"

#include "Utils/Bind.h"

#include <concepts>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace KaputEngine::Resource
{
	template <typename T>
	concept ManagableResource =
		std::same_as<Resource, T> ||
		(std::derived_from<T, Resource>
		&& std::derived_from<T, std::enable_shared_from_this<T>>);

	class ResourceManager
	{
		friend Resource;

	public:
		static void add(_In_ std::shared_ptr<Resource> resource);

		template <ManagableResource T = Resource>
		static _Ret_maybenull_ std::shared_ptr<T> get(Bind<std::filesystem::path> auto&& path, bool autoLoad = true);

		static void remove(const std::filesystem::path& path);

		template <ManagableResource T>
		static void registerType();

	private:
		using CreateFunc = std::shared_ptr<Resource>(const std::filesystem::path& path);

		static std::unordered_map<std::string, CreateFunc*> s_createFuncs;
		static std::unordered_map <std::filesystem::path,
			std::weak_ptr<Resource>> s_resources;
		static std::recursive_mutex s_resourceMutex;
	};
}
