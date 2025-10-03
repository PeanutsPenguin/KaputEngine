#pragma once

#include "GameObject/ObjectBase.h"
#include "Rendering/Color.h"
#include "Resource/Texture.h"

#include <LibMath/Vector/Vector2.h>
#include <LibMath/Vector/Vector4.h>

#include <functional>

#define UIOBJECT_SIGS(type, base) \
OBJECTBASE_SIGS(type, base) \
friend UIObject;

namespace KaputEngine
{
	struct VirtualWindow;

	/// <summary>
	/// Class to unify every UI elements
	/// </summary>
	class UIObject : public ObjectBase
	{
		friend class VirtualWindow;

		OBJECTBASE_SIGS(UIObject, ObjectBase)

	protected:
		REGISTER_SIG(UIObject);

	public:
		explicit UIObject(ConstructorBlocker);
		UIObject(ConstructorBlocker, const Id& id);

		template <std::derived_from<UIObject> T, typename... Args>
		_NODISCARD static std::shared_ptr<T> create(Args&&... args);

		_NODISCARD _Success_(return) static std::shared_ptr<UIObject> create(const std::string& typeName);

		template <std::derived_from<UIObject> T>
		static void registerCreate();

		template <std::derived_from<UIObject> T>
		static void registerDeserialize();

		template <std::derived_from<UIObject> T>
		static sol::usertype<T> registerLuaType(sol::state& lua);

		template <std::derived_from<UIObject> T>
		static void defineLuaMembers(sol::usertype<T>& type);

		void serializeValues(Text::Xml::XmlSerializeContext& context) const override;

		_NODISCARD bool deserializeMap(const Text::Xml::XmlNode::Map& map) override;

		_NODISCARD _Ret_maybenull_ static std::shared_ptr<UIObject> load(const Text::Xml::XmlNode& node);

		virtual void render() = 0;
		void setScale(const LibMath::Vector2f& size);
		void setAligned(const bool aligned);

		_NODISCARD static std::vector<std::string> getTypes();

		GameObject* parentObject() const noexcept override;
		Scene* parentScene() const noexcept override;

		LibMath::Vector2f getWindowSize() noexcept;
		bool getAligned() const noexcept;

		void setParentWindow(_In_ VirtualWindow* window);

		_NODISCARD VirtualWindow* getParentWindow();

		void getProperties(std::vector<Inspector::Property>& out) noexcept override;

		~UIObject() override = default;

		bool m_anchored = false;
		LibMath::Vector4f m_position;
		LibMath::Vector2f m_size;

	private :
		bool m_align = false;
		LibMath::Vector2f m_windowSize;
		VirtualWindow* m_window;

		using CreateFunc = std::shared_ptr<UIObject>();
		using DeserializeFunc = _Ret_maybenull_ std::shared_ptr<UIObject>(const Text::Xml::XmlNode& node);

		static std::unordered_map<std::string, CreateFunc*> s_createFuncs;
		static std::unordered_map<std::string, DeserializeFunc*> s_deserializeFuncs;
	};

	class UIText : public UIObject
	{
		UIOBJECT_SIGS(UIText, UIObject)
		OBJECTBASE_DEFAULT_REGISTER(UIText)

	public:
		explicit UIText(ConstructorBlocker);
		UIText(ConstructorBlocker, const Id& id);
		UIText(ConstructorBlocker, const LibMath::Vector4f& pos, const std::string& text, const Rendering::Color& col, bool align, bool anchored);

		void serializeValues(Text::Xml::XmlSerializeContext& context) const override;

		_NODISCARD bool deserializeMap(const Text::Xml::XmlNode::Map& map) override;

		void render() override;

		void setText(const std::string& text);
		_NODISCARD std::string getText();

		void setColor(const Rendering::Color& col);
		_NODISCARD Rendering::Color getColor();

		~UIText() override = default;

		void getProperties(std::vector<Inspector::Property>& out) noexcept override;

	protected:
		static void defineLuaMembers(sol::usertype<UIText>& type);

	private:
		std::string m_text;
		Rendering::Color m_color;
		//TO DO : Add Font
	};

