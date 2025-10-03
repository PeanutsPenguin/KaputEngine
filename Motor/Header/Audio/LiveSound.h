#pragma once

#include "Utils/Pointer.h"
#include "Utils/RemoveVector.h"

namespace irrklang
{
    class ISound;
}

namespace KaputEngine::Audio
{
    class Sound;

    class LiveSound : public RemoveVectorStatusSource<LiveSound>
    {
        friend class AudioEngine;

        struct ConstructorBlocker
        {
            explicit ConstructorBlocker() = default;
        };

    public:
        LiveSound(ConstructorBlocker, Sound& source, irrklang::ISound& sound);
        LiveSound(const LiveSound&) = delete;
        LiveSound(LiveSound&&) = delete;
        ~LiveSound();

        static _Ret_maybenull_ std::shared_ptr<LiveSound> create(
            Sound& source, _In_opt_ irrklang::ISound* sound, float volume);

        void stop();

        _NODISCARD exclusive_weak_ptr<LiveSound> exclusive_weak_from_this() noexcept;
        _NODISCARD exclusive_weak_ptr<const LiveSound> exclusive_weak_from_this() const noexcept;

    private:
        irrklang::ISound& m_sound;
        exclusive_weak_ptr<LiveSound> m_ptr;
        Sound& m_source;
    };
}
