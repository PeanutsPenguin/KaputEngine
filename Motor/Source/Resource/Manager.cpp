#include "Resource/Manager.h"

#include <iostream>

using namespace KaputEngine::Resource;

using std::cerr;

decltype(ResourceManager::s_resources)     ResourceManager::s_resources;
decltype(ResourceManager::s_resourceMutex) ResourceManager::s_resourceMutex;
decltype(ResourceManager::s_createFuncs)   ResourceManager::s_createFuncs;

void ResourceManager::add(_In_ std::shared_ptr<Resource> resource)
{
	if (!resource->path())
	{
		cerr << __FUNCTION__": Cannot add a resource without a path.\n";
		return;
	}

	std::lock_guard lock(s_resourceMutex);

	if (const auto it = s_resources.find(*resource->path()); it != s_resources.end())
		if (const auto resourcePtr = it->second.lock())
		{
			if (resourcePtr == resource)
				return;

			cerr << __FUNCTION__": Resource \"" << *resource->path() << "\" already exists.\n";
			return;
		}

	s_resources[*resource->path()] = resource;
}

void ResourceManager::remove(const std::filesystem::path& path)
{
	std::lock_guard lock(s_resourceMutex);
	s_resources.erase(path);
}
