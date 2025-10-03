#pragma once

#include "DetailWindow/DetailWindow.h"
#include "FileExplorer/FileExplorer.h"
#include "GameView/GameView.h"
#include "Hierarchy/Hierarchy.h"
#include "Id.h"
#include "PickingHandler/PickingHandler.h"
#include "SaveWindow/SaveWindow.h"
#include "ToolsWindow/ToolsWindow.h"

namespace KaputEditor
{
	constexpr int NO_ID = 0;

	enum EditorState: int
	{
		E_NONE,
		E_PLAYING,
		E_PAUSED,
		E_MOVING,
		E_SCALING,
		E_ROTATING
	};

	class Editor
	{
	public:
		Editor(const Editor&) = delete;
		Editor(Editor&&) = delete;

		void init();

		static _Ret_notnull_ _NODISCARD Editor* getInstance() noexcept;

		_NODISCARD bool setSelectedObject(const KaputEngine::Id& id, KaputEngine::Scene& scene);

		_NODISCARD bool setSelectedObject(std::shared_ptr<KaputEngine::ObjectBase> obj);

		_NODISCARD std::shared_ptr<KaputEngine::GameObject> idToGameObject(const KaputEngine::Id& id, KaputEngine::Scene& scene);

		_NODISCARD std::shared_ptr<KaputEngine::ObjectBase> getSelectedObjectPtr() noexcept;

		_NODISCARD const KaputEngine::Id& getSelectedObjectId() noexcept;

		_NODISCARD EditorState getState() const noexcept;

		_NODISCARD LibMath::Vector3f& getObjectAngles() noexcept;

		_NODISCARD GameView& getGameView() noexcept;

		_NODISCARD PicklingHandler& getPickingHandler() noexcept;

		_NODISCARD DetailWindow& getDetailWindow() noexcept;

		_NODISCARD HierarchyWindow& getHierarchyWindow() noexcept;

		_NODISCARD ToolsWindow& getToolWindow() noexcept;

		_NODISCARD FileExplorer& getFileExplorerWindow() noexcept;

		_NODISCARD SaveWindow& getSaveWindow() noexcept;

		_NODISCARD std::string& getScenePath() noexcept;

		void setState(EditorState state);

		_NODISCARD bool setSelectedObject(KaputEngine::Id& id, KaputEngine::Scene& scene);

		void setObjectAngles(const LibMath::Vector3f& angles);

		_NODISCARD bool loadScene(_In_ const char* path);

		_NODISCARD bool openScene(_In_ const char* path);

		void destroy();

		void unselectObject();

		bool m_closing;

	private:
		Editor() = default;
		static Editor* s_instance;

		GameView m_gameView;
		PicklingHandler m_picker;
		DetailWindow m_detailWindow;
		HierarchyWindow m_hierarchy;
		ToolsWindow m_tool;
		FileExplorer m_fileExplorer;
		SaveWindow m_saveWindow;

		std::pair<std::shared_ptr<KaputEngine::Scene>, std::string> m_currentScene;

		KaputEngine::Id m_selectedId;
		std::shared_ptr<KaputEngine::ObjectBase> m_object;
		//std::optional<std::shared_ptr<UIObject>> m_uiObject;
		LibMath::Vector3f m_objEulerAngles;
		EditorState m_state;

		std::pair<bool, std::shared_ptr<KaputEngine::GameObject>> getGameObjectFromId(
			const KaputEngine::Id& id, KaputEngine::GameObject& object);
	};
}
