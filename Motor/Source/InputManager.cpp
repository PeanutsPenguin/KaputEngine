#include "InputManager.h"

#include "Application.h"

#include <ImGui/imgui.h>

using namespace KaputEngine;

using LibMath::Vector2d;

const Vector2d& InputManager::mousePosition() const noexcept
{
	return m_mousePosition;
}

Vector2d InputManager::mouseDelta() const noexcept
{
	return m_lastMousePosition - m_mousePosition;
}

void InputManager::updateMouse() noexcept
{
    m_lastMousePosition = m_mousePosition;
    ImVec2 pos = ImGui::GetMousePos();
    m_mousePosition.x() = pos.x;
    m_mousePosition.y() = pos.y;
}

bool InputManager::isKeyPressed(eKey key, _In_opt_ VirtualWindow* window)
{
    if (key == KEY_MOUSE_LEFT_BUTTON)
        return this->isLeftClickPressed(window);

    bool result;

    if (!window)
        result = ImGui::IsKeyPressed((ImGuiKey)key);
    else
    {
        window->beginWindow();
        if (ImGui::IsWindowFocused())
            result = ImGui::IsKeyPressed((ImGuiKey)key);
        else
            result = false;
        window->endWindow();
    }

    return result;
}

bool InputManager::isKeyDown(eKey key, _In_opt_ VirtualWindow* window)
{
    if (key == KEY_MOUSE_LEFT_BUTTON)
        return this->isLeftClickDown(window);

    bool result;

    if (window == nullptr)
        result = ImGui::IsKeyDown((ImGuiKey)key);
    else
    {
        window->beginWindow();
        if (ImGui::IsWindowFocused())
            result = ImGui::IsKeyDown((ImGuiKey)key);
        else
            result = false;
        window->endWindow();
    }

    return result;
}

bool InputManager::isKeyReleased(eKey key, _In_opt_ VirtualWindow* window)
{
    if (key == KEY_MOUSE_LEFT_BUTTON)
        return this->isLeftClickReleased(window);

    bool result;

    if (window == nullptr)
        result = ImGui::IsKeyReleased((ImGuiKey)key);
    else
    {
        window->beginWindow();
        if (ImGui::IsWindowFocused())
            result = ImGui::IsKeyReleased((ImGuiKey)key);
        else
            result = false;
        window->endWindow();
    }

    return result;
}

bool InputManager::isLeftClickPressed(_In_opt_ VirtualWindow* window)
{
    bool result;

    if (window == nullptr)
        result = ImGui::IsMouseClicked(0);
    else
    {
        window->beginWindow();
        if (ImGui::IsWindowFocused())
            result = ImGui::IsMouseClicked(0);
        else
            result = false;
        window->endWindow();
    }

    return result;
}

bool InputManager::isLeftClickReleased(_In_opt_ VirtualWindow* window)
{
    bool result;

    if (window == nullptr)
        result = ImGui::IsMouseReleased(0);
    else
    {
        window->beginWindow();
        if (ImGui::IsWindowFocused())
            result = ImGui::IsMouseReleased(0);
        else
            result = false;
        window->endWindow();
    }

    return result;
}

bool InputManager::isLeftClickDown(_In_opt_ VirtualWindow* window)
{
    bool result;

    if (window == nullptr)
        result = ImGui::IsMouseDown(0);
    else
    {
        window->beginWindow();
        if (ImGui::IsWindowFocused())
            result = ImGui::IsMouseDown(0);
        else
            result = false;
        window->endWindow();
    }

    return result;
}

bool InputManager::isMouseLeftDoubleClicked() const noexcept
{
    return ImGui::IsMouseDoubleClicked(0);;
}
