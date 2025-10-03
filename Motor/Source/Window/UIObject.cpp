#include "Window/UIObject.hpp"

#include "Inspector/Property.hpp"
#include "Resource/Manager.hpp"
#include "Text/Xml/Context.hpp"
#include "Text/Xml/Node.hpp"
#include "Window/VirtualWindow.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

using namespace KaputEngine;
using namespace KaputEngine::Text::Xml;

using KaputEngine::Inspector::Property;
using KaputEngine::Rendering::Color;

using LibMath::Vector2i;
using LibMath::Vector2f;
using LibMath::Vector4f;

using std::string;

#pragma region IMPLEMENTAION
OBJECTBASE_IMPL(UIObject)
OBJECTBASE_IMPL(UIText)
OBJECTBASE_IMPL(UISlider)
OBJECTBASE_IMPL(UIImage)
OBJECTBASE_IMPL(SceneRenderer)
OBJECTBASE_IMPL(UIButton)
#pragma endregion

decltype(UIObject::s_createFuncs) UIObject::s_createFuncs = { };
decltype(UIObject::s_deserializeFuncs) UIObject::s_deserializeFuncs = { };

#pragma region Text
UIText::UIText(ConstructorBlocker)
	: UIText(ConstructorBlocker(), s_nextId++) { }

UIText::UIText(ConstructorBlocker, const Id& id)
	: UIObject(ConstructorBlocker(), id)
{
	this->m_position = { 0 };
	this->m_text     = "Lorem Ipsum";
	this->m_color    = Color::White;
	this->setAligned(false);
	this->m_anchored = false;
}

UIText::UIText(ConstructorBlocker,
	const Vector4f& pos, const string& text, const Color& col, bool align, bool anchored)
	: UIObject(ConstructorBlocker(), s_nextId++)
{
	this->m_position = pos;
	this->m_text     = text;
	this->m_color    = col;
	this->setAligned(align);
	this->m_anchored = anchored;
}

void UIText::serializeValues(XmlSerializeContext& context) const
{
	UIObject::serializeValues(context);

	context.value("Text", this->m_text);
	context.value("Color", this->m_color);
}

bool UIText::deserializeMap(const XmlNode::Map& map)
{
	if (!UIObject::deserializeMap(map))
		return false;

	if (const auto it = map.find("Text"); it != map.end())
	{
		std::optional<string> op = it->second->parse<std::string>();

		if (!op)
		{
			deserializeError(__FUNCTION__, "Text");
			return false;
		}

		m_text = static_cast<string>(*op);
	}

	if (const auto it = map.find("Color"); it != map.end())
	{
		std::optional<Color> op = it->second->parse<Color>();

		if (!op)
		{
			deserializeError(__FUNCTION__, "Color");
			return false;
		}

		m_color = static_cast<Color>(*op);
	}

	return true;
}

void UIText::setText(const string& text)
{
	this->m_text = text;
}

string UIText::getText()
{
	return this->m_text;
}

void UIText::setColor(const Color& col)
{
	this->m_color = col;
}

Color UIText::getColor()
{
	return this->m_color;
}

void UIText::defineLuaMembers(sol::usertype<UIText>& type)
{
	UIObject::defineLuaMembers(type);

	type["text"]  = sol::property(&getText, &setText);
	type["color"] = sol::property(&getColor, &setColor);
}

void UIText::render()
{
	if (this->m_anchored)
	{
		Vector2f pos = this->getParentWindow()->getPadding();
		pos.y() += this->getParentWindow()->getFrameHeight();

		Vector2f windowSize = this->getWindowSize();
		ImGui::SetCursorPos({ this->m_position.z() * windowSize.x() + pos.x(), this->m_position.w() * windowSize.y() + pos.y() });
		ImGui::PushTextWrapPos(this->getWindowSize().x());
		ImGui::TextColored(ImColor { this->m_color.r(), this->m_color.g(), this->m_color.b(), this->m_color.a() }, this->m_text.c_str());
		ImGui::PopTextWrapPos();
	}
	else
	{
		Vector2f pos = this->getParentWindow()->getPadding();
		pos.y() += this->getParentWindow()->getFrameHeight();

		ImGui::SetCursorPos({ this->m_position.x() + pos.x(), this->m_position.y() + pos.y() });
		ImGui::PushTextWrapPos(this->getWindowSize().x());
		ImGui::TextColored(ImColor { this->m_color.r(), this->m_color.g(), this->m_color.b(), this->m_color.a() }, this->m_text.c_str());
		ImGui::PopTextWrapPos();
	}
}

