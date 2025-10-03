#pragma once

#include <vcruntime.h>

namespace irrklang
{
	class ISoundEngine;
}

namespace KaputEngine::Audio
{
	class AudioEngine
	{
		friend class Sound;

	public:
		AudioEngine();
		AudioEngine(const AudioEngine&) = delete;
		AudioEngine(AudioEngine&&) noexcept = delete;

		~AudioEngine() noexcept;

		void updateTracking();

		void stopAll();

	private:
		_NODISCARD irrklang::ISoundEngine& engine() noexcept;
		_NODISCARD const irrklang::ISoundEngine& engine() const noexcept;

		irrklang::ISoundEngine* m_engine;
	};
}
