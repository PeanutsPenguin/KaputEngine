#include "Rendering/Color.h"

#include <sol.hpp>

using KaputEngine::Rendering::Color;

using LibMath::ArrIndex;

const Color
	Color::White  = Hex(0xFFFFFFFF),
	Color::Black  = Hex(0x000000FF),
	Color::Red    = Hex(0xFF0000FF),
	Color::Green  = Hex(0x00FF00FF),
	Color::Blue   = Hex(0x0000FFFF),
	Color::Yellow = Hex(0xFFFF00FF),
	Color::Transparent = Color();

Color Color::Hex(const uint32_t hex)
{
	Color out;
	const auto& bytes = reinterpret_cast<const uint8_t(&)[4]>(hex);

	for (ArrIndex i = 0; i < Size; i++)
		out.raw()[Size - i - 1] = bytes[i] / 255.f;

	return out;
}

REGISTER_FINAL_IMPL(Color,
sol::usertype<Color> type = state.globals().new_usertype<Color>("Color", sol::constructors<Color()>());

type["r"] = sol::property(
	[](Color& color) { return color.r(); },
	[](Color& color, float value) { color.r() = value; });

type["g"] = sol::property(
[](Color& color) { return color.g(); },
[](Color& color, float value) { color.g() = value; });

type["b"] = sol::property(
[](Color& color) { return color.b(); },
[](Color& color, float value) { color.b() = value; });

type["a"] = sol::property(
[](Color& color) { return color.a(); },
[](Color& color, float value) { color.a() = value; }))
