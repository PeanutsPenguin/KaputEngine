#include "Registry.hpp"

#include "Application.h"

#include "Component/Audio/AudioComponent.h"
#include "Component/Audio/AudioListenerComponent.h"
#include "Component/Component.hpp"
#include "Component/Lighting/DirectionalLightComponent.h"
#include "Component/Lighting/LightComponent.hpp"
#include "Component/Lighting/PointLightComponent.h"
#include "Component/PhysicComponent.h"
#include "Component/RenderComponent.h"
#include "Component/ScriptComponent.h"
#include "GameObject/Camera.h"
#include "GameObject/GameObject.hpp"
#include "Rendering/Color.h"
#include "Resource/Material.h"
#include "Resource/Mesh.h"
#include "Resource/Resource.h"
#include "Resource/Script.h"
#include "Resource/Shader.h"
#include "Resource/ShaderProgram.h"
#include "Resource/Sound.h"
#include "Resource/Texture.h"
#include "Window/UIObject.hpp"

#include <LibMath/Angle/Degree.h>
#include <sol.hpp>

using namespace KaputEngine;
using namespace KaputEngine::Resource;
using namespace LibMath;

sol::state& Registry::getAppState()
{
	return Application::luaState();
}

void Registry::registerDefaultTypes()
{
	registerType<Rendering::Color>();

	registerType<Cartesian2f>();
	registerType<Cartesian3f>();
	registerType<Vector2f>();
	registerType<Vector3f>();
	registerType<Rotorf>();

	registerType<Transform>();

	registerType<ObjectBase>();
	registerType<GameObject>();

	registerType<Component>();
	registerType<AudioComponent>();
	registerType<AudioListenerComponent>();
	registerType<LightComponent>();
	registerType<DirectionalLightComponent>();
	registerType<PointLightComponent>();
	registerType<PhysicComponent>();
	registerType<RenderComponent>();
	registerType<ScriptComponent>();

	registerType<Camera>();

	registerType<MaterialResource>();
	registerType<MeshResource>();
	registerType<ScriptResource>();
	registerType<ShaderResource>();
	registerType<ShaderProgramResource>();
	registerType<SoundResource>();
	registerType<TextureResource>();

	registerType<UIObject>();
	registerType<UIText>();
	registerType<UISlider>();
	registerType<UIButton>();
	registerType<UIImage>();
}

void Registry::registerOperator()
{
	sol::state& lua = getAppState();

	lua.globals().set_function("multiply",
	sol::overload(

		[](Vector3f vec, float value) -> Vector3f
		{
			return vec * value;
		},
		[](Vector3f vec, int value) -> Vector3f
		{
			return vec * value;
		},
		[](Vector3f vec, Vector3f vec2) -> Vector3f
		{
			return vec * vec2;
		}
	));

	lua.globals().set_function("add",
	sol::overload(
		[](Vector3f vec, Vector3f vec2) -> Vector3f
		{
			return vec + vec2;
		}
	));

	lua.globals().set_function("subtract",
	sol::overload(
		[](Vector3f vec, Vector3f vec2) -> Vector3f
		{
			return vec - vec2;
		}
	));
}

