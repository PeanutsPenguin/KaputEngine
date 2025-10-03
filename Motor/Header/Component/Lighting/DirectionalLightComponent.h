#pragma once

#include "LightComponent.h"

#include "Rendering/Lighting/LightData.h"

namespace KaputEngine
{
	namespace Rendering::Lighting
	{
		class DirectionalLightBuffer;
	}

	class DirectionalLightComponent final : public LightComponent
	{
		COMPONENT_SIGS(DirectionalLightComponent, LightComponent)
		OBJECTBASE_DEFAULT_REGISTER(DirectionalLightComponent)

	public:
		COMPONENT_CTORS(DirectionalLightComponent)

		_NODISCARD const Rendering::Lighting::DirectionalLightData& getData() const noexcept;
		_NODISCARD const Rendering::Lighting::LightData& getBaseData() const noexcept override;

		void setData(const Rendering::Lighting::DirectionalLightData& data) noexcept;

		void serializeValues(Text::Xml::XmlSerializeContext& context) const override;

		void getProperties(std::vector<Inspector::Property>& out) noexcept final;

	protected:
		_NODISCARD Rendering::Lighting::LightBuffer& getSceneBuffer(Scene& scene) const noexcept final;
		_NODISCARD Rendering::Lighting::LightData& getBaseData() noexcept final;

		bool deserializeMap(const Text::Xml::XmlNode::Map& map) final;

	private:
		Rendering::Lighting::DirectionalLightData m_data;
	};
}
