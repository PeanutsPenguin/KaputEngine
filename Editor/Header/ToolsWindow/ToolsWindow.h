#pragma once

#include "Window/VirtualWindow.h"

namespace KaputEditor
{
	class ToolsWindow
	{
	public:
		ToolsWindow();

		void render();

		void stopPlaying();

		~ToolsWindow() = default;
	private:
		KaputEngine::VirtualWindow* m_window;
		bool m_hideCursor = false;
		bool m_focusOnGameWindow = false;

		void renderPlayButton();

		void renderPauseButton();

		void renderSaveButton();

		void renderRotationButton();

		void renderTranslationButton();

		void renderScalingButton();

		void renderMoreButton();
	};
}
