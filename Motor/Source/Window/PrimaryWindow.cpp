#include "Window/PrimaryWindow.h"

#include "Application.h"
#include "GameObject/Camera.h"
#include "Scene/Scene.h"

#include <LibMath/MathArray/Utilities.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ImGUI/imgui.h>
#include <ImGUI/imgui_impl_glfw.h>
#include <ImGui/imgui_impl_opengl3.h>

using namespace KaputEngine;

using std::cerr;

using LibMath::Vector2i;

GLFWwindow* PrimaryWindow::getHandle() noexcept
{
    return this->m_windowHandle;
}

bool PrimaryWindow::init(const char* title, const Vector2i& size)
{
    m_title = title;
    m_size = size;

    ///Window Initiation
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    ///Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(size.x(), size.y(), this->m_title, nullptr, nullptr);

    this->m_windowHandle = window;

    if (!window)
    {
        cerr << "Failed to create GLFW window\n";
        glfwTerminate();

        return false;
    }

    ///Make the window's context current
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetWindowCloseCallback(window, window_close_callBack);

    if (!gladLoadGL())
    {
        cerr << "Failed to initialize GLAD\n";
        return false;
    }

    glViewport(0, 0, size.x(), size.y());
    glEnable(GL_DEPTH_TEST);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoDocking;

    this->m_backgroundWindow = new VirtualWindow("BackroundWindow", static_cast<UIWindowFlags>(flags));

    if (!this->initImGui())
    {
        cerr << "Failed to initialize ImGui\n";
        return false;
    }
    return true;
}

bool PrimaryWindow::initImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   //Keyboard flag
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    //Mouse Flag
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     //Multiple viewports flag
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;       //Enable docking for the windows

    ImGui::StyleColorsDark();

    ImGuiStyle& imgui_style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        imgui_style.WindowRounding = 0.0f;
        imgui_style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(this->m_windowHandle, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.

    return ImGui_ImplOpenGL3_Init();
}

const Vector2i& PrimaryWindow::getSize() const noexcept
{
    return m_size;
}

VirtualWindow* PrimaryWindow::getBackgroundWindow() noexcept
{
    return this->m_backgroundWindow;
}

void PrimaryWindow::setScale(const Vector2i& size) noexcept
{
    m_size = size;

    glViewport(0, 0, size.x(), size.y());

    if (this->m_scene != nullptr)
    {
        LibMath::Vector2f sizeF = LibMath::array_convert<float>(size);
        float apect = sizeF.x() / sizeF.y();

        std::weak_ptr<Camera> cam = this->m_scene->getPrimaryCamera();

        if (!cam.expired())
            cam.lock()->setAspect(apect);
    }
}

void PrimaryWindow::framebuffer_size_callback(GLFWwindow*, int width, int height)
{
    Application::getWindow().setScale({ width, height });
}

void PrimaryWindow::window_close_callBack(GLFWwindow* window)
{
    Application::callCloseFunctions();
}

bool PrimaryWindow::shouldClose() const noexcept
{
    return glfwWindowShouldClose(m_windowHandle) || m_shouldClose;
}

void PrimaryWindow::update()
{
    Application::audio().updateTracking();

    if (m_scene)
    {
        if (!m_scene->started())
            m_scene->start();

        m_scene->update(Application::getDeltaTime());
    }
}

void PrimaryWindow::render()
{
    if (!this->m_docking)
        this->renderBackgroundWindow();

    if (m_scene)
        m_scene->render();
}

_Ret_maybenull_ std::shared_ptr<Scene>& PrimaryWindow::currentScene() noexcept
{
    return m_scene;
}

Vector2i PrimaryWindow::getPosition()
{
    Vector2i pos;
    glfwGetWindowPos(this->m_windowHandle, &pos.x(), &pos.y());

    return pos;
}

_Ret_maybenull_ std::shared_ptr<Scene> const& PrimaryWindow::currentScene() const noexcept
{
    return m_scene;
}

void PrimaryWindow::renderBackgroundWindow()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    this->m_backgroundWindow->beginWindow();
    ImGui::PopStyleVar(3);

    if (this->m_docking)
    {
        ImGuiID dockSpaceId = ImGui::GetID("InvisibleWindowDockSpace");
        ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
    }

    this->m_backgroundWindow->render();

    ImGui::End();
}

void PrimaryWindow::glUpdate()
{
    glfwPollEvents();
    glfwSwapBuffers(m_windowHandle);
}

void PrimaryWindow::setDockSpace(const bool update)
{
    this->m_docking = update;
}


void PrimaryWindow::setCursorStatus(bool hidden)
{
    if (hidden)
        glfwSetInputMode(this->getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    else
        glfwSetInputMode(this->getHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

void PrimaryWindow::destroy()
{
    this->m_scene->destroy();
    this->m_scene.reset();
    delete this->m_backgroundWindow;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(this->m_windowHandle);
}

void PrimaryWindow::setClose(bool value)
{
    if (value)
        m_shouldClose = true;
    else
    {
        m_shouldClose = false;
        glfwSetWindowShouldClose(m_windowHandle, GLFW_FALSE);
    }
}
