#pragma once

#include "VirtualWindow.h"

#include <imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace KaputEngine
{
	template <typename T>
	const T* VirtualWindow::getDragNDropValue(const std::string& name)
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(name.c_str()); payload)
			return static_cast<T*>(payload->Data);

		return nullptr;
	}
}
