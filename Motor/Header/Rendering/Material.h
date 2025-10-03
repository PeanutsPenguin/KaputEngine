#pragma once

#include "Color.h"
#include "Rendering/Sampler.h"

#include <LibMath/Vector/Vector3.h>

namespace KaputEngine::Resource
{
	class MaterialResource;
}

class aiMaterial;

namespace KaputEngine::Rendering
{
	class Material
	{
	public:
		Material() = default;
		Material(Resource::MaterialResource& parent);
		Material(const Material&) = delete;
		Material(Material&&) noexcept = default;

		~Material() = default;

		_NODISCARD Material& operator=(Material&&) = default;

		_NODISCARD _Success_(return) bool init(aiMaterial&& material);

		_NODISCARD Sampler<Color>& albedo() noexcept;
		_NODISCARD const Sampler<Color>& albedo() const noexcept;
		void setAlbedo(const Color& col);

		_NODISCARD Sampler<LibMath::Vector3f>& normal() noexcept;
		_NODISCARD const Sampler<LibMath::Vector3f>& normal() const noexcept;

		_NODISCARD Sampler<float>& metallic() noexcept;
		_NODISCARD const Sampler<float>& metallic() const noexcept;
		void setMetallic(float metal);

		_NODISCARD Sampler<float>& roughness() noexcept;
		_NODISCARD const Sampler<float>& roughness() const noexcept;
		void setRoughness(float rough);

		_NODISCARD Sampler<float>& ambientOcclusion() noexcept;
		_NODISCARD const Sampler<float>& ambientOcclusion() const noexcept;
		void setOcclusion(float occl);

		_NODISCARD _Ret_maybenull_ Resource::MaterialResource* parentResource() noexcept;
		_NODISCARD _Ret_maybenull_ const Resource::MaterialResource* parentResource() const noexcept;

	private:
		Sampler<Color> m_albedo;
		Sampler<LibMath::Vector3f> m_normal;
		Sampler<float> m_metallic;
		Sampler<float> m_roughness;
		Sampler<float> m_ambientOcclusion;

		Resource::MaterialResource* m_resource = nullptr;
	};

	struct MaterialLayer
	{
		const Material& primary;
		_Maybenull_ const Material* fallback = nullptr;

		/// <summary>
		/// Generates a sampler layer for a given sampler
		/// </summary>
		/// <param name="sampler">Function pointer of the target sampler from Material. Ex: &amp;Material::albedo</param>
		template <typename T>
		_NODISCARD SamplerLayer<T> getSampler(const Sampler<T>& (Material::* sampler)() const) const noexcept;
	};
}
