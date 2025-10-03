#pragma once

#include "ScriptComponent.h"

template <typename T>
void KaputEngine::ScriptComponent::defineSolProperty(
	_Out_ std::vector<KaputEngine::Inspector::Property>& out, const std::string& name, const T& value) noexcept
{
	out.emplace_back(name,
	[this, value]() -> T { return value; },
	[this, name](const T& value) { m_environment[name] = value; });
}
