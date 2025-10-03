#include "Window/VirtualWindow.h"

#include "Application.h"
#include "Id.h"
#include "Window/WindowConfig.h"

#include <sal.h>

using namespace LibMath;

using namespace KaputEngine;

using Rendering::Color;

using std::string;

VirtualWindow::VirtualWindow(_In_ const char* title)
{
    this->m_title = title;
    this->m_size  = Vector2f{ WindowConfig::DEFAULT_WIDTH, WindowConfig::DEFAULT_HEIGHT };
    this->m_flags = UIWindowFlags::E_NONE;
}

VirtualWindow::VirtualWindow(_In_ const char* const title, const UIWindowFlags flags)
{
    this->m_title = title;
    this->m_size  = Vector2f{ WindowConfig::DEFAULT_WIDTH, WindowConfig::DEFAULT_HEIGHT };
    this->m_flags = flags;
}

VirtualWindow::VirtualWindow(_In_ const char* const title, const Vector2f& size)
{
    this->m_title = title;
    this->m_size  = size;
    this->m_flags = UIWindowFlags::E_NONE;
}

VirtualWindow::VirtualWindow(_In_ const char* const title, const Vector2f& size, const UIWindowFlags flags)
{
    this->m_title = title;
    this->m_size  = size;
    this->m_flags = flags;
}

void VirtualWindow::setScale(const Vector2f& size)
{
    this->m_size = size;
}

void VirtualWindow::setFlags(const UIWindowFlags flags)
{
	this->m_flags = flags;
}

void VirtualWindow::setWindowSizeInFrame(const Vector2f& size)
{
    ImGui::SetWindowSize({ size.x(), size.y() });
}

void VirtualWindow::setWindowPositionInFrame(const Vector2f& size)
{
    ImGui::SetWindowPos({ size.x(), size.y() });
}

void VirtualWindow::setWindowFocusInFrame()
{
    this->beginWindow();
    ImGui::SetWindowFocus();
    this->endWindow();
}

bool VirtualWindow::isWindowFocused()
{
    this->beginWindow();
    bool result = ImGui::IsWindowFocused();
    this->endWindow();

    return result;
}

void VirtualWindow::setCursorPos(const Vector2f& position)
{
    ImGui::SetCursorPos({ position.x(), position.y() });
}

std::shared_ptr<UIImage> VirtualWindow::addImage(
    const Vector2f& size, const Vector2f& pos, const std::shared_ptr<Resource::TextureResource>& src, const bool aligned, const bool anchored)
{
	Vector2f ratio = pos / this->m_size;
    std::shared_ptr<UIImage> result = std::make_shared<UIImage>(
       UIObject::ConstructorBlocker(), size, Vector4f{ pos.x(),pos.y(), ratio.x(),ratio.y() }, src, aligned, anchored);
    this->m_widgets.push_back(result);
    result->setParentWindow(this);
    return result;
}

std::shared_ptr<SceneRenderer> VirtualWindow::addGlImage(const Vector2i& size, const unsigned int data)
{
    std::shared_ptr<SceneRenderer> result = std::make_shared<SceneRenderer>(UIObject::ConstructorBlocker(), size, data);
	this->m_widgets.push_back(result);
    result->setParentWindow(this);
	return result;
}

std::shared_ptr<UIText> VirtualWindow::addText(
    const Vector2f& pos, const string& text, const Color& col, const bool aligned, const bool anchored)
{
	Vector2f ratio = pos / this->m_size;
    std::shared_ptr<UIText> result = std::make_shared<UIText>(
        UIObject::ConstructorBlocker(), Vector4f{ pos.x(), pos.y(), ratio.x(), ratio.y() }, text, col, aligned, anchored);
    this->m_widgets.push_back(result);
    result->setParentWindow(this);
    return result;
}

void KaputEngine::VirtualWindow::removeItemFocus()
{
    ImGui::SetWindowFocus(nullptr);
}

bool VirtualWindow::TreeNode(const std::string& text)
{
    return ImGui::TreeNodeEx(text.c_str());
}