void Registry::registerLuaInput()
{
	sol::state& lua = getAppState();

	// TODO Use loop
	lua.new_enum("Key",
		"Zero", eKey::KEY_0,
		"One", eKey::KEY_1,
		"Two", eKey::KEY_2,
		"Three", eKey::KEY_3,
		"Four", eKey::KEY_4,
		"Five", eKey::KEY_5,
		"Six", eKey::KEY_6,
		"Seven", eKey::KEY_7,
		"Eight", eKey::KEY_8,
		"Nine", eKey::KEY_9,
		"A", eKey::KEY_A,
		"B", eKey::KEY_B,
		"C", eKey::KEY_C,
		"D", eKey::KEY_D,
		"E", eKey::KEY_E,
		"F", eKey::KEY_F,
		"G", eKey::KEY_G,
		"H", eKey::KEY_H,
		"I", eKey::KEY_I,
		"J", eKey::KEY_J,
		"K", eKey::KEY_K,
		"L", eKey::KEY_L,
		"M", eKey::KEY_M,
		"N", eKey::KEY_N,
		"O", eKey::KEY_O,
		"P", eKey::KEY_P,
		"Q", eKey::KEY_Q,
		"R", eKey::KEY_R,
		"S", eKey::KEY_S,
		"T", eKey::KEY_T,
		"U", eKey::KEY_U,
		"V", eKey::KEY_V,
		"W", eKey::KEY_W,
		"X", eKey::KEY_X,
		"Y", eKey::KEY_Y,
		"Z", eKey::KEY_Z,
		"ESCAPE", eKey::KEY_ESCAPE,
		"MouseLeft", eKey::KEY_MOUSE_LEFT_BUTTON,
		"MouseRight", eKey::KEY_MOUSE_RIGHT_BUTTON,
		"Space", eKey::KEY_SPACE
		);

	//I know im supposed to bind the function directly but for ome reason it doesn't work
	lua.globals().set_function("isKeyPressed", [](eKey key) -> bool
	{
		return Application::getInputManager().isKeyPressed(key);
	});

	lua.globals().set_function("isKeyDown", [](eKey key) -> bool
	{
		return Application::getInputManager().isKeyDown(key);
	});

	lua.globals().set_function("isKeyReleased", [](eKey key) -> bool
	{
		return Application::getInputManager().isKeyReleased(key);
	});

	lua.globals().set_function("getMouseDelta", []() -> Vector2f
	{
		return (Vector2f)Application::getInputManager().mouseDelta();
	});

	lua.globals().set_function("getMousePosition", []() -> Vector2f
	{
		return (Vector2f)Application::getInputManager().mousePosition();
	});
}

void Registry::registerTransform()
{
	sol::state& lua = getAppState();
	using KTransform = KaputEngine::Transform;

	lua.globals().new_usertype<KTransform>("Transform",
		sol::constructors<KTransform()>(),
		"position", sol::readonly(&KTransform::position),
		"rotation", sol::readonly(&KTransform::rotation),
		"scale", sol::readonly(&KTransform::scale));
}

void Registry::registerAngle()
{
	sol::state& lua = getAppState();

	sol::usertype<Degree> degree = lua.globals().new_usertype<Degree>("Degree", sol::constructors<Degree(), Degree(float)>());

	degree["cos"] = [](Degree& deg) -> float
	{
		return LibMath::cos(Radian(deg.radian()));
	};

	degree["sin"] = [](Degree& deg) -> float
	{
		return LibMath::sin(Radian(deg.radian()));
	};

	sol::usertype<Radian> rad = lua.globals().new_usertype<Radian>("Radian", sol::constructors<Radian(), Radian(float)>());

	rad["cos"] = [](Radian& rad)
	{
		return LibMath::cos(rad);
	};

	rad["sin"] = [](Radian& rad)
	{
		return LibMath::sin(rad);
	};
}

void Registry::registerGlobals()
{
	sol::state& lua = getAppState();

	lua.globals()["deltaTime"] = sol::readonly_property(&Application::getDeltaTime);

	lua.globals()["getUIObject"] = [](const std::string& name) -> std::shared_ptr<UIObject>
	{
		return Application::getWindow().currentScene()->getUIObjectByName(name);
	};

	lua.globals()["loadScene"] = [](const std::string& scenePath)
	{
		std::shared_ptr<Scene> newScene = std::make_shared<Scene>();

		if (newScene->load(scenePath))
		{
			Application::getWindow().currentScene() = newScene;
			Application::audio().stopAll();
		}
	};

	lua.globals()["toString"] = [](float value) -> std::string
	{
		return std::to_string(value);
	};

	lua.globals()["setCursorHidden"] = [](bool value)
	{
		Application::getWindow().setCursorStatus(value);
	};
}
