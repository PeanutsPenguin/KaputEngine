#pragma once

#include <LibMath/MathArray/Concepts.h>

#include <sol.hpp>
#include <string>

namespace KaputEngine
{
	struct Registry
	{
		template <typename T, typename... Args>
		static void registerType(Args&&... args);

		static void registerDefaultTypes();

		static void registerLuaInput();

		_NODISCARD static sol::state& getAppState();

		template <LibMath::IsLinearMathArray T>
		static void arrayAlias(sol::usertype<T>& type, _In_ const char* name, LibMath::ArrIndex index);

		template <LibMath::IsLinearMathArray T>
		static void registerArrayType(sol::state& lua, const std::string& typeName);

		static void registerOperator();

		static void registerTransform();

        static void registerAngle();

		static void registerGlobals();
    };
}
