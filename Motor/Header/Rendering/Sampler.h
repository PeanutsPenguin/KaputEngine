#pragma once

#include "TextureSample.h"
#include "VertexAttribute.h"

#include <variant>
#include <vcruntime.h>

namespace KaputEngine::Rendering
{
    namespace Buffer
    {
        class TextureBuffer;
    }

    enum class eSamplerFallback : uint8_t
    {
        LAYER,
        GLOBAL,
        ATTRIBUTE,
    };

    template <typename T>
	class Sampler
    {
    public:
        using FallbackData = std::variant<nullptr_t, T, VertexAttribute>;

        Sampler() noexcept;
        Sampler(nullptr_t) noexcept;
        Sampler(const T& value) noexcept;
        Sampler(VertexAttribute attribute) noexcept;

        Sampler(const Sampler&) = default;
        Sampler(Sampler&&) noexcept = default;

        ~Sampler() = default;

        Sampler& operator=(const Sampler& sampler);
        Sampler& operator=(Sampler&& sampler) noexcept;

        _NODISCARD T& global();
        _NODISCARD const T& global() const;

        _NODISCARD VertexAttribute& attribute();
        _NODISCARD const VertexAttribute& attribute() const;

        _NODISCARD TextureSample& texture();

        _NODISCARD const TextureSample& texture() const;

        _NODISCARD eSamplerFallback fallbackMode() const;

        _NODISCARD FallbackData& fallbackData();
        _NODISCARD const FallbackData& fallbackData() const;

    private:
        FallbackData m_fallback;
        TextureSample m_texture;
    };

    template <typename T>
    struct SamplerLayer
    {
        const Sampler<T>& primary;
        _Maybenull_ const Sampler<T>* fallback = nullptr;
    };
}

#include "Sampler.hpp"
