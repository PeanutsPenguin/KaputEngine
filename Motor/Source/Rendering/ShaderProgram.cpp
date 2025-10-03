#include "Rendering/ShaderProgram.hpp"

#include "GameObject/Camera.h"
#include "Queue/Context.h"
#include "Rendering/Material.hpp"
#include "Rendering/Shader.h"

using namespace KaputEngine;
using namespace KaputEngine::Rendering;

using KaputEngine::Resource::ShaderProgramResource;
using KaputEngine::Queue::ContextQueue;

using LibMath::Vector3f;
using std::cerr;
using std::string;

ShaderProgram::~ShaderProgram()
{
	destroy();
}

ShaderProgram::ShaderProgram(ShaderProgramResource& parent) : m_resource(&parent) { }

void ShaderProgram::create(ShaderList&& shaders)
{
	m_shaders = std::move(shaders);

	ContextQueue::instance().push([this]
	{
		m_id = glCreateProgram();

		for (const auto& shader : m_shaders)
			glAttachShader(m_id, shader->id());

		glLinkProgram(m_id);

		int status;

		glGetProgramiv(m_id, GL_LINK_STATUS, &status);
		if (!status)
		{
			char infoLog[512];
			glGetProgramInfoLog(m_id, 512, nullptr, infoLog);
			cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << '\n';

			destroy();
		}
	}).wait();
}

void ShaderProgram::destroy()
{
	ContextQueue::instance().push([id = m_id]
	{
		glDeleteProgram(id);
	});

	m_id = 0;
}

bool ShaderProgram::use() const
{
	if (!m_id)
		return false;

	ContextQueue::instance().push([this]
	{
		glUseProgram(m_id);
	}).wait();

	return true;
}

void ShaderProgram::unuse() const
{
	ContextQueue::instance().push([]()
	{
		glUseProgram(0);
	}).wait();
}

const ShaderProgram::ShaderList& ShaderProgram::shaders() const noexcept
{
	return m_shaders;
}

unsigned int& ShaderProgram::id() noexcept
{
	return m_id;
}

const unsigned int& ShaderProgram::id() const noexcept
{
	return m_id;
}

#pragma region Uniforms
void ShaderProgram::setUniform(
	const UniformReference& uniform, _In_reads_(count) const Material* const values, const int count) const
{
	int location = getLocation(uniform);

	if (location == -1)
	{
		investigateUniform(uniform, ".albedo.mode");
		return;
	}

	for (int i = 0; i < count; ++i)
	{
		const Material& mat = values[i];

		setUniform(location, mat.albedo());
		setUniform(location += 3, mat.normal());
		setUniform(location += 3, mat.metallic());
		setUniform(location += 3, mat.roughness());
		setUniform(location += 3, mat.ambientOcclusion());
	}
}

void ShaderProgram::setUniform(
	const UniformReference& uniform, _In_reads_(count) const MaterialLayer* const values, const int count) const
{
	int location = getLocation(uniform);

	if (location == -1)
	{
		investigateUniform(uniform, ".albedo.mode");
		return;
	}

	for (int i = 0; i < count; ++i)
	{
		const MaterialLayer& layer = values[i];

		setUniform(location, layer.getSampler<Color>(&Material::albedo));
		setUniform(location += 3, layer.getSampler<Vector3f>(&Material::normal));
		setUniform(location += 3, layer.getSampler<float>(&Material::metallic));
		setUniform(location += 3, layer.getSampler<float>(&Material::roughness));
		setUniform(location += 3, layer.getSampler<float>(&Material::ambientOcclusion));
	}
}

void ShaderProgram::setUniform(
	const UniformReference& uniform, _In_reads_(count) const Camera* const values, const int count) const
{
	int location = getLocation(uniform);

	if (location == -1)
	{
		investigateUniform(uniform, ".position");
		return;
	}

	for (int i = 0; i < count; ++i)
	{
		const Camera& cam = values[i];

		setUniform(location, cam.getWorldTransform().position);
		setUniform(location += 1, cam.getViewProjectionMatrix());
	}
}

_Ret_maybenull_ ShaderProgramResource* ShaderProgram::parentResource() noexcept
{
	return m_resource;
}

_Ret_maybenull_ const ShaderProgramResource* ShaderProgram::parentResource() const noexcept
{
	return m_resource;
}