void UIText::getProperties(std::vector<Property>& out) noexcept
{
	Base::getProperties(out);

	out.emplace_back("Text",
	[this]() -> string { return getText(); },
	[this](const string& txt) { setText(txt); });

	out.emplace_back("Color",
	[this]() -> Color { return getColor(); },
	[this](const Color& col) { setColor(col); });
}
#pragma endregion

#pragma region Image
UIImage::UIImage(ConstructorBlocker,
	const Vector2f& size, const Vector4f& pos, const std::shared_ptr<Resource::TextureResource>& src, bool align, bool anchored)
	: UIObject(ConstructorBlocker(), s_nextId++)
{
	this->m_size     = size;
	this->m_position = pos;
	this->m_resource = src;
	this->setAligned(align);
	this->m_anchored = anchored;
}

void UIImage::serializeValues(XmlSerializeContext& context) const
{
	UIObject::serializeValues(context);

	context.value("Path", m_resource->path()->string());
}

bool UIImage::deserializeMap(const XmlNode::Map& map)
{
	if (!UIObject::deserializeMap(map))
		return false;

	if (const auto it = map.find("Path"); it != map.end())
	{
		std::optional<string> op = it->second->parse<string>();

		if (!op)
		{
			deserializeError(__FUNCTION__, "Path");
			return false;
		}

		m_resource = Resource::ResourceManager::get<Resource::TextureResource>(static_cast<string>(*op));
	}

	return true;
}

UIImage::UIImage(ConstructorBlocker)
	: UIImage(ConstructorBlocker(), s_nextId++) { }

UIImage::UIImage(ConstructorBlocker, const Id& id)
	: UIObject(ConstructorBlocker(), id)
{
	this->m_position = { 0 };
	this->m_size     = { 573, 574 };
	this->m_resource = Resource::ResourceManager::get<Resource::TextureResource>("Kaput/Texture/noTexture.kasset");

	this->setAligned(false);
	this->m_anchored = false;
}

void UIImage::render()
{
	if (this->m_anchored)
	{
		Vector2f pos = this->getParentWindow()->getPadding();
		pos.y() += this->getParentWindow()->getFrameHeight();

		const Vector2f windowSize = this->getWindowSize();
		ImGui::SetCursorPos({ this->m_position.z() * windowSize.x() + pos.x(), this->m_position.w() * windowSize.y() + pos.y() });
	}
	else
	{
		Vector2f pos = Vector2f { this->m_position.x(), this->m_position.y() } + this->getParentWindow()->getPadding();
		pos.y() += this->getParentWindow()->getFrameHeight();
		ImGui::SetCursorPos({ pos.x(), pos.y() });
	}

	unsigned int tex_id = 0;

	if (this->m_resource)
		tex_id = this->m_resource->data().id();

	if (tex_id != 0)
		ImGui::Image(tex_id, { this->m_size.x(), this->m_size.y() }, { 0, 1 }, { 1, 0 });
}

void UIImage::setImagePosition(const Vector2f& pos)
{
	Vector2f ratio = pos / this->getWindowSize();
	this->m_position.x() = pos.x();
	this->m_position.y() = pos.y();
	this->m_position.z() = ratio.x();
	this->m_position.w() = ratio.y();
}

void UIImage::setID(const std::shared_ptr<Resource::TextureResource>& src)
{
	this->m_resource = src;
}

string UIImage::getTexPath()
{
	if (this->m_resource)
		return this->m_resource->path()->string();
	else
		return "No Resource";
}

void UIImage::defineLuaMembers(sol::usertype<UIImage>& type)
{
	UIObject::defineLuaMembers(type);
}
#pragma endregion

#pragma region SceneRenderer
SceneRenderer::SceneRenderer(ConstructorBlocker)
	: UIObject(ConstructorBlocker(), s_nextId++) { }

SceneRenderer::SceneRenderer(ConstructorBlocker(), const Id& id)
	: UIObject(ConstructorBlocker(), id) { }

SceneRenderer::SceneRenderer(ConstructorBlocker, const Vector2f& size, unsigned int data)
	: UIObject(ConstructorBlocker(), s_nextId++), m_texData(data)
{
	this->m_size = size;
}