bool VirtualWindow::TreeNode(const std::string& text, TreeNodeFlags flags)
{
    return ImGui::TreeNodeEx(text.c_str(), flags);
}

bool VirtualWindow::isItemClicked()
{
    return ImGui::IsItemClicked();
}

bool VirtualWindow::startRightClickItemMenu()
{
    return ImGui::BeginPopupContextItem();
}

bool VirtualWindow::beginMenu(const std::string& name)
{
    return ImGui::BeginMenu(name.c_str());
}

void KaputEngine::VirtualWindow::endMenu()
{
    ImGui::EndMenu();
}

bool VirtualWindow::isItemHovered()
{
    return ImGui::IsItemHovered();
}

void VirtualWindow::endPopup()
{
    ImGui::EndPopup();
}

bool VirtualWindow::addMenuItem(const string& name)
{
    return ImGui::MenuItem(name.c_str());
}

void VirtualWindow::popTree()
{
    ImGui::TreePop();
}

bool VirtualWindow::beginDragAndDropSource()
{
    return ImGui::BeginDragDropSource(ImGuiDragDropFlags_None);
}

bool VirtualWindow::beginDragAndDropTarget()
{
    return ImGui::BeginDragDropTarget();
}

void VirtualWindow::endDragAndDropSource()
{
    ImGui::EndDragDropSource();
}

void VirtualWindow::endDragAndDropTarget()
{
    ImGui::EndDragDropTarget();
}

void VirtualWindow::setDragNDropValue(const string& name, const Id& id)
{
    ImGui::SetDragDropPayload(name.c_str(), &id, sizeof(Id), ImGuiCond_Once);
}

void VirtualWindow::setDragNDropValue(const string& name, const char* const id, const size_t size)
{
    ImGui::SetDragDropPayload(name.c_str(), id, size, ImGuiCond_Once);
}

void VirtualWindow::renderText(const string& text, bool wrappedPos)
{
    if(wrappedPos)
    {
        ImGui::PushTextWrapPos(this->m_size.x());
        ImGui::Text(text.c_str());
        ImGui::PopTextWrapPos();
    }
    else
        ImGui::Text(text.c_str());

}

void VirtualWindow::renderText(const string& text, const Vector2f& position, bool wrappedPos)
{
    if (wrappedPos)
    {
        ImGui::SetCursorPos({ position.x(), position.y() });
        ImGui::PushTextWrapPos(this->m_size.x());
        ImGui::Text(text.c_str());
        ImGui::PopTextWrapPos();
    }
    else
    {
        ImGui::SetCursorPos({ position.x(), position.y() });
        ImGui::Text(text.c_str());
    }
}

void VirtualWindow::renderText(const string& text, const Vector2f& position, const Color& col, bool wrappedPos)
{
    if (wrappedPos)
    {
        ImGui::SetCursorPos({ position.x(), position.y() });
        ImGui::PushTextWrapPos(this->m_size.x());
        ImGui::TextColored(ImColor { col.r(), col.g(), col.b(), col.a() }, text.c_str());
        ImGui::PopTextWrapPos();
    }
    else
    {
        ImGui::SetCursorPos({ position.x(), position.y() });
        ImGui::TextColored(ImColor { col.r(), col.g(), col.b(), col.a() }, text.c_str());
    }
}

bool VirtualWindow::renderButton(const string& name)
{
    return ImGui::Button(name.c_str());
}

float VirtualWindow::getFrameHeight()
{
    this->beginWindow();
    float result = ImGui::GetFrameHeight();
    this->endWindow();
    return result;
}

bool VirtualWindow::renderButton(const string& name, const Vector2f& position)
{
    ImGui::SetCursorPos({ position.x(), position.y() });
    return ImGui::Button(name.c_str());
}

bool VirtualWindow::selectable(const string& name, const bool status, const Vector2f& position)
{
	ImGui::SetCursorPos({ position.x(), position.y() });
	return ImGui::Selectable(name.c_str(), status);
}

bool VirtualWindow::selectable(const string& name)
{
    return ImGui::Selectable(name.c_str());
}

