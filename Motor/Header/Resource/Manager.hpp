#pragma once

#include "Manager.h"

#include "Text/Xml/Parser.h"
#include "Utils/rawalloc.hpp"

#include <concepts>
#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>

namespace KaputEngine::Resource
{
	template <ManagableResource T>
	_Ret_maybenull_ std::shared_ptr<T> ResourceManager::get(Bind<std::filesystem::path> auto&& _path, const bool autoLoad)
	{
		using std::cerr;
		using std::string;
		using std::string_view;

		std::lock_guard lock(s_resourceMutex);

		decltype(auto) path = bindGet<std::filesystem::path>(FORWARD(_path));

		// Resource previously requested, create a shared ptr from existing allocation
		if (const auto it = s_resources.find(path); it != s_resources.end())
		{
			const std::shared_ptr<Resource> resourcePtr = it->second.lock();

			if constexpr (!std::same_as<T, Resource>)
				if (std::shared_ptr<T> castResourcePtr = std::dynamic_pointer_cast<T>(resourcePtr))
					return castResourcePtr;
				else
				{
					// TODO Mention actual type of resource
					cerr << __FUNCTION__": Attempt to load resource \"" << path << "\" as type " << T::TypeHeader << " but is of incompatible type.\n";
					return nullptr;
				}
		}

		if (path.extension() != ".kasset")
		{
			cerr << __FUNCTION__": Cannot fetch non-loaded resource \"" << path << "\" that is not kasset.\n";
			return nullptr;
		}

		std::ifstream file(path, std::ios::in);

		if (!file.is_open())
		{
			cerr << __FUNCTION__": Error opening file: " << path << '\n';

			if constexpr (std::same_as<T, Resource>)
				return nullptr;
			else
			{
				std::shared_ptr<T> fakeRes = Resource::create<T>(path);
				s_resources.insert({ FORWARD(path), fakeRes });

				return fakeRes;
			}
		}

		string content(std::istreambuf_iterator { file }, {});
		file.close();

		string_view view = content;
		rawalloc<string_view> sectionNameAlloc;

		if (!Text::Xml::XmlParser::parseTagName(view, sectionNameAlloc.val()))
			if constexpr (std::same_as<T, Resource>)
				return nullptr;
			else
			{
				std::shared_ptr<T> fakeRes = Resource::create<T>(path);
				s_resources.insert({ FORWARD(path), fakeRes });

				return fakeRes;
			}

		const auto sectionName = string(sectionNameAlloc.val());

		if constexpr (!std::same_as<T, Resource>)
			if (sectionName != T::TypeHeader)
			{
				cerr << __FUNCTION__": Attempt to load resource \"" << path << "\" as type " << T::TypeHeader << " but is of incompatible type " << sectionName << ".\n";

				std::shared_ptr<T> fakeRes = Resource::create<T>(path);
				s_resources.insert({ FORWARD(path), fakeRes });

				return fakeRes;
			}

		const std::shared_ptr<Resource> ptr = s_createFuncs[sectionName](path);

		s_resources.insert({ FORWARD(path), ptr });

		if (autoLoad)
			std::static_pointer_cast<T>(ptr)->load(std::move(content), eMultiThreadPolicy::MULTI_THREAD);

		return std::static_pointer_cast<T>(ptr);
	}

	template <ManagableResource T>
	void ResourceManager::registerType()
	{
		s_createFuncs[T::TypeHeader] = [](const std::filesystem::path& path) -> std::shared_ptr<Resource>
		{
			return Resource::create<T>(path);
		};
	}
}