	class UIImage : public UIObject
	{
		UIOBJECT_SIGS(UIImage, UIObject)
		OBJECTBASE_DEFAULT_REGISTER(UIImage)

	public:
		explicit UIImage(ConstructorBlocker);
		UIImage(ConstructorBlocker, const Id& id);
		UIImage(ConstructorBlocker, const LibMath::Vector2f& size, const LibMath::Vector4f& pos, const std::shared_ptr<Resource::TextureResource>& src, bool align, bool anchored);

		void serializeValues(Text::Xml::XmlSerializeContext& context) const override;

		_NODISCARD bool deserializeMap(const Text::Xml::XmlNode::Map& map) override;

		void render() override;

		void setImagePosition(const LibMath::Vector2f& pos);

		void setID(const std::shared_ptr<Resource::TextureResource>& ptr);

		_NODISCARD std::string getTexPath();

		~UIImage() override = default;

	protected:
		static void defineLuaMembers(sol::usertype<UIImage>& type);

	private:
		std::shared_ptr<Resource::TextureResource> m_resource;
	};

	class SceneRenderer: public UIObject
	{
		UIOBJECT_SIGS(SceneRenderer, UIObject)
		OBJECTBASE_DEFAULT_REGISTER(SceneRenderer)

	public:
		explicit SceneRenderer(ConstructorBlocker);
		SceneRenderer(ConstructorBlocker(), const Id& id);
		SceneRenderer(ConstructorBlocker, const LibMath::Vector2f& size, unsigned int data);

		void render() override;

		void setImageSize(const LibMath::Vector2i& size);

		void setID(const unsigned int id);

		~SceneRenderer() override = default;

	private:
		unsigned int m_texData;
	};

	class UISlider : public UIObject
	{
		UIOBJECT_SIGS(UISlider, UIObject)
		OBJECTBASE_DEFAULT_REGISTER(UISlider)

	public:
		explicit UISlider(ConstructorBlocker);
		UISlider(ConstructorBlocker, const Id& id);
		UISlider(ConstructorBlocker, const LibMath::Vector4f& pos,const LibMath::Vector2f& minMax,float* data,const char* name, float width, bool align, bool anchored);

		_NODISCARD LibMath::Vector2f getMinMax();
		void setMinMax(const LibMath::Vector2f& value);

		_NODISCARD float getValue();
		void setDataStocker(float* value);

		_NODISCARD std::string getName();
		void setName(const std::string& newName);

		_NODISCARD float getWidth();
		void setWidth(float newValue);

		void render() override;

		void serializeValues(Text::Xml::XmlSerializeContext& context) const override;

		_NODISCARD bool deserializeMap(const Text::Xml::XmlNode::Map& map) override;

		~UISlider() override = default;

		void getProperties(std::vector<Inspector::Property>& out) noexcept override;

	protected:
		static void defineLuaMembers(sol::usertype<UISlider>& type);

	private:
		LibMath::Vector2f m_minMax;
		float* m_dataStocker;
		std::string m_name;
		float m_width;
	};

	class UIButton : public UIObject
	{
		UIOBJECT_SIGS(UIButton, UIObject)
		OBJECTBASE_DEFAULT_REGISTER(UIButton)

	public:
		explicit UIButton(ConstructorBlocker);
		UIButton(ConstructorBlocker, const Id& id);
		UIButton(ConstructorBlocker,
			const std::string& name,const LibMath::Vector4f& pos, const LibMath::Vector2f& size, std::function<void()> func, bool align, bool anchored);

		void serializeValues(Text::Xml::XmlSerializeContext& context) const override;

		_NODISCARD bool deserializeMap(const Text::Xml::XmlNode::Map& map) override;

		_NODISCARD std::string getName();
		void setName(const std::string& name);

		_NODISCARD bool onClick();

		void render() override;

		_NODISCARD bool isClicked();

		~UIButton() override = default;

		void getProperties(std::vector<Inspector::Property>& out) noexcept override;

	protected:
		static void defineLuaMembers(sol::usertype<UIButton>& type);

	private:
		std::string m_name;
		std::function<void()> m_function;
		bool m_isClicked = false;
	};
}
