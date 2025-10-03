#pragma once

#include "IRenderable.h"
#include "IUpdatable.h"

#include "VirtualWindow.h"

constexpr int OPENGL_MAJOR_VERSION = 4;
constexpr int OPENGL_MINOR_VERSION = 3;

struct GLFWwindow;

namespace KaputEngine
{
    class Scene;

    class PrimaryWindow : public IUpdatable, public IRenderable
    {
    public:
        PrimaryWindow() = default;
        ~PrimaryWindow() override = default;

        PrimaryWindow(PrimaryWindow&) = delete;
        PrimaryWindow(PrimaryWindow&&) = delete;

        GLFWwindow* getHandle() noexcept;

        /// <summary>
        /// Init the app correctly
        /// </summary>
        /// <param name="title">Name of the window you want to initialize</param>
        /// <param name="size">Size of the window you want to create</param>
        /// <returns>Boolean to check if everything initialized correctly</returns>
        _NODISCARD bool init(const char* title, const LibMath::Vector2i& size);

        /// <summary>
        /// Get the size of the window
        /// </summary>
        /// <returns>A Vec2 with the size</returns>
        _NODISCARD const LibMath::Vector2i& getSize() const noexcept;

        /// <summary>
        /// Get a pointer to the UI window
        /// </summary>
        /// <returns>A pointer to the UI window</returns>
        _NODISCARD VirtualWindow* getBackgroundWindow() noexcept;

        /// <summary>
        /// Set the size of the window
        /// </summary>
        /// <param name="size">Size you wish</param>
        void setScale(const LibMath::Vector2i& size) noexcept;

        /// <summary>
        /// Returns true if the window should close
        /// </summary>
        /// <returns>A true boolean if the windows should close</returns>
        _NODISCARD bool shouldClose() const noexcept;

        /// <summary>
        /// Close the window
        /// </summary>
        void setClose(bool value);

        /// <summary>
        /// Update the window
        /// </summary>
        void update() override;

        /// <summary>
        /// Render the window
        /// </summary>
        void render() override;

        /// <summary>
        /// Get a pointer to the scene
        /// </summary>
        /// <returns>A pointer the a Scene object </returns>
        _NODISCARD _Ret_maybenull_ std::shared_ptr<Scene>& currentScene() noexcept;

		_NODISCARD LibMath::Vector2i getPosition();

        /// <summary>
        /// Get a pointer to the scene
        /// </summary>
        /// <returns>A pointer the a Scene object </returns>
        _NODISCARD _Ret_maybenull_ const std::shared_ptr<KaputEngine::Scene>&  currentScene() const noexcept;

        //TO DO : fix
        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

        static void window_close_callBack(GLFWwindow* window);

        /// <summary>
        /// Render the UI of the game
        /// </summary>
        void renderBackgroundWindow();

        /// <summary>
        /// Call glSwapBuffer
        /// </summary>
        void glUpdate();

		void setDockSpace(const bool update);

        void setCursorStatus(bool hidden);

        void destroy();
    private:
        /// <summary>
        /// Initialize ImGui correctly
        /// </summary>
        /// <returns>Return false if the initialization failed</returns>
        _NODISCARD bool initImGui();

        VirtualWindow* m_backgroundWindow;
        std::shared_ptr<Scene> m_scene;
        LibMath::Vector2i m_size;
        GLFWwindow* m_windowHandle;
        const char* m_title;
        bool m_shouldClose = false;
		bool m_docking = false;
    };
}
