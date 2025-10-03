#pragma once


#define VULKAN 0
#define OPENGL 1
#define DIRECTX 2

#define RENDERLIB VULKAN




#include "Component.h"

#include "IWorldRenderable.h"

namespace KaputEngine
{
    namespace Rendering
    {
        class Material;
        class Mesh;
        class ShaderProgram;
    }

    class RenderComponent : public Component, public IWorldRenderable
    {
        COMPONENT_SIGS(RenderComponent, Component)
        OBJECTBASE_DEFAULT_REGISTER(RenderComponent)

    public:
        explicit RenderComponent(GameObject& parent,
            _In_ const std::shared_ptr<const Rendering::Mesh>& mesh,
            _In_ const std::shared_ptr<Rendering::Material>& material);

		explicit RenderComponent(GameObject& parent);

        RenderComponent(GameObject& parent, const Id& id,
            _In_ const std::shared_ptr<const Rendering::Mesh>& mesh,
            _In_ const std::shared_ptr<Rendering::Material>& material);

		RenderComponent(GameObject& parent, const Id& id);

        void destroy() override;

        _NODISCARD bool getCanRender() const noexcept;
        void setCanRender(bool canRender) noexcept;

        void setShaderProgram(const std::shared_ptr<const Rendering::ShaderProgram>& prog);

        void render(const Camera& camera) override;

         _NODISCARD _Ret_maybenull_ std::shared_ptr<const Rendering::Mesh>& mesh() noexcept;
         _NODISCARD _Ret_maybenull_ const std::shared_ptr<const Rendering::Mesh>& mesh() const noexcept;

        _NODISCARD _Ret_maybenull_ std::shared_ptr<Rendering::Material>& material() noexcept;
        _NODISCARD _Ret_maybenull_ const std::shared_ptr<const Rendering::Material> material() const noexcept;

        _NODISCARD std::string getMeshPath()const noexcept;
        _NODISCARD std::string getMatPath() const noexcept;

		void serializeValues(Text::Xml::XmlSerializeContext& context) const override;

        void getProperties(std::vector<Inspector::Property>& out) noexcept override;

    protected:
	    _Success_(return) bool deserializeMap(_In_ const Text::Xml::XmlNode::Map& map) override;

        void registerRender(Scene& scene);
        void unregisterRender();

        void registerQueues(Scene& scene) override;
        void unregisterQueues() override;

        static void defineLuaMembers(sol::usertype<RenderComponent>& type);

		std::shared_ptr<const Rendering::ShaderProgram> m_program;
		std::shared_ptr<const Rendering::Mesh> m_mesh;
		std::shared_ptr<Rendering::Material> m_material;

    private:
        bool m_canRender = true;
    };
}
