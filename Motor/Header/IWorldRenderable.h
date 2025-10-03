#pragma once

#include "Utils/RemoveVector.h"

namespace KaputEngine
{
    class Camera;

    struct IWorldRenderable : RemoveVectorStatusSource<IWorldRenderable>
    {
        using RenderFunc = void(const Camera&);

        virtual void render(const Camera& camera) {}
    };
}
