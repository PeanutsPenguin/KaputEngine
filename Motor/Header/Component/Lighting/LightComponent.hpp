#pragma once

#include "LightComponent.h"

#include "../Component.hpp"

namespace KaputEngine
{
	template<std::derived_from<LightComponent> T>
	void LightComponent::defineLuaMembers(sol::usertype<T>& type)
	{
		Component::defineLuaMembers(type);

		type["index"] = sol::readonly(&LightComponent::m_index);

		type["enabled"] = sol::property(
		[](LightComponent& comp) { return static_cast<bool>(comp.getBaseData().enabled); },
		[](LightComponent& comp, bool enabled)
		{
			comp.getBaseData().enabled = enabled;
			comp.updateThisBuffer();
		});

		type["intensity"] = sol::property(
		[](LightComponent& comp) { return comp.getBaseData().intensity; },
		[](LightComponent& comp, float intensity)
		{
			comp.getBaseData().intensity = intensity;
			comp.updateThisBuffer();
		});

		type["color"] = sol::property(
		[](LightComponent& comp) { return comp.getBaseData().color; },
		[](LightComponent& comp, Rendering::Color color)
		{
			comp.getBaseData().color = color;
			comp.updateThisBuffer();
		});
	}
}
