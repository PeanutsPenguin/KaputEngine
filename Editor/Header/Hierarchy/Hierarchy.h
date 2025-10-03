#pragma once

#include "Scene/Scene.h"
#include "Window/VirtualWindow.h"
#include "Text/String.h"

namespace KaputEditor
{
	class HierarchyWindow
	{
	public:
		HierarchyWindow();

		void setScene(_In_ std::shared_ptr<KaputEngine::Scene> scene);

		void setRenamingObject(KaputEngine::GameObject& obj);

		void render();

		~HierarchyWindow() = default;

	private:
		KaputEngine::VirtualWindow* m_window;
		std::shared_ptr<KaputEngine::Scene> m_scene;
		KaputEngine::Id m_renamingId;
		char m_newName[NAME_MAX_CHARACTERS];

		_NODISCARD std::shared_ptr<KaputEngine::Scene>& getScene() noexcept;

		_NODISCARD  const std::shared_ptr<KaputEngine::Scene>& getScene() const noexcept;
		
		_NODISCARD bool displayRoot();

		void displayEntity(KaputEngine::GameObject& obj);

		void renderSceneValues();
	
		void renderRightClick();

		_NODISCARD bool displayCanvas();

		_NODISCARD bool renderUIObject(std::shared_ptr<KaputEngine::UIObject> object);
	};
}
