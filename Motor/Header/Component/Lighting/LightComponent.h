#pragma once

#include "../Component.h"

namespace KaputEngine
{
	namespace Rendering::Lighting
	{
		class LightBuffer;
		struct LightData;
	}

	class LightComponent : public Component
	{
		COMPONENT_SIGS(LightComponent, Component)
		OBJECTBASE_DEFAULT_REGISTER(LightComponent)

	public:
		_NODISCARD const int& index() const noexcept;

		_NODISCARD virtual const Rendering::Lighting::LightData& getBaseData() const noexcept = 0;

		void getProperties(std::vector<Inspector::Property>& out) noexcept override;

	protected:
		int m_index = -1;

		COMPONENT_CTORS(LightComponent)

		_NODISCARD virtual Rendering::Lighting::LightData& getBaseData() noexcept = 0;
		_NODISCARD virtual Rendering::Lighting::LightBuffer& getSceneBuffer(Scene& scene) const noexcept = 0;

		void registerQueues(Scene& scene) final;
		void unregisterQueues() final;

		void updateThisBuffer() const noexcept;

		void serializeValues(Text::Xml::XmlSerializeContext& context) const override;

		bool deserializeMap(const Text::Xml::XmlNode::Map& map) override;

		template <std::derived_from<LightComponent> T>
		static void defineLuaMembers(sol::usertype<T>& type);
	};
}