void SceneRenderer::render()
{
	Vector2f pos = this->getParentWindow()->getPadding();
	pos.y() += this->getParentWindow()->getFrameHeight();
	ImGui::SetCursorPos({ pos.x(), pos.y() });

	ImGui::Image(this->m_texData, { this->m_size.x(), this->m_size.y() }, { 0, 1 }, { 1, 0 });
}

void SceneRenderer::setImageSize(const Vector2i& size)
{
	this->m_size = size;
}

void SceneRenderer::setID(const unsigned int id)
{
	this->m_texData = id;
}
#pragma endregion

#pragma region Slider
UISlider::UISlider(ConstructorBlocker,
	const Vector4f& pos, const Vector2f& minMax, float* data, const char* name, float width, bool align, bool anchored)
	: UIObject(ConstructorBlocker(), s_nextId++)
{
	this->m_position    = pos;
	this->m_minMax      = minMax;
	this->m_dataStocker = data;
	this->m_name        = name;
	this->m_width       = width;
	this->setAligned(align);
	this->m_anchored    = anchored;
}

Vector2f UISlider::getMinMax()
{
	return this->m_minMax;
}

void UISlider::setMinMax(const Vector2f& value)
{
	this->m_minMax = value;
}

float UISlider::getValue()
{
	return *this->m_dataStocker;
}

void UISlider::setDataStocker(float* value)
{
	this->m_dataStocker = value;
}

string UISlider::getName()
{
	return this->m_name;
}

void UISlider::setName(const string& newName)
{
	this->m_name = newName;
}

float UISlider::getWidth()
{
	return this->m_width;
}

void UISlider::setWidth(float newValue)
{
	this->m_width = newValue;
}

UISlider::UISlider(ConstructorBlocker)
	: UISlider(ConstructorBlocker(), s_nextId++) { }

UISlider::UISlider(ConstructorBlocker, const Id & id)
	: UIObject(ConstructorBlocker(), id)
{
	this->m_position    = { 0 };
	this->m_minMax      = { 0, 5 };
	this->m_dataStocker = nullptr;
	this->m_name        = "newSlider";
	this->m_width       = 500;
	this->setAligned(false);
	this->m_anchored    = false;
}

void UISlider::defineLuaMembers(sol::usertype<UISlider>& type)
{
	UIObject::defineLuaMembers(type);

	type["minMax"]      = sol::property(&getMinMax, &setMinMax);
	type["dataStocker"] = sol::property(&getValue, &setDataStocker);
	type["name"]        = sol::property(&getName, &setName);
	type["width"]       = sol::property(&getWidth, &setWidth);
}

void UISlider::render()
{
	if (this->m_anchored)
	{
		Vector2f pos = this->getParentWindow()->getPadding();
		pos.y() += this->getParentWindow()->getFrameHeight();

		const Vector2f windowSize = this->getWindowSize();
		ImGui::SetCursorPos({ this->m_position.z() * windowSize.x() + pos.x(), this->m_position.w() * windowSize.y() + pos.y() });
	}
	else
	{
		Vector2f
			pad    = this->getParentWindow()->getPadding(),
			newPos = LibMath::Vector2f { this->m_position.x(), this->m_position.y() } + pad;

		newPos.y() += this->getParentWindow()->getFrameHeight();
		ImGui::SetCursorPos({ newPos.x(), newPos.y() });
	}

	ImGui::SetNextItemWidth(this->m_width);

	if (this->m_dataStocker)
		ImGui::SliderFloat(this->m_name.c_str(), this->m_dataStocker, this->m_minMax.x(), this->m_minMax.y());
	else
	{
		float temp = 0;
		ImGui::SliderFloat(this->m_name.c_str(), &temp, this->m_minMax.x(), this->m_minMax.y());
	}
}

void UISlider::serializeValues(XmlSerializeContext& context) const
{
	UIObject::serializeValues(context);

	context.value("minMax", m_minMax);
	context.value("name", m_name);
	context.value("width", m_width);
}

bool UISlider::deserializeMap(const XmlNode::Map& map)
{
	if (!UIObject::deserializeMap(map))
		return false;

	if (const auto it = map.find("minMax"); it != map.end())
	{
		std::optional<Vector2f> op = it->second->parse<Vector2f>();

		if (!op)
		{
			deserializeError(__FUNCTION__, "minMax");
			return false;
		}

		m_minMax = static_cast<Vector2f>(*op);
	}

	if (const auto it = map.find("name"); it != map.end())
	{
		std::optional<string> op = it->second->parse<string>();

		if (!op)
		{
			deserializeError(__FUNCTION__, "name");
			return false;
		}

		m_name = static_cast<string>(*op);
	}

	if (const auto it = map.find("width"); it != map.end())
	{
		std::optional<float> op = it->second->parse<float>();

		if (!op)
		{
			deserializeError(__FUNCTION__, "width");
			return false;
		}

		m_width = static_cast<float>(*op);
	}

	return true;
}

