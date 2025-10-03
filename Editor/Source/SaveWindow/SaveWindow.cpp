#include "SaveWindow/SaveWindow.h"

#include "Application.h"
#include "Editor/Editor.h"

using namespace KaputEditor;
using namespace KaputEngine;

SaveWindow::SaveWindow()
{
	UIWindowFlags flags = static_cast<UIWindowFlags>(E_NO_RESIZE | E_NO_MOVE | E_NO_COLLAPSE);

	this->m_window = std::make_unique<VirtualWindow>("CAREFUL", flags);
}

void SaveWindow::render()
{
	if (Editor::getInstance()->getScenePath() == "")
	{
		Application::setWindowShouldClose(true);
		return;
	}

	this->m_window->beginWindow();

	this->m_window->setWindowFocusInFrame();
	this->m_window->setWindowSizeInFrame({ 300, 100 });
	this->m_window->setWindowPositionInFrame(Application::getWindow().getPosition() + (Application::getWindow().getSize()/2));

	this->m_window->renderText("Do you wanna save before quit ?");

	if (this->m_window->renderButton("Save", {10, 70 }))
	{
		Application::getWindow().currentScene()->save(Editor::getInstance()->getScenePath(), true);
		Application::setWindowShouldClose(true);
	}

	this->m_window->onSameLine(0);

	if (this->m_window->renderButton("Close"))
		Application::setWindowShouldClose(true);

	this->m_window->onSameLine(0);

	if (this->m_window->renderButton("Cancel close"))
		Editor::getInstance()->m_closing = false;


	this->m_window->endWindow();
}

void SaveWindow::destroy()
{
	this->m_window.release();
}
