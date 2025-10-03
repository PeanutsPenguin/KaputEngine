#pragma once

#include "Text/Xml/Serializer.h"

#include "Text/Xml/Node.h"
#include "Transform.h"

#include <sal.h>

namespace KaputEngine
{
	class TransformSource : public Text::Xml::IXmlSerializer
	{
	public:
		TransformSource() = default;
		TransformSource(const TransformSource&) = delete;
		TransformSource(TransformSource&&) = delete;

		virtual ~TransformSource() = default;

		_NODISCARD const Transform& getLocalTransform() const noexcept;
		void setLocalTransform(const Transform& transform) noexcept;
		void setLocalPosition(const LibMath::Cartesian3f& position) noexcept;
		void setLocalRotation(const LibMath::Rotorf& rotation) noexcept;
		void setLocalScale(const LibMath::Vector3f& scale) noexcept;

		_NODISCARD const Transform& getWorldTransform() const noexcept;
		_NODISCARD const Transform& getWorldTransformUnsafe() const noexcept;
		virtual void setWorldTransform(const Transform& transform) noexcept;

		_NODISCARD bool isTransformDirty() const noexcept;

		_NODISCARD _Ret_maybenull_ virtual const TransformSource* getParentTransform() const = 0;

		virtual void setTransformDirty() const;

		void reverseCalculateLocal(const Transform& parent);

		void serializeValues(Text::Xml::XmlSerializeContext& context) const override;

		virtual void updateWorldTransform() const noexcept;

	protected:
		bool deserializeMap(const Text::Xml::XmlNode::Map& map);

		mutable bool m_dirtyTransform = true;
		mutable Transform m_worldTransform;
		Transform m_localTransform;
		// ^ mutable: World transform is strictly derived from local and parent transform and not an inherit property
		// Updating this cache is not considered mutating the object
		// Allows updating in getter functions
	};
}