void UISlider::getProperties(std::vector<Property>& out) noexcept
{
	Base::getProperties(out);

	out.emplace_back("SliderName",
	[this]() -> string { return getName(); },
	[this](const string& txt) { setName(txt.c_str()); });

	out.emplace_back("MinMax",
	[this]() -> Vector2f { return getMinMax(); },
	[this](const Vector2f& value) { setMinMax(value); });

	out.emplace_back("Width",
	[this]() -> float { return getWidth(); },
	[this](const float& value) { setWidth(value); });
}
#pragma endregion

#pragma region Button
UIButton::UIButton(ConstructorBlocker,
	const string& name, const Vector4f& pos, const Vector2f& size, std::function<void()> func, bool align, bool anchored)
	: UIObject(ConstructorBlocker(), s_nextId++)
{
	this->m_position = pos;
	this->m_name     = name;
	this->m_size     = size;
	this->m_function = func;
	this->setAligned(align);
	this->m_anchored = anchored;
}

void KaputEngine::UIButton::serializeValues(Text::Xml::XmlSerializeContext& context) const
{
	UIObject::serializeValues(context);

	context.value("name", this->m_name);
}

bool KaputEngine::UIButton::deserializeMap(const Text::Xml::XmlNode::Map& map)
{
	if (!UIObject::deserializeMap(map))
		return false;

	if (const auto it = map.find("name"); it != map.end())
	{
		std::optional<string> op = it->second->parse<string>();

		if (!op)
		{
			deserializeError(__FUNCTION__, "name");
			return false;
		}

		m_name = static_cast<string>(*op);
	}

	return true;
}

UIButton::UIButton(ConstructorBlocker)
	: UIButton(ConstructorBlocker(), s_nextId++) { }

UIButton::UIButton(ConstructorBlocker, const Id & id)
	: UIObject(ConstructorBlocker(), id)
{
	this->m_position = { 0 };
	this->m_name = "newButton";
	this->m_size = { 10, 10 };
	this->setAligned(false);
	this->m_anchored = false;
	this->m_function = []() { std::cout << "No Function Linked" << std::endl; };
}

string UIButton::getName()
{
	return this->m_name;
}

void UIButton::setName(const string& name)
{
	this->m_name = name;
}

void UIButton::render()
{
	if (this->onClick())
		this->m_function();
}

bool KaputEngine::UIButton::isClicked()
{
	return this->m_isClicked;
}

bool UIButton::onClick()
{
	if (this->getAligned())
		return ImGui::Button(this->m_name.c_str(), ImVec2 { this->m_size.x(), this->m_size.y() });

	Vector2f windowSize = this->getWindowSize();

	if (this->m_anchored)
	{
		Vector2f pos = this->getParentWindow()->getPadding();
		pos.y() += this->getParentWindow()->getFrameHeight();

		Vector2f windowSize = this->getWindowSize();
		ImGui::SetCursorPos({ this->m_position.z() * windowSize.x() + pos.x(), this->m_position.w() * windowSize.y() + pos.y() });
	}
	else
	{
		Vector2f
			pad    = this->getParentWindow()->getPadding(),
			newPos = LibMath::Vector2f { this->m_position.x(), this->m_position.y() } + pad;

		newPos.y() += this->getParentWindow()->getFrameHeight();
		ImGui::SetCursorPos({ newPos.x(), newPos.y() });
	}

	this->m_isClicked = ImGui::Button(this->m_name.c_str(), ImVec2 { this->m_size.x(), this->m_size.y() });
	return this->m_isClicked;
}

void UIButton::getProperties(std::vector<Property>& out) noexcept
{
	Base::getProperties(out);

	out.emplace_back("ButtonName",
	[this]() -> std::string { return getName(); },
	[this](const string& txt) { setName(txt); });
}

void UIButton::defineLuaMembers(sol::usertype<UIButton>& type)
{
	UIObject::defineLuaMembers(type);

	type["name"] = sol::property(&getName, &setName);
	type["onClick"] = &onClick;
	type["isClicked"] = sol::readonly_property(&isClicked);
}
#pragma endregion

