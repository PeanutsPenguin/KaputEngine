#include "FileExplorer/FileExplorer.h"

#include "Application.h"
#include "Editor/Editor.h"
#include "Resource/Manager.hpp"
#include "Resource/Resource.h"
#include "Text/Xml/Context.hpp"
#include "Window/UIObject.h"
#include "Window/WindowConfig.h"

#include <fstream>
#include <shobjidl.h>
#include <string>
#include <windows.h>

using namespace KaputEngine;
using namespace KaputEngine::Resource;
using namespace KaputEngine::Text::Xml;

using KaputEditor::FileExplorer;

using KaputEngine::Rendering::Color;
using KaputRes = KaputEngine::Resource::Resource;

using std::cerr;
using std::cout;
using std::filesystem::current_path;
using std::filesystem::path;
using std::string;
using std::wstring;
using std::filesystem::copy_options;
using std::ofstream;

FileExplorer::FileExplorer()
{
	this->m_window = Application::addUIWindow("File Explorer", UIWindowFlags::E_NO_SCROLL_BAR);

	this->m_currentPath   = current_path();
	this->m_selectedEntry = current_path();
	this->m_prefixPath = current_path();

	this->goUpButton();
	this->importButton();

	this->m_pathText = this->m_window->addText({ 105, 0 }, this->m_relativePath, Color::White, true, false);

	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	if (FAILED(hr))
		cerr << __FUNCTION__": Failed to initialize file Explorer";
}

void FileExplorer::renderFiles()
{
	this->m_window->beginWindow();
	int offset = 0;

	for (auto &entry : std::filesystem::directory_iterator(this->m_currentPath))
	{
		const bool is_selected = (entry.path() == this->m_selectedEntry);
		const bool is_directory = entry.is_directory();
		const bool is_file = entry.is_regular_file();
		string entry_name = entry.path().filename().string();

		if (is_directory)
			entry_name = "[D] - " + entry_name;
		else if (is_file)
		{
			if (entry.path().extension() != ".kasset" && entry.path().extension() != ".kscene")
				continue;

			entry_name = "[F] - " + entry_name;
		}

		if (this->m_window->selectable(entry_name.c_str(), is_selected, { 20, 65.f + offset }))
		{
			if (is_directory)
				this->onDirectoryClick(entry.path());
			else if (is_file)
				this->onFileClick(entry.path());
		}

		if (this->m_window->startRightClickItemMenu())
		{
			if (this->m_window->addMenuItem("Show in File Explorer"))
			{
				string command = "explorer /select,\"" +
					this->m_currentPath.string() + '\\' + entry.path().filename().string() + '"';
				system(command.c_str());
			}

			this->m_window->endPopup();
		}

		//This is supposed to enable move file but we don't support it right now 
		path pathCpy = entry.path();

		if (is_file)
			this->fileDragAndDrop(pathCpy);
		//else if (is_directory)
		//	this->directoryDragAndDrop(pathCpy);

		offset += 20;
	}

	this->rightClickMenu();

	if (this->m_creating.second)
		this->renderCreation();

	this->m_window->endWindow();
}

void FileExplorer::directoryDragAndDrop(path& dirPath)
{
	if (this->m_window->beginDragAndDropTarget())
	{
		const char* asset = this->m_window->getDragNDropValue<const char>("Kasset");

		if (!asset)
		{
			this->m_window->endDragAndDropTarget();
			return;
		}

		path rootPath = dirPath;

		while (rootPath.has_parent_path() && rootPath.filename().compare("Assets"))
			rootPath = rootPath.parent_path();

		string source = rootPath.string() + "\\" + asset;

		path assetPath = asset;
		string cpyEmplacement = dirPath.string() + "\\" + assetPath.filename().string();

		copy_file(source, cpyEmplacement, copy_options::overwrite_existing);

		path newRelativePath = this->toRelativePath(cpyEmplacement);
		std::shared_ptr<KaputRes> original = ResourceManager::get<KaputRes>(asset);
		//if(original)
		//	original->changePath(newRelativePath);

		//Namespace because conflict
		std::filesystem::remove(source);

		this->m_window->endDragAndDropTarget();
	}
}

void FileExplorer::fileDragAndDrop(path& entry)
{
	string fullPath = this->m_relativePath + entry.filename().string();

	const char* value = fullPath.c_str();
	size_t length = fullPath.size() + 1;

	if (this->m_window->beginDragAndDropSource())
	{
		this->m_window->setDragNDropValue("Kasset", value, length * sizeof(char));
		this->m_window->renderText(fullPath);
		this->m_window->endDragAndDropSource();
	}
}

