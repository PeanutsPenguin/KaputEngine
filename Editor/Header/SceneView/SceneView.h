#pragma once

#include "CustomFrameBuffer/CustomFrameBuffer.h"
#include "Scene/Scene.h"
#include "Window/VirtualWindow.h"

#include <memory>

namespace KaputEditor
{
	class ImGuiVirtualWindow;
	class OpenGlImage;

	class SceneView
	{
	public:
		SceneView(const char* name = "SceneView");

		virtual void preRender();

		virtual void render();

		virtual void postRender() { };

		virtual void resize();

		void bindFramebuffer();

		void unbindFramebuffer();

		_NODISCARD std::shared_ptr<KaputEngine::Scene> scene() noexcept;
		_NODISCARD const std::shared_ptr<KaputEngine::Scene> scene() const noexcept;

		_NODISCARD _Ret_notnull_ KaputEngine::VirtualWindow* getWindow();
		virtual void setScene(_In_ std::shared_ptr<KaputEngine::Scene>& scene);

		~SceneView() = default;

	protected:
		std::shared_ptr<KaputEngine::Scene> m_scene;
		std::unique_ptr<CustomFrameBuffer> m_framebuffer;
		KaputEngine::VirtualWindow* m_window;
		std::shared_ptr<KaputEngine::SceneRenderer> m_image;

		_NODISCARD std::shared_ptr<KaputEngine::Scene> getScene();
	};
}