#pragma region UIObject
UIObject::UIObject(ConstructorBlocker)
	: UIObject(ConstructorBlocker(), s_nextId++) { }

UIObject::UIObject(ConstructorBlocker, const Id & id)
	: ObjectBase(id) { }

std::shared_ptr<UIObject> UIObject::create(const string& typeName)
{
	if (const auto it = s_createFuncs.find(typeName); it != s_createFuncs.end())
		return it->second();
	else
	{
		std::cerr << __FUNCTION__": Unknown Ui Object type \"" << typeName << "\".\n";
		return nullptr;
	}
}

void UIObject::serializeValues(XmlSerializeContext& context) const
{
	ObjectBase::serializeValues(context);

	context.value("Postion", m_position);
	context.value("size", m_size);
	context.value("anchored", m_anchored);
	context.value("align", m_align);
}

bool UIObject::deserializeMap(const XmlNode::Map& map)
{
	if (!ObjectBase::deserializeMap(map))
		return false;

	if (const auto it = map.find("Postion"); it != map.end())
	{
		std::optional<Vector4f> op = it->second->parse<Vector4f>();

		if (!op)
		{
			deserializeError(__FUNCTION__, "Position");
			return false;
		}

		m_position = static_cast<Vector4f>(*op);
	}

	if (const auto it = map.find("size"); it != map.end())
	{
		std::optional<Vector2f> op = it->second->parse<Vector2f>();

		if (!op)
		{
			deserializeError(__FUNCTION__, "size");
			return false;
		}

		m_size = static_cast<Vector2f>(*op);
	}

	if (const auto it = map.find("anchored"); it != map.end())
	{
		std::optional<bool> op = it->second->parse<bool>();

		if (!op)
		{
			deserializeError(__FUNCTION__, "anchored");
			return false;
		}

		m_anchored = static_cast<bool>(*op);
	}

	if (const auto it = map.find("align"); it != map.end())
	{
		std::optional<bool> op = it->second->parse<bool>();

		if (!op)
		{
			deserializeError(__FUNCTION__, "align");
			return false;
		}

		m_align = static_cast<bool>(*op);
	}

	return true;
}

void UIObject::setScale(const Vector2f& size)
{
	this->m_windowSize = size;
}

void UIObject::setAligned(const bool aligned)
{
	this->m_align = aligned;
}

std::vector<string> UIObject::getTypes()
{
	std::vector<string> types;
	types.reserve(s_createFuncs.size());

	for (const std::pair<string, std::function<CreateFunc>>& pair : s_createFuncs)
		types.emplace_back(pair.first);

	return types;
}

GameObject* UIObject::parentObject() const noexcept
{
	return nullptr;
}

Scene* UIObject::parentScene() const noexcept
{
	return nullptr;
}

Vector2f UIObject::getWindowSize() noexcept
{
	return this->m_windowSize;
}

bool UIObject::getAligned() const noexcept
{
	return this->m_align;
}

void UIObject::setParentWindow(VirtualWindow* window)
{
	this->m_window = window;
}

VirtualWindow* UIObject::getParentWindow()
{
	return this->m_window;
}

void UIObject::getProperties(std::vector<Property>& out) noexcept
{
	Base::getProperties(out);

	out.emplace_back("Position",
	[this]() -> Vector2f { return m_position.splice<2>(); },
	[this](const Vector2f& pos)
	{
		Vector2f ratio = pos / m_window->getSize();
		m_position = { pos.x(), pos.y(), ratio.x(), ratio.y() };
	});

	out.emplace_back("Size",
	[this]() -> Vector2f { return m_size; },
	[this](const Vector2f& size) { m_size = size; });

	out.emplace_back("Anchored",
	[this]() -> bool { return m_anchored; },
	[this](const bool& anchor)
	{
		if (anchor)
		{
			Vector2f
				pos = { m_position.x(), m_position.y() },
				ratio = pos / m_window->getSize();

			m_position = { pos.x(), pos.y(), ratio.x(), ratio.y() };
		}

		m_anchored = anchor;
	});
}

_Ret_maybenull_ std::shared_ptr<UIObject> UIObject::load(const XmlNode& node)
{
	const auto createIt = s_deserializeFuncs.find(node.name);

	if (createIt == s_deserializeFuncs.end())
	{
		std::cerr << __FUNCTION__": Unknown UI Object Type type \"" << node.name << "\".\n";
		return nullptr;
	}

	return createIt->second(node);
}
#pragma endregion