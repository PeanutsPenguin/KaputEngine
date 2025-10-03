#pragma once

#include "Audio/Engine.h"
#include "InputManager.h"
#include "Window/PrimaryWindow.h"

#include <functional>
#include <sol/sol.hpp>

namespace KaputEngine
{
    class Scene;

    class Application
    {
    public:
        /// <summary>
        /// Initializes the application.
        /// </summary>
        /// <param name="title">Name of the window</param>
        /// <param name="size">Size of the window</param>
        /// <returns>Returns false if initialization fails</returns>
        static _NODISCARD bool init(const char* title, const LibMath::Vector2i& size);

        /// <summary>
        /// Runs the application loop.
        /// </summary>
        static void run();

        /// <summary>
        /// Exits the application loop.
        /// </summary>
        static void quit();

		static void clearScreen(const Rendering::Color& col);

        /// <summary>
        /// Returns a reference to the Application's main window.
        /// </summary>
        /// <returns>A reference to PrimaryWindow class</returns>
        static PrimaryWindow& getWindow() noexcept;

        static double getDeltaTime() noexcept;

        static InputManager& getInputManager() noexcept;

        static void setPauseBool();

        static void setWindowShouldClose(bool value);

        /// <summary>
        /// Adds a UI window for the editor.
        /// </summary>
        /// <param name="window">Object from de ImGuiVirtualWindow class</param>
        /// <returns>A pointer to the created window</returns>
        static VirtualWindow* addUIWindow(VirtualWindow& window);

        /// <summary>
        /// Adds a UI window for the editor.
        /// </summary>
        /// <param name="name">Name of the window you want to create</param>
        /// <returns>A pointer to the created window</returns>
        static VirtualWindow* addUIWindow(const char* name);

        /// <summary>
        /// Adds a UI window for the editor.
        /// </summary>
        /// <param name="name">Name of the window you want to create</param>
        /// <param name="flags"></param>
        /// <returns>A pointer to the created window</returns>
        static VirtualWindow* addUIWindow(const char* name, UIWindowFlags flags);

        /// <summary>
        /// Initializes a new UI frame.
        /// </summary>
        static void newUIFrame();

        /// <summary>
        /// Renders the UI windows.
        /// </summary>
        static void renderWindows();

        /// <summary>
        /// Renders the UI Frame.
        /// </summary>
        static void renderUIFrame();

		/// <summary>
		/// Cleans up resources used by the application.
		/// </summary>
		static void cleanup();

		static void update();

		static void resizeViewport(const LibMath::Vector2i& size);

		_NODISCARD static sol::state& luaState() noexcept;

        _NODISCARD static Audio::AudioEngine& audio() noexcept;

        static void onClose(std::function<void()>&& func);

        static void callCloseFunctions();

        static std::function<IUpdatable::UpdateFunc>
            preUpdate,
            postUpdate;

        static std::function<IRenderable::RenderFunc>
            preRender,
            postRender;

    private:
		static void updateDeltaTime();

        friend class ObjectBase;
        friend class RenderComponent;

        static PrimaryWindow s_window;
        static InputManager s_inputs;

        static std::vector<VirtualWindow*> s_UIWindows;

        static double
            s_lastFrame,
            s_deltaTime;

        static bool s_shouldQuit;
        static bool s_paused;

		static sol::state s_lua;

        static Audio::AudioEngine s_audioEngine;

        static std::vector<std::function<void()>> s_onClose;
    };
}
