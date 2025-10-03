#pragma once

#include "Hierarchy/Hierarchy.h"
#include "SceneCamera/SceneCamera.h"
#include "Window/VirtualWindow.h"
#include "Window/UIObject.h"

#include <filesystem>

namespace KaputEditor
{
	enum CreationType
	{
		E_SCENE,
		E_SCRIPT,
		E_FOLDER
	};

	class FileExplorer
	{
	public:
		FileExplorer();

		void renderFiles();

		~FileExplorer() = default;

	private:
		KaputEngine::VirtualWindow* m_window;
		std::shared_ptr<KaputEngine::UIText> m_pathText;

		std::filesystem::path m_currentPath;
		std::filesystem::path m_selectedEntry;

		std::filesystem::path m_prefixPath;
		std::string m_relativePath;

		std::pair<CreationType, bool> m_creating;
		char m_newName[NAME_MAX_CHARACTERS];

		void addToSceneCallBack(std::shared_ptr<SceneCamera> cam);

		void importButton();

		void importCallBack();

		void goUpButton();

		void goUpCallBack();

		void rightClickMenu();

		bool openFileDialog(std::wstring& path);

		void importMesh(const std::filesystem::path& source);

		void importTexture(const std::filesystem::path& source);

		void importSound(const std::filesystem::path& source);

		void renderCreation();

		void sceneCreation();

		void folderCreation();

		void scriptCreation();

		void directoryDragAndDrop(std::filesystem::path& dirPath);

		void fileDragAndDrop(std::filesystem::path& dirPath);

		void onDirectoryClick(const std::filesystem::path& entry);

		void onFileClick(const std::filesystem::path& entry);

		_NODISCARD std::filesystem::path toRelativePath(std::filesystem::path orgin);
	};
}
