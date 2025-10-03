#include "ToolsWindow/ToolsWindow.h"

#include "Application.h"
#include "Editor/Editor.h"

using namespace KaputEditor;
using namespace KaputEngine;

using std::cerr;
using std::cout;

ToolsWindow::ToolsWindow()
{
	this->m_window = Application::addUIWindow("Tools");
}

void ToolsWindow::render()
{
	this->m_window->beginWindow();

	this->renderPlayButton();

	this->m_window->onSameLine(0);

	this->renderPauseButton();

	this->m_window->onSameLine(0);

	this->renderSaveButton();

	this->m_window->onSameLine(0);

	this->renderTranslationButton();

	this->m_window->onSameLine(0);

	this->renderRotationButton();

	this->m_window->onSameLine(0);

	this->renderScalingButton();

	this->m_window->onSameLine(0);

	this->renderMoreButton();

	this->m_window->endWindow();
}

void ToolsWindow::stopPlaying()
{
	Editor* instance = Editor::getInstance();

	instance->setState(E_NONE);

	//if (!instance->loadScene("Game/Scene/newScene.kscene"))
	if (!instance->loadScene("Kaput/Scene/Editor.kscene"))
		cerr << "__FUNCTION__ : Failed to reload the scene\n";

	if (instance->setSelectedObject(instance->getSelectedObjectId(), *Application::getWindow().currentScene()))
		cerr << "Failed to re-select the same object as before\n";

	Application::getWindow().setCursorStatus(false);

	Application::audio().stopAll();
}

void ToolsWindow::renderPlayButton()
{
	Editor* instance = Editor::getInstance();
	std::string name = "Play";

	if (instance->getState() == E_PLAYING)
		name = "Stop";

	if (this->m_window->renderButton(name))
	{
		if (instance->getState() == E_PLAYING)
			this->stopPlaying();
		else
		{
			if (std::shared_ptr<Scene> scene = Application::getWindow().currentScene(); scene)
				scene->save("Kaput/Scene/Editor.kscene", true);

			instance->setState(E_PLAYING);

			if (this->m_focusOnGameWindow)
				instance->getGameView().setWindowFocus();

			Application::getWindow().setCursorStatus(this->m_hideCursor);
		}
	}
}

void ToolsWindow::renderPauseButton()
{
	Editor* instance = Editor::getInstance();

	if (instance->getState() != E_PLAYING)
		return;

	if (this->m_window->renderButton("Pause"))
	{
		if (instance->getState() == E_PAUSED)
			instance->setState(E_PLAYING);
		else
			instance->setState(E_PAUSED);
	}
}

void ToolsWindow::renderSaveButton()
{
	if (this->m_window->renderButton("Save"))
		Application::getWindow().currentScene()->save(Editor::getInstance()->getScenePath(), true);
}

void ToolsWindow::renderRotationButton()
{
	if (this->m_window->renderButton("Rotate##Tool"))
	{
		Editor::getInstance()->setState(E_ROTATING);
		Editor::getInstance()->getPickingHandler().switchToRotateMesh();
	}
}

void ToolsWindow::renderScalingButton()
{
	if (this->m_window->renderButton("Scale##Tool"))
	{
		Editor::getInstance()->setState(E_SCALING);
		Editor::getInstance()->getPickingHandler().switchToMoveMesh();
	}
}

void ToolsWindow::renderMoreButton()
{
	const char* name = "More";
	if (this->m_window->renderButton(name))
		this->m_window->InitPopUp(name);

	if (this->m_window->beginPopUp(name))
	{
		decltype(auto) _ = this->m_window->renderCheckBox("Hide Cursor On Play", this->m_hideCursor);
		_ = this->m_window->renderCheckBox("Focus on Game Window On Play", this->m_focusOnGameWindow);

		this->m_window->endPopUp();
	}
}

void ToolsWindow::renderTranslationButton()
{
	if (this->m_window->renderButton("Move##Tool"))
	{
		Editor::getInstance()->setState(E_MOVING);
		Editor::getInstance()->getPickingHandler().switchToMoveMesh();
	}
}
