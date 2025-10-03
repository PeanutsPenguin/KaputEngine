#pragma once

#include <sal.h>
#include <variant>
#include <vector>

#include <LibMath/MathArray/MathArray.h>

namespace KaputEngine
{
	class Camera;

	namespace Resource
	{
		class ShaderProgramResource;
	}
}

namespace KaputEngine::Rendering
{
	template <typename T>
	class Sampler;

	template <typename T>
	class SamplerLayer;

	using UniformReference = std::variant<int, const char*>;

	class ShaderProgram
	{
	public:
		using ShaderList = std::vector<std::shared_ptr<const class Shader>>;

		ShaderProgram() = default;
		ShaderProgram(Resource::ShaderProgramResource& parent);
		ShaderProgram(const ShaderProgram&) = delete;
		ShaderProgram(ShaderProgram&&) = delete;

		virtual ~ShaderProgram();

		void create(ShaderList&& shaders);
		void destroy();

		_NODISCARD bool use() const;
		void unuse() const;

		_NODISCARD const ShaderList& shaders() const noexcept;

		_NODISCARD unsigned int& id() noexcept;
		_NODISCARD const unsigned int& id() const noexcept;

#pragma region Uniforms
#pragma region Overloads
#define SHADER_UNIFORM_SIG(...) \
void setUniform(const UniformReference& uniform, _In_reads_(count) const __VA_ARGS__* values, int count) const;

#define SHADER_UNIFORM_SIG_MATRIX(type, width, height) SHADER_UNIFORM_SIG(LibMath::MathArray<width, height, type>)
#define SHADER_UNIFORM_SIG_ARRAY(type, length) SHADER_UNIFORM_SIG_MATRIX(type, 1, length)

#define SHADER_UNIFORM_SIG_ARRAYS(type) \
SHADER_UNIFORM_SIG_ARRAY(type, 1) \
SHADER_UNIFORM_SIG_ARRAY(type, 2) \
SHADER_UNIFORM_SIG_ARRAY(type, 3) \
SHADER_UNIFORM_SIG_ARRAY(type, 4)

#define SHADER_UNIFORM_SIG_MATRICES(type) \
SHADER_UNIFORM_SIG_MATRIX(type, 2, 2) \
SHADER_UNIFORM_SIG_MATRIX(type, 2, 3) \
SHADER_UNIFORM_SIG_MATRIX(type, 2, 4) \
SHADER_UNIFORM_SIG_MATRIX(type, 3, 2) \
SHADER_UNIFORM_SIG_MATRIX(type, 3, 3) \
SHADER_UNIFORM_SIG_MATRIX(type, 3, 4) \
SHADER_UNIFORM_SIG_MATRIX(type, 4, 2) \
SHADER_UNIFORM_SIG_MATRIX(type, 4, 3) \
SHADER_UNIFORM_SIG_MATRIX(type, 4, 4)

		SHADER_UNIFORM_SIG(int)
		SHADER_UNIFORM_SIG(unsigned int)
		SHADER_UNIFORM_SIG(float)
		SHADER_UNIFORM_SIG(double)
		SHADER_UNIFORM_SIG(bool)

		SHADER_UNIFORM_SIG_ARRAYS(int)
		SHADER_UNIFORM_SIG_ARRAYS(unsigned int)
		SHADER_UNIFORM_SIG_ARRAYS(float)
		SHADER_UNIFORM_SIG_ARRAYS(double)

		SHADER_UNIFORM_SIG_MATRICES(float)
		SHADER_UNIFORM_SIG_MATRICES(double)
#pragma endregion

		template <typename T>
		void setUniform(const UniformReference& uniform, const T& value) const;

		template <typename T>
		void setUniform(const UniformReference& uniform, _In_reads_(count) const Sampler<T>* values, int count) const;

		template <typename T>
		void setUniform(const UniformReference& uniform, _In_reads_(count) const SamplerLayer<T>* values, int count) const;

		void setUniform(const UniformReference& uniform, _In_reads_(count) const class Material* values, int count) const;
		void setUniform(const UniformReference& uniform, _In_reads_(count) const struct MaterialLayer* values, int count) const;

		void setUniform(const UniformReference& uniform, _In_reads_(count) const Camera* values, int count) const;

		_NODISCARD _Ret_maybenull_ Resource::ShaderProgramResource* parentResource() noexcept;
		_NODISCARD _Ret_maybenull_ const Resource::ShaderProgramResource* parentResource() const noexcept;
#pragma endregion

	private:
		unsigned int m_id = 0;
		ShaderList m_shaders;
		Resource::ShaderProgramResource* m_resource = nullptr;

		_NODISCARD int getLocation(const UniformReference& uniform) const;

		void investigateUniform(const UniformReference& uniform, const std::string& target) const;
	};
}
