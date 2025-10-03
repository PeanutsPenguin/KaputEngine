#pragma once

#include "Id.h"
#include "Rendering/GlTypes.h"
#include "Window/UIObject.h"
#include "Window/WindowConfig.h"

#include <functional>

namespace KaputEngine
{
    class VirtualWindow
    {
    public:
        //No default constructor can be initialized because the window need a name
        VirtualWindow() = delete;
        VirtualWindow(_In_ const char* title);
        VirtualWindow(_In_ const char* title, UIWindowFlags flags);
        VirtualWindow(_In_ const char* title, const LibMath::Vector2f& size);
        VirtualWindow(_In_ const char* title, const LibMath::Vector2f& size, UIWindowFlags flags);

        VirtualWindow(VirtualWindow&)  = default;
        VirtualWindow(VirtualWindow&&) = default;
        VirtualWindow& operator=(VirtualWindow&)  = default;
        VirtualWindow& operator=(VirtualWindow&&) = default;

        ~VirtualWindow() = default;

        /// <summary>
        /// Set the size of the window
        /// </summary>
        /// <param name="size">Vector 2 of the required size</param>
        void setScale(const LibMath::Vector2f& size);

		void setFlags(UIWindowFlags flags);

        void setWindowSizeInFrame(const LibMath::Vector2f& size);

        void setWindowPositionInFrame(const LibMath::Vector2f& size);

        void setWindowFocusInFrame();

        _NODISCARD bool isWindowFocused();

        void setCursorPos(const LibMath::Vector2f& position);

        /// <summary>
        /// Add an image to render on the calling window
        /// </summary>
        /// <param name="data">Texture id of the wanted image</param>
        /// <param name="pos">Desired position of the image</param>
        /// <param name="size">Size of the image</param>
        std::shared_ptr<UIImage> addImage(
            const LibMath::Vector2f& size, const LibMath::Vector2f& pos, const std::shared_ptr<Resource::TextureResource>& src, bool aligned = false, bool anchored = false);

        std::shared_ptr<SceneRenderer> addGlImage(const LibMath::Vector2i& size, unsigned int data);

        /// <summary>
        /// Add text on the calling window
        /// </summary>
        /// <param name="pos">Position of the text</param>
        /// <param name="col">Color of the text (r, g, b, a) from 0 to 1</param>
        /// <param name="text">text you want to display</param>
        std::shared_ptr<UIText> addText(
            const LibMath::Vector2f& pos, const std::string& text, const Rendering::Color& col, bool aligned = false, bool anchored = false);

        void removeItemFocus();

        _NODISCARD bool TreeNode(const std::string& text);

        _NODISCARD bool TreeNode(const std::string& text, TreeNodeFlags flags);

        void popTree();

        _NODISCARD bool isItemClicked();

        _NODISCARD bool startRightClickItemMenu();

        _NODISCARD bool beginMenu(const std::string& name);

        void endMenu();

        _NODISCARD bool isItemHovered();

        void endPopup();

        _NODISCARD bool beginDragAndDropSource();

        _NODISCARD bool beginDragAndDropTarget();

        void endDragAndDropSource();

        void endDragAndDropTarget();

        void setDragNDropValue(const std::string& name, const Id& id);

        void setDragNDropValue(const std::string& name, const char* id, size_t size);

        template <typename T>
        _NODISCARD const T* getDragNDropValue(const std::string& name);

        _NODISCARD bool addMenuItem(const std::string& name);

		void renderText(const std::string& text, bool wrappedPos = false);

        void renderText(const std::string& text, const LibMath::Vector2f& position, bool wrappedPos = false);

        void renderText(const std::string& text, const LibMath::Vector2f& position, const  Rendering::Color& col, bool wrappedPos = false);

        _NODISCARD bool renderButton(const std::string& name);

        _NODISCARD float getFrameHeight();

        _NODISCARD bool renderButton(const std::string& name, const LibMath::Vector2f& position);

        _NODISCARD bool selectable(const std::string& name, bool status, const LibMath::Vector2f& pos);

        _NODISCARD bool selectable(const std::string& name);

		void callSeparator();

        _NODISCARD bool renderVec3(const std::string& name, LibMath::LinearMathArray3f& vec, bool readOnly = false);