void ShaderProgram::investigateUniform(const UniformReference& uniform, const string& target) const
{
	if (const char* const* str = std::get_if<const char*>(&uniform); str)
	{
		string name = *str;

		if (!name.ends_with(target))
			cerr << "Shader uniform: \"" << name <<
			"\": Struct or array uniforms set by name must reference the first primitive value. Expected: \"" <<
			target << "\"\n";

		cerr << "Shader uniform \"" << name << "\" not found.\n";
	}
	else
		cerr << "Shader uniform not found. Cannot investigate uniform accessed by location.\n";
}

#define SHADER_UNIFORM_FUNC(type, body) \
void ShaderProgram::setUniform(const UniformReference& uniform, _In_reads_(count) const type* values, GLsizei count) const \
{ \
	int location = getLocation(uniform); \
	if (location == -1) \
		return; \
	ContextQueue::instance().push([location, count, values] { body; }).wait(); \
}

#define SHADER_UNIFORM_FUNC_SCALAR(type, typeSuffix) \
SHADER_UNIFORM_FUNC(ARGS(type), ARGS(glUniform1##typeSuffix##v(location, count, values)))

#define SHADER_UNIFORM_FUNC_ARRAY(type, typeSuffix, length) \
SHADER_UNIFORM_FUNC(ARGS(LibMath::MathArray<1, length, type>), ARGS(glUniform##length####typeSuffix##v(location, count, values->raw())))

#define SHADER_UNIFORM_FUNC_MATRIX(type, sizeName, width, height) \
SHADER_UNIFORM_FUNC(ARGS(LibMath::MathArray<width, height, type>), ARGS(glUniformMatrix##sizeName##v(location, count, GL_FALSE, values->raw())))

#define SHADER_UNIFORM_FUNC_ARRAYS(type, typeSuffix) \
SHADER_UNIFORM_FUNC_ARRAY(ARGS(type), typeSuffix, 1) \
SHADER_UNIFORM_FUNC_ARRAY(ARGS(type), typeSuffix, 2) \
SHADER_UNIFORM_FUNC_ARRAY(ARGS(type), typeSuffix, 3) \
SHADER_UNIFORM_FUNC_ARRAY(ARGS(type), typeSuffix, 4)

#define SHADER_UNIFORM_FUNC_MATRICES(type, typeSuffix) \
SHADER_UNIFORM_FUNC_MATRIX(ARGS(type), 2##typeSuffix, 2, 2) \
SHADER_UNIFORM_FUNC_MATRIX(ARGS(type), 2x3##typeSuffix, 2, 3) \
SHADER_UNIFORM_FUNC_MATRIX(ARGS(type), 2x4##typeSuffix, 2, 4) \
SHADER_UNIFORM_FUNC_MATRIX(ARGS(type), 3x2##typeSuffix, 3, 2) \
SHADER_UNIFORM_FUNC_MATRIX(ARGS(type), 3##typeSuffix, 3, 3) \
SHADER_UNIFORM_FUNC_MATRIX(ARGS(type), 3x4##typeSuffix, 3, 4) \
SHADER_UNIFORM_FUNC_MATRIX(ARGS(type), 4x2##typeSuffix, 4, 2) \
SHADER_UNIFORM_FUNC_MATRIX(ARGS(type), 4x3##typeSuffix, 4, 3) \
SHADER_UNIFORM_FUNC_MATRIX(ARGS(type), 4##typeSuffix, 4, 4)

SHADER_UNIFORM_FUNC_SCALAR(int, i)
SHADER_UNIFORM_FUNC_SCALAR(unsigned int, ui)
SHADER_UNIFORM_FUNC_SCALAR(float, f)
SHADER_UNIFORM_FUNC_SCALAR(double, d)

SHADER_UNIFORM_FUNC_ARRAYS(int, i)
SHADER_UNIFORM_FUNC_ARRAYS(unsigned int, ui)
SHADER_UNIFORM_FUNC_ARRAYS(float, f)
SHADER_UNIFORM_FUNC_ARRAYS(double, d)

SHADER_UNIFORM_FUNC_MATRICES(float, f)
SHADER_UNIFORM_FUNC_MATRICES(double, d)

int ShaderProgram::getLocation(const UniformReference& uniform) const
{
	switch (uniform.index())
	{
	case 0:
		return std::get<int>(uniform);
	case 1:
		return glGetUniformLocation(m_id, std::get<const char*>(uniform));
	default:
		return -1;
	}
}
#pragma endregion
