#pragma once

#include <concepts>

namespace sol
{
    class state;
}

namespace KaputEngine
{
    class Registry;
}

#define REGISTER_SIG(type) \
friend KaputEngine::Registry; \
template <std::derived_from<type> T = type> \
static void registerType(sol::state& state)

#define REGISTER_FINAL_SIG(type) \
friend KaputEngine::Registry; \
static void registerType(sol::state& state)

#define REGISTER_IMPL(type, body) \
template <std::derived_from<type> T> \
void type::registerType(sol::state& state) { body; }

#define REGISTER_FINAL_IMPL(type, body) \
void type::registerType(sol::state& state) { body; }
