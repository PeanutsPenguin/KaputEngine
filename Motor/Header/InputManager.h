#pragma once

#include "Utils/Input.h"
#include "Window/VirtualWindow.h"

#include <LibMath/Vector/Vector2.h>

struct GLFWwindow;

static constexpr int GLFW_OFFSET = 48;

namespace KaputEngine
{
	class InputManager
	{
	public:
		InputManager() = default;

		InputManager(InputManager&) = delete;
		InputManager(InputManager&&) = delete;

		_NODISCARD bool isKeyPressed(eKey key, _In_opt_ VirtualWindow* window = nullptr);

		_NODISCARD bool isKeyDown(eKey key, _In_opt_ VirtualWindow* window = nullptr);

		_NODISCARD bool isKeyReleased(eKey key, _In_opt_ VirtualWindow* window = nullptr);

		_NODISCARD bool isLeftClickPressed(_In_opt_ VirtualWindow* window = nullptr);

		_NODISCARD bool isLeftClickReleased(_In_opt_ VirtualWindow* window = nullptr);

		_NODISCARD bool isLeftClickDown(_In_opt_ VirtualWindow* window = nullptr);

		_NODISCARD bool isMouseLeftDoubleClicked() const noexcept;

		_NODISCARD const LibMath::Vector2d& mousePosition() const noexcept;
		_NODISCARD LibMath::Vector2d mouseDelta() const noexcept;

		~InputManager() = default;

	private:
		friend class Application;

		void updateMouse() noexcept;

		LibMath::Vector2d
			m_mousePosition,
			m_lastMousePosition;
	};
}

