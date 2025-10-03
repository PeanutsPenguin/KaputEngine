#define SOL_LUAJIT 1
#define SOL_ALL_SAFETIES_ON 1

#include "Application.h"

#include "Queue/Context.h"
#include "Registry.h"

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

using namespace LibMath;
using namespace KaputEngine;

using KaputEngine::Audio::AudioEngine;
using KaputEngine::Queue::ContextQueue;
using KaputEngine::Rendering::Color;

decltype(Application::preUpdate)     Application::preUpdate  = nullptr;
decltype(Application::postUpdate)    Application::postUpdate = nullptr;
decltype(Application::preRender)     Application::preRender  = nullptr;
decltype(Application::postRender)    Application::postRender = nullptr;
decltype(Application::s_audioEngine) Application::s_audioEngine;
decltype(Application::s_lua)	     Application::s_lua;

bool Application::s_shouldQuit = false;
bool Application::s_paused = false;
PrimaryWindow Application::s_window;

double Application::s_lastFrame = glfwGetTime();
double Application::s_deltaTime = 0;

std::vector<VirtualWindow*> Application::s_UIWindows;
std::vector<std::function<void()>> Application::s_onClose;

InputManager Application::s_inputs;

bool Application::init(const char* title, const Vector2i& size)
{
	if (!s_window.init(title, size))
		return false;

	s_lua.open_libraries(sol::lib::base);

	Registry::registerDefaultTypes();
	Registry::registerLuaInput();
	Registry::registerAngle();
	Registry::registerOperator();
	Registry::registerGlobals();

	return true;
}

void Application::update()
{
	updateDeltaTime();
	s_inputs.updateMouse();
}

void Application::resizeViewport(const Vector2i& size)
{
	// TODO Use ContextQueue
	glViewport(0, 0, size.x(), size.y());
}

sol::state& Application::luaState() noexcept
{
	return s_lua;
}

AudioEngine& Application::audio() noexcept
{
    return s_audioEngine;
}

void Application::onClose(std::function<void()>&& func)
{
	s_onClose.emplace_back(std::move(func));
}

void Application::callCloseFunctions()
{
	for (std::function<void()>& func : s_onClose)
		func();
}

void Application::run()
{
	while (!s_shouldQuit && !s_window.shouldClose())
	{
		ContextQueue::instance().popAll();

		update();

		if (!s_paused)
		{
			if (preUpdate)
				preUpdate();

			s_window.update();

			if (postUpdate)
				postUpdate();
		}

		// Apply changes made during upates
		ContextQueue::instance().popAll();

		if (preRender)
			preRender();

			newUIFrame();               //Initialize a new UI frame (for the editor && the game)
			renderWindows();            //Render the virtual windows (for the editor)
			s_window.render();          //Render everuthing in the scene (for the game)
			renderUIFrame();            //Render the UI frame (for the editor && the game)

		if (postRender)
			postRender();

		s_window.glUpdate();
	}

	cleanup();
}

void Application::quit()
{
	s_shouldQuit = true;
	getWindow().setClose(true);
}

void Application::clearScreen(const Color& col)
{
	Vector2f size = s_window.getSize();

	glViewport(0,0 ,size.x(), size.y());
	glClearColor(col.r(), col.g(), col.b(), col.a());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

VirtualWindow* Application::addUIWindow(VirtualWindow& window)
{
	VirtualWindow* result = new VirtualWindow(window);
	s_UIWindows.push_back(result);

	return result;
}

VirtualWindow* Application::addUIWindow(const char* const name)
{
	VirtualWindow* result = new VirtualWindow(name);
	s_UIWindows.push_back(result);

	return result;
}

VirtualWindow* Application::addUIWindow(const char* const name, UIWindowFlags flags)
{
	VirtualWindow* result = new VirtualWindow(name, flags);
	s_UIWindows.push_back(result);

	return result;
}

double Application::getDeltaTime() noexcept
{
	return s_deltaTime;
}

InputManager& Application::getInputManager() noexcept
{
	return s_inputs;
}

void Application::setPauseBool()
{
	s_paused = !s_paused;
}

void Application::setWindowShouldClose(bool value)
{
	s_window.setClose(value);
}

void Application::updateDeltaTime()
{
	const double currentFrame = glfwGetTime();
	s_deltaTime = currentFrame - s_lastFrame;
	s_lastFrame = currentFrame;
}

void Application::cleanup()
{
	for (VirtualWindow* uiWindow : s_UIWindows)
		delete uiWindow;

	s_onClose.clear();
	s_window.destroy();
	//s_lua.collect_garbage();
}

void Application::renderWindows()
{
	for (VirtualWindow* window : s_UIWindows)
		window->render();
}

void Application::newUIFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//ImGui::ShowDemoWindow();
}

void Application::renderUIFrame()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO();

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

PrimaryWindow& Application::getWindow() noexcept
{
	return s_window;
}