void FileExplorer::onDirectoryClick(const path& entry)
{
	this->m_currentPath /= entry.filename();
	this->m_relativePath += entry.filename().string() + "/";

	string pathText = "Current Directory : " + this->m_relativePath;
	this->m_pathText->setText(pathText);
}

void FileExplorer::onFileClick(const path& entry)
{
	if (entry.extension() == ".kscene")
	{
		string path = this->m_relativePath + entry.filename().string();

		if (!Editor::getInstance()->openScene(path.c_str()))
			cerr << __FUNCTION__": Failed to open scene at : " << path;
		
		this->m_selectedEntry = entry;
	}
}

path FileExplorer::toRelativePath(path origin)
{
	path result = origin.filename();

	while (origin.has_parent_path() && origin.parent_path().filename().compare("Assets"))
	{
		origin = origin.parent_path();
		result = origin.filename() / result;
	}

	return result;
}

void FileExplorer::importButton()
{
	this->m_window->addButton("Import",{115,25},{60,30},[this]()
	{
		this->importCallBack();
	},true,true);
}

void FileExplorer::importCallBack()
{
	wstring _path;
	if (this->openFileDialog(_path))
	{
		path source = _path;
		string ext = source.extension().string();

		if (ext == ".obj" || ext == ".fbx")
			this->importMesh(source);
		else if (ext == ".jpg" || ext == ".png" || ext == ".tga")
			this->importTexture(source);
		else if (ext == ".ogg" || ext == ".mp3" || ext == ".wav" || ext == ".flacc")
			this->importSound(source);
		else
			cerr << __FUNCTION__": File : " << source << " not supported" << ".\n";
	}
	else
		cerr << __FUNCTION__": Failed to import file";
}

void FileExplorer::goUpButton()
{
	this->m_window->addButton("Up", { 0, 0 }, { 30, 30 }, [this]()
	{
		this->goUpCallBack();
	},false,true);
}

void FileExplorer::goUpCallBack()
{
	if (this->m_currentPath.has_parent_path() && this->m_currentPath.filename().compare("Assets"))
	{
		int sizeParent = this->m_currentPath.filename().string().size() + 1;
		int sizePath   = this->m_relativePath.size();
		size_t index   = sizePath - sizeParent;

		this->m_relativePath = this->m_relativePath.substr(0,  index);

		this->m_currentPath = this->m_currentPath.parent_path();
		string pathText = "Current Directory : " + this->m_relativePath;
		this->m_pathText->setText(pathText);
	}
}

void FileExplorer::rightClickMenu()
{
	const char* createButton = "Create";

	if (Application::getInputManager().isKeyPressed(KEY_MOUSE_RIGHT_BUTTON, this->m_window) && this->m_window->isMouseInWindow())
		this->m_window->InitPopUp(createButton);

	if (this->m_window->beginPopUp(createButton))
	{
		if (this->m_window->selectable("Create Scene"))
		{
			this->m_creating.first = E_SCENE;
			this->m_creating.second = true;
			strcpy(this->m_newName, "newScene");
		}

		if (this->m_window->selectable("Create Script"))
		{
			this->m_creating.first = E_SCRIPT;
			this->m_creating.second = true;
			strcpy(this->m_newName, "newScript");
		}

		if (this->m_window->selectable("Create Folder"))
		{
			this->m_creating.first = E_FOLDER;
			this->m_creating.second = true;
			strcpy(this->m_newName, "newFolder");
		}

		if (this->m_window->selectable("Show in File Explorer"))
		{
			string command = "explorer " + this->m_currentPath.string();
			system(command.c_str());
		}

		this->m_window->endPopUp();
	}
}

bool FileExplorer::openFileDialog(wstring& pathGetter)
{
	IFileOpenDialog *p_file_open = nullptr;
	bool result = false;

	while (true)
	{
		HRESULT hr = CoCreateInstance(	CLSID_FileOpenDialog,			//File Type
										nullptr,							//not part of an aggregate
										CLSCTX_ALL,						//Context for object
										IID_IFileOpenDialog,			//Identifier
										reinterpret_cast<void **>(&p_file_open));		//object
		if (FAILED(hr))
		{
			cerr << __FUNCTION__": Failed to create instance for windows explorer.";
			break;
		}

		hr = p_file_open->Show(nullptr);
		if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED)) // No items were selected.
			break;
		else if (FAILED(hr))
		{
			cerr << __FUNCTION__": Windows file explorer failed";
			break;
		}

		IShellItem *p_item;
		hr = p_file_open->GetResult(&p_item);

		if (FAILED(hr))
		{
			cerr << __FUNCTION__": Windows file explorer failed.";
			break;
		}

		if (SUCCEEDED(hr))
		{
			PWSTR path;
			hr = p_item->GetDisplayName(SIGDN_FILESYSPATH, &path);
			if (SUCCEEDED(hr))
			{
				pathGetter = path;
				CoTaskMemFree(path);
			}
			p_item->Release();
			result = true;
			break;
		}
	}

	if (p_file_open)
		p_file_open->Release();

	return result;
}

