#pragma once

#include "Utils/RemoveVector.h"

namespace KaputEngine
{
    struct IRenderable : RemoveVectorStatusSource<IRenderable>
    {
        using RenderFunc = void();

        IRenderable& operator=(const IRenderable&) = delete;
        IRenderable& operator=(IRenderable&&) = delete;

        virtual void render() {}
    };
}
