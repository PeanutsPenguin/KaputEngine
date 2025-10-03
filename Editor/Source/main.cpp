#include "Application.h"
#include "Editor/Editor.h"

#include "Queue/Context.h"
#include "Resource/Manager.hpp"
#include "Resource/Material.h"
#include "Resource/Mesh.h"

using namespace KaputEditor;

using namespace KaputEngine;
using namespace KaputEngine::Audio;
using namespace KaputEngine::Resource;
using namespace KaputEngine::Text;
using namespace KaputEngine::Text::Xml;
using namespace KaputEditor;

using Queue::ContextQueue;

int mainImpl(int argc, char** argv)
{
	Application::init("Kaput Editor", { 1280, 720 });

	const auto mat	 = ResourceManager::get<MaterialResource>("Kaput/Material/noTextureMat.kasset");
	const auto mesh1 = ResourceManager::get<MeshResource>("Kaput/Mesh/TranslateGizmo.kasset");
	const auto mesh2 = ResourceManager::get<MeshResource>("Kaput/Mesh/rotateGizmo.kasset");

	Editor* instance = Editor::getInstance();
	instance->init();

	while (!Application::getWindow().shouldClose())
	{
		ContextQueue::instance().popAll();

		Application::newUIFrame();
		//Should be in motor (like always update delate time on it's own)
		Application::update();

		if (Application::getInputManager().isKeyPressed(KEY_ESCAPE))
			if (instance->getState() == E_PLAYING)
				instance->getToolWindow().stopPlaying();
			else
			{
				Application::quit();
				break;
			}

		if (instance->getState() == E_PLAYING)
			Application::getWindow().update();

		Application::getWindow().renderBackgroundWindow();

		instance->getGameView().preRender();
		instance->getGameView().render();

		instance->getPickingHandler().preRender();
		instance->getPickingHandler().update();
		instance->getPickingHandler().render();

		instance->getDetailWindow().render();

		instance->getToolWindow().render();

		instance->getHierarchyWindow().render();

		instance->getFileExplorerWindow().renderFiles();

		//Render UI Windows
		Application::renderWindows();

		if (instance->m_closing)
			instance->getSaveWindow().render();

		instance->getGameView().postRender();

		Application::renderUIFrame();
		Application::getWindow().glUpdate();
	}

	Application::cleanup();
	Editor::getInstance()->destroy();

	return 0;
}

#define WIN32_LEAN_AND_MEAN /* No need for every windows header */
#include <Windows.h>
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <iostream>

int main(int argc, char** argv) {
#ifndef NDEBUG
	_CrtMemState start;
	_CrtMemCheckpoint(&start);

	int returnValue = mainImpl(argc, argv);

	_CrtMemState end;
	_CrtMemCheckpoint(&end);

	_CrtMemState difference;
	if (_CrtMemDifference(&difference, &start, &end)) {
		OutputDebugString(TEXT("---------- _CrtMemDumpStatistics ----------\n\n"));
		_CrtMemDumpStatistics(&difference);
		OutputDebugString(TEXT("\n---------- _CrtMemDumpAllObjectsSince ----------\n\n"));
		_CrtMemDumpAllObjectsSince(&end);
		OutputDebugString(TEXT("\n---------- _CrtMemDumpMemoryLeaks ----------\n\n"));
		_CrtDumpMemoryLeaks();

		if (!returnValue)
			return -1;
	}

	return returnValue;
#else
	return mainImpl(argc, argv);
#endif
}
