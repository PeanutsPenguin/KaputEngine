#include "GameView/GameView.h"
#include "Editor/Editor.h"
#include "Rendering/Color.h"
#include "Window/UIObject.h"

using namespace KaputEditor;
using namespace KaputEngine;

using KaputEngine::Scene;
using KaputEngine::Rendering::Color;

using LibMath::Vector2f;

GameView::GameView() : SceneView("GameView") { }

void GameView::postRender()
{
	this->m_window->beginWindow();

	for (std::shared_ptr<UIObject> widget : this->getScene()->getHUD())
	{
		if (widget->getAligned())
			ImGui::SameLine();
		widget->setScale(this->m_window->getSize());

		if (widget->getParentWindow() != this->m_window)
			widget->setParentWindow(this->m_window);

		widget->render();
	}

	if (Editor::getInstance()->getState() == E_PLAYING)
	{
		this->m_window->endWindow();
		return;
	}

	Vector2f pos = (this->m_window->getSize() /2);
	pos.x() -= 100;

	if (std::shared_ptr<Scene> scene = getScene(); scene)
	{
		if (scene->getPrimaryCamera().expired())
			this->m_window->renderText("No MainCamera in Scene", pos, Color::White);
	}
	else
		this->m_window->renderText("No opened Scene", pos, Color::White);

	this->m_window->endWindow();
}

void GameView::setWindowFocus()
{
	this->m_window->setWindowFocusInFrame();
}
