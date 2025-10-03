#pragma once

#include "Utils/Pointer.h"
#include "Utils/RemoveVector.h"

#include <LibMath/Vector/Vector3.h>

#include <filesystem>

#include <ik_ISound.h> // Include is required - Forward declare causes error in lib header
#include <ik_ISoundStopEventReceiver.h>

namespace irrklang
{
	class ISoundSource;
}

namespace KaputEngine::Resource
{
	class SoundResource;
}

namespace KaputEngine::Audio
{
	class Sound : private irrklang::ISoundStopEventReceiver
	{
		friend class LiveSound;

	public:
		Sound() = default;
		Sound(Resource::SoundResource& parent);
		Sound(const Sound&) = delete;
		Sound(Sound&&) = delete;

		~Sound() noexcept;

		_NODISCARD _Success_(true) bool create(const std::filesystem::path& path);
		void destroy();

		_NODISCARD bool valid() const noexcept;

		void play(bool loop, float volume = 1.f);
		void play(const LibMath::Vector3f& pos, bool loop, float volume = 1.f);

		_NODISCARD _Ret_maybenull_ Resource::SoundResource* parentResource() noexcept;
		_NODISCARD _Ret_maybenull_ const Resource::SoundResource* parentResource() const noexcept;

	private:
		void OnSoundStopped(irrklang::ISound*, irrklang::E_STOP_EVENT_CAUSE, void* userData) final;

		irrklang::ISoundSource* m_soundSource     = nullptr;
		Resource::SoundResource* m_parentResource = nullptr;

		RemoveVector<LiveSound, std::shared_ptr<LiveSound>> m_liveSounds;
	};
}