void KaputEngine::VirtualWindow::callSeparator()
{
    ImGui::Separator();
}

bool VirtualWindow::renderVec3(const std::string& name, LinearMathArray3f& vec, bool readOnly)
{
    ImGuiInputTextFlags flags;

    if (readOnly)
        flags = ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_ReadOnly;
    else
        flags = ImGuiInputTextFlags_CharsDecimal;

    return ImGui::InputFloat3(name.c_str(), &vec[0], "%.3f", flags);
}

bool VirtualWindow::renderVec2(const std::string& name, LinearMathArray2f& vec, bool readOnly)
{
    ImGuiInputTextFlags flags;

    if (readOnly)
        flags = ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_ReadOnly;
    else
        flags = ImGuiInputTextFlags_CharsDecimal;

    return ImGui::InputFloat2(name.c_str(), &vec[0], "%.3f", flags);
}

bool VirtualWindow::renderInputFloat(const std::string& name, float& value, bool readOnly)
{
    ImGuiInputTextFlags flags;

    if (readOnly)
        flags = ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_ReadOnly;
    else
        flags = ImGuiInputTextFlags_CharsDecimal;

    return ImGui::InputFloat(name.c_str(), &value, flags);
}

bool VirtualWindow::renderSlider(const string& name, _In_ float& value, const Vector2f& minMax, bool readOnly)
{
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_CharsDecimal;

    if (readOnly)
        flags = ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_ReadOnly;

    return ImGui::SliderFloat(name.c_str(), &value, minMax.x(), minMax.y(), "%.3f", flags);
}

bool VirtualWindow::renderInt(const string& name, _In_ int& value, bool readOnly)
{
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_CharsDecimal;

    if (readOnly)
        flags = ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_ReadOnly;

    return ImGui::InputInt(name.c_str(), &value, flags);
}

bool VirtualWindow::renderColorPicker(const string& name, Color& col, bool readOnly)
{
    if (readOnly)
        return ImGui::ColorEdit3(name.c_str(), &col[0], ImGuiColorEditFlags_NoInputs);
    else
        return ImGui::ColorEdit3(name.c_str(), &col[0]);
}

void VirtualWindow::InitPopUp(const string& name)
{
    ImGui::OpenPopup(name.c_str());
}

bool VirtualWindow::isMouseInWindow()
{
    Vector2d
        windowSize = this->getSize(),
        windowPos = this->getPosition(),			//Based on the entire screen (Top Left Corner)
        mousPos = Application::getInputManager().mousePosition();

    if (mousPos.x() > windowPos.x() && mousPos.x() < windowPos.x() + windowSize.x())
        if (mousPos.y() > windowPos.y() && mousPos.y() < windowPos.y() + windowSize.y())
            return true;

    return false;
}

bool VirtualWindow::beginPopUp(const string& name)
{
    return ImGui::BeginPopup(name.c_str(), ImGuiWindowFlags_NoMove);
}

void VirtualWindow::endPopUp()
{
    ImGui::EndPopup();
}

void VirtualWindow::onSameLine(const float offset)
{
    ImGui::SameLine(offset);
}

void VirtualWindow::onSameLine()
{
    ImGui::SameLine();
}

bool VirtualWindow::renderCheckBox(const string& name, bool& value, bool readOnly)
{
    if (readOnly)
    {
        bool cpy = value;
        return ImGui::Checkbox(name.c_str(), &cpy);
    }
    else
        return ImGui::Checkbox(name.c_str(), &value);
}

void VirtualWindow::pushID(const char* name)
{
    ImGui::PushID(name);
}

void VirtualWindow::popID()
{
    ImGui::PopID();
}

bool VirtualWindow::renderInputText(
    _In_ const char* name, _Out_writes_bytes_(size) char* const buf, const size_t size, bool readOnly)
{
    ImGui::SetNextItemAllowOverlap();
    ImGuiInputTextFlags flags;

    if(readOnly)
        flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll |ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_ReadOnly;
    else
        flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll |ImGuiInputTextFlags_CharsNoBlank;

    return ImGui::InputText(name, buf, size, flags);
}

