#pragma once

#include "GameObject.h"
#include "Window/WindowConfig.h"

#include <LibMath/Matrix.h>
#include <LibMath/Vector/Vector3.h>

#include <sol/sol.hpp>
#include <vcruntime.h>

namespace KaputEngine
{
    class Camera : public GameObject
    {
        GAMEOBJECT_SIGS(Camera, GameObject)
        OBJECTBASE_DEFAULT_REGISTER(Camera)

    public:
        _NODISCARD const LibMath::Vector3f& getFront() const noexcept;

        _NODISCARD const LibMath::Matrix4f& getViewProjectionMatrix() const noexcept;

		void setAspect(float aspect);

        void setYaw(float yaw);

        void setPitch(float pitch);

        void setFront(const LibMath::Vector3f& front);

        _NODISCARD float getYaw() const noexcept;

        _NODISCARD float getPitch() const noexcept;

        _NODISCARD const LibMath::Vector3f& getRight() const noexcept;

        _NODISCARD LibMath::Matrix4f getViewMatrix() const noexcept;
        _NODISCARD LibMath::Matrix4f getProjectionMatrix() const noexcept;

        void getProperties(std::vector<Inspector::Property>& out) noexcept override;

    protected:
        static void defineLuaMembers(sol::usertype<Camera>& type);

    private:
        mutable LibMath::Vector3f
            m_front,
            m_up,
            m_right,
            m_worldUp;

        float
            m_yaw = 0.f,
            m_pitch = 0.f,
			m_aspect = WindowConfig::DEFAULT_WIDTH / WindowConfig::DEFAULT_HEIGHT;

        mutable LibMath::Matrix4f m_viewProjection;

        void updateWorldTransform() const noexcept final;
    };
}
