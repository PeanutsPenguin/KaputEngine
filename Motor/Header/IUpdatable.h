#pragma once

#include "Utils/RemoveVector.h"

namespace KaputEngine
{
    struct IUpdatable : RemoveVectorStatusSource<IUpdatable>
    {
        using UpdateFunc = void();
        virtual void update(){}
    };
}
