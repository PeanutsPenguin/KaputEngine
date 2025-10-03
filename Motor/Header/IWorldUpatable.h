#pragma once

#include "Utils/RemoveVector.h"

namespace KaputEngine
{
    struct IWorldUpdatable : RemoveVectorStatusSource<IWorldUpdatable>
    {
        using UpdateFunc = void(double deltaTime);
        virtual void update(double deltaTime){}
    };
}
