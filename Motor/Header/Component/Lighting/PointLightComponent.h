#pragma once

#include "LightComponent.h"
#include "Scene/Transform/Source.h"

#include "Rendering/Lighting/LightData.h"

namespace KaputEngine
{
	namespace Rendering::Lighting
	{
		class PointLightBuffer;
	}

	class PointLightComponent : public LightComponent, public TransformSource
	{
		COMPONENT_SIGS(PointLightComponent, LightComponent)
		OBJECTBASE_DEFAULT_REGISTER(PointLightComponent)

	public:
		PointLightComponent(GameObject& parent);
		PointLightComponent(GameObject& parent, const Id& id);

		PointLightComponent(const PointLightComponent&) = delete;
		PointLightComponent(PointLightComponent&&) = delete;

		_NODISCARD const Rendering::Lighting::PointLightData& getData() const noexcept;
		_NODISCARD const Rendering::Lighting::LightData& getBaseData() const noexcept override;

		void setData(const Rendering::Lighting::PointLightData& data) noexcept;

		_NODISCARD _Ret_notnull_ TransformSource* getParentTransform() const final;

		void updateWorldTransform() const noexcept final;
		void setTransformDirty() const final;
		void setWorldTransform(const Transform& transform) noexcept final;

		void update(double deltaTime) override;

		void getProperties(std::vector<Inspector::Property>& out) noexcept override;

	protected:
		_NODISCARD Rendering::Lighting::LightData& getBaseData() noexcept final;

		_NODISCARD bool deserializeMap(const Text::Xml::XmlNode::Map& map) final;

	private:
		Rendering::Lighting::PointLightData m_data;

		_NODISCARD Rendering::Lighting::LightBuffer& getSceneBuffer(Scene& scene) const noexcept final;
	};
}