std::shared_ptr<UISlider> VirtualWindow::addSlider(
    const Vector2f& pos, const Vector2f& minMax, float* data, const char* const name, const float width, const bool aligned, const bool anchored)
{
    Vector2f ratio = pos / this->m_size;
    std::shared_ptr<UISlider> result = std::make_shared<UISlider>(
       UIObject::ConstructorBlocker(), Vector4f{ pos.x(), pos.y(), ratio.x(), ratio.y() }, minMax, data, name, width, aligned, anchored);
    this->m_widgets.push_back(result);
    result->setParentWindow(this);
    return result;
}

std::shared_ptr<UIButton> VirtualWindow::addButton(
    const char* const name, const Vector2f& position, const Vector2f& size, const std::function<void()>& func, const bool aligned, const bool anchored)
{
    Vector2f ratio = position / this->m_size;
	Vector2f sizeRatio = size / this->m_size;
    std::shared_ptr<UIButton> result = std::make_shared<UIButton>(UIObject::ConstructorBlocker(), name, Vector4f{ position.x(),position.y(), ratio.x(), ratio.y() }, Vector4f{ size.x(), size.y(), sizeRatio.x(), sizeRatio.y() }, func, aligned, anchored);
    this->m_widgets.push_back(result);
    result->setParentWindow(this);
    return result;
}

const Vector2f VirtualWindow::getSize()
{
    this->beginWindow();
    float window_width = ImGui::GetWindowSize().x;
    float window_height = ImGui::GetWindowSize().y;

    this->endWindow();

    this->m_size = {window_width,window_height};

    return this->m_size;
}

const Vector2f VirtualWindow::getContentSize()
{
	this->beginWindow();
	float window_width = ImGui::GetContentRegionAvail().x;
	float window_height = ImGui::GetContentRegionAvail().y;

	this->endWindow();

	return  {window_width,window_height};
}

const Vector2f VirtualWindow::getPadding()
{
	this->beginWindow();
	ImVec2 pad = ImGui::GetCurrentWindow()->WindowPadding;
	this->endWindow();

	return {pad.x,pad.y};
}

const UIWindowFlags VirtualWindow::getFlags()
{
	return this->m_flags;
}

//Return bottom left corner of the window
const Vector2f VirtualWindow::getPosition()
{
    this->beginWindow();
	ImRect windowRect = ImGui::GetCurrentWindow()->InnerRect;
    this->endWindow();

    return Vector2f{ windowRect.Min.x, windowRect.Min.y};
}

bool VirtualWindow::hasResized() noexcept
{
    this->beginWindow();
    this->endWindow();
    return this->m_resized;
}

void VirtualWindow::setInitialized(bool setter)
{
    this->m_initiated = setter;
}

void VirtualWindow::render()
{
    this->beginWindow();

    for (std::shared_ptr<UIObject> widget : this->m_widgets)
    {
		if(widget->getAligned())
			ImGui::SameLine();
        widget->setScale(this->getSize());

        if (widget->getParentWindow() != this)
            widget->setParentWindow(this);

        widget->render();
    }

    this->endWindow();
}

void VirtualWindow::beginWindow()
{
	ImGui::Begin(this->m_title,nullptr, static_cast<ImGuiWindowFlags_>(m_flags));

    this->checkInitiation();
}

void VirtualWindow::endWindow()
{
    ImGui::End();
    this->m_initiated = false;
}

std::vector<std::shared_ptr<UIObject>>& VirtualWindow::getUIObjects()
{
    return this->m_widgets;
}

void VirtualWindow::addUIObject(_In_ std::shared_ptr<UIObject>& obj)
{
    this->m_widgets.push_back(obj);
}

void VirtualWindow::init()
{
    this->updateSize();
    this->m_initiated = true;
}

void VirtualWindow::checkInitiation()
{
    if (!this->m_initiated)
        this->init();
}

void VirtualWindow::updateSize()
{
    ImVec2 size = ImGui::GetWindowSize();

    this->m_resized = (size.y != this->m_size.y() || size.x != this->m_size.x());

    this->m_size = { size.x, size.y };
}
