#pragma once

#include "Sampler.h"
#include "Utils/Assign.h"

#define TEMPLATE template <typename T>
#define SAMPLER Sampler<T>

namespace KaputEngine::Rendering
{
    TEMPLATE
    SAMPLER::Sampler() noexcept :
        m_fallback(nullptr), m_texture() { }

    TEMPLATE
    SAMPLER::Sampler(nullptr_t) noexcept :
        m_fallback(nullptr), m_texture() { }

    TEMPLATE
    SAMPLER::Sampler(const T& value) noexcept :
        m_fallback(value), m_texture() { }

    TEMPLATE
    SAMPLER::Sampler(VertexAttribute attribute) noexcept:
        m_fallback(attribute), m_texture() { }

    TEMPLATE
    SAMPLER& SAMPLER::operator=(const Sampler& sampler)
    {
        copyAssign(*this, sampler);
        return *this;
    }

    TEMPLATE
    SAMPLER& SAMPLER::operator=(Sampler&& sampler) noexcept
    {
        moveAssign(*this, std::move(sampler));
        return *this;
    }

    TEMPLATE
    T& SAMPLER::global()
    {
        return std::get<T>(m_fallback);
    }

    TEMPLATE
    const T& SAMPLER::global() const
    {
        return const_cast<Sampler*>(this)->global();
    }

    TEMPLATE
    VertexAttribute& SAMPLER::attribute()
    {
        return std::get<VertexAttribute>(m_fallback);
    }

    TEMPLATE
    const VertexAttribute& SAMPLER::attribute() const
    {
        return std::get<VertexAttribute>(m_fallback);
    }

    TEMPLATE
    TextureSample& SAMPLER::texture()
    {
        return m_texture;
    }

    TEMPLATE
    const TextureSample& SAMPLER::texture() const
    {
        return m_texture;
    }

    TEMPLATE
    eSamplerFallback SAMPLER::fallbackMode() const
    {
        return static_cast<eSamplerFallback>(m_fallback.index());
    }

    TEMPLATE
    SAMPLER::FallbackData& SAMPLER::fallbackData()
    {
        return m_fallback;
    }

    TEMPLATE
    const SAMPLER::FallbackData& SAMPLER::fallbackData() const
    {
        return m_fallback;
    }
}

#undef TEMPLATE
#undef SAMPLER