        _NODISCARD bool renderVec2(const std::string& name, LibMath::LinearMathArray2f& vec, bool readOnly = false);

        _NODISCARD bool renderInputFloat(const std::string& name, float& value, bool readOnly = false);

        _NODISCARD bool renderSlider(const std::string& name, float& value, const LibMath::Vector2f& minMax, bool readOnly = false);

        _NODISCARD bool renderInt(const std::string& name, _In_ int& value, bool readOnly = false);

        _NODISCARD bool renderColorPicker(const std::string& name, Rendering::Color& col, bool readOnly = false);

        void InitPopUp(const std::string& name);

        _NODISCARD bool isMouseInWindow();
            
        _NODISCARD bool beginPopUp(const std::string& name);

        void endPopUp();

        void onSameLine(float offset);

        void onSameLine();

        _NODISCARD bool renderCheckBox(const std::string& name, bool& value, bool readOnly = false);

        void pushID(const char* name); // TODO Annotate ptr

        void popID();

       _NODISCARD bool renderInputText(
           _In_ const char* name, _Out_writes_bytes_(size)  char* buf, size_t size, bool readOnly = false);

        /// <summary>
        /// Add a slider on the calling window
        /// </summary>
        /// <param name="name">Name of the slider</param>
        /// <param name="data">Variable that'll stock the slider data</param>
        /// <param name="minMax">Vector2 that stock the min(x) and the max(y) value</param>
        /// <param name="pos">Position of the slider</param>
        /// <param name="width">Width in percentage of the window (from 0 to 1)</param>
        std::shared_ptr<UISlider> addSlider(
            const LibMath::Vector2f& pos, const LibMath::Vector2f& minMax, float* data, const char* name, float width, bool aligned = false, bool anchored = false);

        /// <summary>
        /// Get the width and height of the window
        /// </summary>
        /// <returns>A Vector2 of the size</returns>
        const LibMath::Vector2f getSize();

		const LibMath::Vector2f getContentSize();

		const _NODISCARD LibMath::Vector2f getPadding();

		const UIWindowFlags getFlags();

        /// <summary>
        /// Add a button to render to the calling Window
        /// </summary>
        /// <param name="name">Name of the button</param>
        /// <param name="position">Position of the button</param>
        /// <param name="size">Size in percentage from the size of the window (from 0 to 1)</param>
        /// <param name="func">std::function that'll be call when the button will be clicked</param>
        /// <returns>A pointer to the button just created</returns>
        std::shared_ptr<UIButton> addButton(
            const char* name, const LibMath::Vector2f& position, const LibMath::Vector2f& size, const std::function<void()>& func, bool aligned = false, bool anchored = false);

        /// <summary>
        /// Return a vec2 with the position of the calling window
        /// </summary>
        _NODISCARD const LibMath::Vector2f getPosition();

        _NODISCARD bool hasResized() noexcept;

        /// <summary>
        /// Sets the Initialized flag.
        /// </summary>
        /// <param name="setter">Set to true of you want to skip the initialisation</param>
        void setInitialized(bool setter);

        /// <summary>
        /// Renders the calling window.
        /// </summary>
        void render();

        /// <summary>
        /// Call ImGui::Begin() for the window, don't forget to call EndWindow()
        /// </summary>
        void beginWindow();

        /// <summary>
        /// Call ImGui::End() for the window
        /// </summary>
        void endWindow();

        _NODISCARD   std::vector<std::shared_ptr<UIObject>>& getUIObjects();

        void addUIObject(_In_ std::shared_ptr<UIObject>& obj);

    private:
        /// <summary>
        /// Init the ImGuiVirtualWindow correctly for now only set the right size but might be add
        /// more stuff that need to be call only on the first "Begin()" later
        /// </summary>
        void init();

        /// <summary>
        /// Check if the CustomWindow has been initiated correctly
        /// </summary>
        void checkInitiation();

        void updateSize();

        std::vector<std::shared_ptr<UIObject>> m_widgets;
        LibMath::Vector2f m_size;
        const char* m_title;
        UIWindowFlags m_flags;
        bool m_initiated = false;
        bool m_resized = false;
    };
}

#include "VirtualWindow.hpp"
