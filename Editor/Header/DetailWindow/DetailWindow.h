#pragma once

#include <memory>
#include <sal.h>
#include <string>

#include "Text/String.h"

namespace KaputEngine
{
	class AudioComponent;
	class GameObject;
	class ObjectBase;
	class PhysicComponent;
	class RenderComponent;
	class Scene;
	class ScriptComponent;
	class Transform;
	class UIImage;
	class VirtualWindow;
}

namespace KaputEditor
{
	class DetailWindow
	{
	public:
		DetailWindow();

		void setScene(_In_ KaputEngine::Scene* scene);

		void render();

		void removeItemFocus();

		_NODISCARD bool isFocus();

		~DetailWindow() = default;

	private:
		_NODISCARD bool renderTransform(_In_opt_ KaputEngine::ObjectBase* source, KaputEngine::Transform& trans, const std::string& id, bool readOnly = false);

		void renderPhysicComponent(KaputEngine::PhysicComponent& pysic);

		void renderAudioComponent(class KaputEngine::AudioComponent& audio);

		void renderSwitchShape(KaputEngine::PhysicComponent& physic);

		void renderSwitchTag(KaputEngine::PhysicComponent& physic);

		void renderCreatingTag(KaputEngine::PhysicComponent& physic);

		void renderScale(KaputEngine::PhysicComponent& physic);

		void renderRenderComponent(class KaputEngine::RenderComponent& render);

		void changeRenderComponentMesh(const char* checker, class KaputEngine::RenderComponent& render);

		void changeRenderComponentMaterial(const char* checker, class KaputEngine::RenderComponent& render);

		void changeAudioComponentAsset(class KaputEngine::AudioComponent& audio);

		void changeUIImageTexture(std::shared_ptr<KaputEngine::UIImage>& img);

		void renderScriptComponent(KaputEngine::ScriptComponent& render);

		void changeScriptComponent(KaputEngine::ScriptComponent& render);

		void renderAddComponentButton(KaputEngine::GameObject& obj, bool physic);

		void renderProperties(KaputEngine::ObjectBase& obj);

		void renderObject(KaputEngine::ObjectBase& obj);

		KaputEngine::VirtualWindow* m_window;
		KaputEngine::Scene* m_scene;
		bool m_creatingTag;
		char m_newTagName[NAME_MAX_CHARACTERS];
	};
}
