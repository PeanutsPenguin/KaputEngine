#pragma once

#include "IWorldRenderable.h"

#include "Physics/PhysicHandler.h"
#include "Rendering/Color.h"
#include "Rendering/Lighting/DirectionalLightBuffer.h"
#include "Rendering/Lighting/PointLightBuffer.h"
#include "Rendering/ShaderProgram.h"
#include "Root.h"
#include "Text/Xml/Context.h"
#include "Text/Xml/Node.h"
#include "Utils/RemoveVector.h"

#include <filesystem>

namespace KaputEngine
{
    class UIObject;

    class Scene:
        public IWorldUpdatable,
        public IWorldRenderable,
        private Text::Xml::IXmlMapParser,
        private Text::Xml::IXmlPolymorphicSerializer
    {
        friend ObjectBase;
        friend class RenderComponent;

    public:
        _NODISCARD _Success_(return) bool load(const std::filesystem::path& path);
        bool save(const std::filesystem::path& path, bool indent) const;

        Scene();

        void start();
        _NODISCARD bool started() const noexcept;

        _NODISCARD SceneRoot& sceneRoot() noexcept;
        _NODISCARD const SceneRoot& sceneRoot() const noexcept;

        _NODISCARD std::weak_ptr<Camera> getPrimaryCamera() noexcept;
        _NODISCARD std::weak_ptr<const Camera> getPrimaryCamera() const noexcept;

        void setPrimaryCamera(Camera& camera) noexcept;
        void setPrimaryCamera(nullptr_t) noexcept;

        _NODISCARD exclusive_weak_ptr<class AudioListenerComponent> getPrimaryListener() noexcept;
        _NODISCARD exclusive_weak_ptr<const class AudioListenerComponent> getPrimaryListener() const noexcept;

        void setPrimaryListener(AudioListenerComponent& listener) noexcept;
        void setPrimaryListener(nullptr_t) noexcept;

        _NODISCARD   std::vector<std::shared_ptr<UIObject>>& getHUD();

        void update(double deltaTime) override;

        void render();
        void render(const Camera& camera) override;

        _NODISCARD Rendering::Color& clearColor() noexcept;

        _NODISCARD const Rendering::Color& clearColor() const noexcept;

        void addUiObject(_In_ const std::shared_ptr<UIObject>& ptr);

        _Ret_maybenull_ _NODISCARD std::shared_ptr<UIObject> getUIObjectByName(const std::string& name);

        void clearBackground();

        PhysicHandler& getPhysicHandler() noexcept;

        _NODISCARD Rendering::Lighting::DirectionalLightBuffer& directionalLightBuffer() noexcept;
        _NODISCARD Rendering::Lighting::PointLightBuffer& pointLightBuffer() noexcept;

        void destroy();
    private:
        _NODISCARD _Ret_notnull_ const char* xmlTypeName() const noexcept final;
        void serializeValues(Text::Xml::XmlSerializeContext& context) const final;

        _NODISCARD _Success_(return) bool deserializeMap(_In_ const Text::Xml::XmlNode::Map& map);

        _NODISCARD bool parseTags(_In_ const Text::Xml::XmlNode::Map& map);

        bool m_started = false;
        SceneRoot m_sceneRoot;

        std::weak_ptr<Camera> m_camera;

        std::vector<std::shared_ptr<UIObject>> m_HUDObject;

        RemoveVector<IWorldUpdatable>  m_updateQueue;
        RemoveVector<IWorldRenderable> m_renderQueue;

        PhysicHandler m_physics;
        Rendering::Color m_clearColor;

        Rendering::Lighting::DirectionalLightBuffer m_directionalLightBuffer;
        Rendering::Lighting::PointLightBuffer m_pointLightBuffer;
    };
}