void FileExplorer::importMesh(const path& source)
{
	path copyEmplacement = this->m_prefixPath / this->m_relativePath;
	string relativeLocation = this->m_relativePath;

	string copyEmplacementParentFolder = copyEmplacement.string();

	copyEmplacement /= source.filename();

	std::filesystem::copy_file(source,copyEmplacement, copy_options::overwrite_existing);

	string name = source.stem().string(); //xxx

	string fullname = name + ".kasset"; //xxx.kasset
	string fullKassetPath = copyEmplacementParentFolder + fullname;

	relativeLocation += source.filename().string();

	ofstream outfile (fullKassetPath.c_str());
	XmlSerializeContext context(outfile, true);

	context.startObject("Mesh");
	context.value("Source", relativeLocation);
	context.endObject();

	outfile.close();
}

void FileExplorer::importTexture(const path& source)
{
	path copyEmplacement = this->m_prefixPath / this->m_relativePath;
	string relativeLocation = this->m_relativePath;

	string copyEmplacementParentFolder = copyEmplacement.string();

	copyEmplacement /= source.filename();

	copy_file(source, copyEmplacement, copy_options::overwrite_existing);

	string name = source.stem().string(); //xxx
	string fullname = name + ".kasset"; //xxx.kasset
	string fullKassetPath = copyEmplacementParentFolder + fullname;

	relativeLocation += source.filename().string();

	ofstream outfile (fullKassetPath.c_str());
	XmlSerializeContext context(outfile, true);

	context.startObject("Texture");
	context.value("Source", relativeLocation);
	context.endObject();

	outfile.close();
}

void FileExplorer::importSound(const path& source)
{
	path copyEmplacement = this->m_prefixPath / this->m_relativePath;
	string relativeLocation = this->m_relativePath;

	string copyEmplacementParentFolder = copyEmplacement.string();

	copyEmplacement /= source.filename();

	std::filesystem::copy_file(source, copyEmplacement, copy_options::overwrite_existing);

	string name = source.stem().string(); //xxx
	string fullname = name + ".kasset"; //xxx.kasset
	string fullKassetPath = copyEmplacementParentFolder + fullname;

	relativeLocation += source.filename().string();

	ofstream outfile (fullKassetPath.c_str());
	XmlSerializeContext context(outfile, true);

	context.startObject("Sound");
	context.value("Source", relativeLocation);
	context.endObject();

	outfile.close();
}

void FileExplorer::renderCreation()
{
	switch (this->m_creating.first)
	{
	case E_SCENE:
		this->sceneCreation();
		break;
	case E_SCRIPT:
		this->scriptCreation();
		break;
	case E_FOLDER:
		this->folderCreation();
		break;
	default:
		break;
	}

}

void FileExplorer::sceneCreation()
{
	if (this->m_window->renderInputText("##Creation", this->m_newName, NAME_MAX_CHARACTERS))
	{
		string userInput = this->m_newName;

		if (userInput == "")
		{
			this->m_creating.second = false;
			return;
		}

		string fullname = this->m_relativePath + userInput + ".kscene";

		Scene emptyScene;

		ofstream outfile(fullname.c_str());
		outfile.close();

		if (!emptyScene.save(fullname, true))
			cerr << "__FUNCTION__ : Failed to create the Scene \n";

		this->m_creating.second = false;
	}
}

void FileExplorer::folderCreation()
{
	if (this->m_window->renderInputText("##Creation", this->m_newName, NAME_MAX_CHARACTERS))
	{
		string userInput = this->m_newName;

		if (userInput == "")
		{
			this->m_creating.second = false;
			return;
		}

		if (!std::filesystem::create_directories(this->m_relativePath + userInput))
			cerr << "__FUNCTION__ : Failed to create the new folder \n";

		this->m_creating.second = false;
	}
}

void FileExplorer::scriptCreation()
{
	if (this->m_window->renderInputText("##Creation", this->m_newName, NAME_MAX_CHARACTERS))
	{
		string userInput = this->m_newName;

		if (userInput == "")
		{
			this->m_creating.second = false;
			return;
		}

		string halfName = this->m_relativePath + userInput;

		string luaName = halfName + ".lua";

		ofstream luaFile(luaName.c_str());
		luaFile.close();

		string kSceneName = halfName + ".kasset";

		ofstream kSceneFile(kSceneName.c_str());

		XmlSerializeContext context(kSceneFile, true);
		context.startObject("Script");
		context.value("Source", luaName);
		context.endObject();

		kSceneFile.close();

		this->m_creating.second = false;
	}
}
