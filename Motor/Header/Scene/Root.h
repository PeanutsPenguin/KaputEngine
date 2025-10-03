#pragma once

#include "GameObject/GameObject.h"

namespace KaputEngine
{
	class SceneRoot final: public GameObject
	{
		friend class Scene;

	public:
		explicit SceneRoot(class Scene& scene);

		static constexpr const char* TypeName = "SceneRoot";

		_NODISCARD constexpr const char* typeName() const noexcept override;

		_NODISCARD _Success_(return) bool deserializeMap(_In_ const Text::Xml::XmlNode::Map& map) final;
	};
}
