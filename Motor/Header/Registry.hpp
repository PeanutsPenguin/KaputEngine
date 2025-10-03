#pragma once

#include "Registry.h"

#include "GameObject/Camera.h"
#include "GameObject/GameObject.hpp"
#include "Scene/Transform/Transform.h"

#include <LibMath/Vector/Vector.h>

#include <format>

namespace KaputEngine
{
	template <LibMath::IsLinearMathArray T>
	void Registry::arrayAlias(sol::usertype<T>& type, _In_ const char* name, LibMath::ArrIndex index)
	{
		type[name] = sol::property(
			[index](T& arr) { return arr.raw()[index]; },
			[index](T& arr, typename T::DataType value) { arr.raw()[index] = value; });
	}

	template <LibMath::IsLinearMathArray T>
	void Registry::registerArrayType(sol::state& lua, const std::string& typeName)
	{
		sol::usertype<T> type = lua.globals().new_usertype<T>(typeName, sol::constructors<T()>());

		arrayAlias(type, "x", 0);

		if constexpr (T::Size >= 2)
			arrayAlias(type, "y", 1);
		if constexpr (T::Size >= 3)
			arrayAlias(type, "z", 2);
		if constexpr (T::Size >= 4)
			arrayAlias(type, "w", 3);

		if constexpr (std::same_as<T, LibMath::Vector<T::Size, typename T::DataType>>)
		{
			type["magnitudeSquared"] = [](T& vec) -> float
			{
				return vec.magnitudeSquared();
			};

			type["normalize"] = [](T& vec) -> T
			{
				T result = vec.normalize();
				return result;
			};
		}
	}

	template <typename T, typename... Args>
	void Registry::registerType(Args&&... args)
	{
		if constexpr (LibMath::IsLinearMathArray<T>)
		{
			// For some reason, std::format crashes with an inline string
			auto suffix = LibMath::typeSuffix<typename T::DataType>();
			registerArrayType<T>(getAppState(), std::format("Vector{}{}", T::Size, suffix));
		}
		else if constexpr (std::same_as<T, Transform>)
			registerTransform();
		else
			T::template registerType(getAppState(), std::forward<Args>(args)...);
	}
}
