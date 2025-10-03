#pragma once

#include "Window/VirtualWindow.h"

namespace KaputEditor
{
	class SaveWindow
	{
	public:
		SaveWindow();

		void render();

		void destroy();

		~SaveWindow() = default;

	private:
		std::unique_ptr<KaputEngine::VirtualWindow> m_window;
	};
}
