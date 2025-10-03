#include "Rendering/Shader.h"

#include "Queue/Context.h"

#include "Text/String.h"

#include <iostream>
#include <vector>

using KaputEngine::Queue::ContextQueue;
using KaputEngine::Rendering::Shader;
using KaputEngine::Resource::ShaderResource;
using KaputEngine::Text::StringUtilities;

using std::cerr;
using std::string_view;

Shader::~Shader()
{
	destroy();
}

Shader::Shader(ShaderResource& parent) : m_resource(&parent) { }

void Shader::create(const GLenum type, const string_view& source, const std::stop_token& stopToken)
{
	m_type = type;

	ContextQueue::instance().push([this, &source, &stopToken]
	{
		if (stopToken.stop_requested())
		{
			destroy();
			return;
		}

		m_id = glCreateShader(m_type);

		const char* const data = source.data();

		const GLint length = static_cast<GLint>(source.length());

		glShaderSource(m_id, 1, &data, &length);
		glCompileShader(m_id);

		int status;

		glGetShaderiv(m_id, GL_COMPILE_STATUS, &status);

		if (!status)
		{
			char infoLog[512];
			glGetShaderInfoLog(m_id, 512, nullptr, infoLog);
			cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << '\n';

			std::vector<string_view> lines = StringUtilities::split(source, '\n', true);

			for (size_t i = 0; i < lines.size(); ++i)
				cerr << i + 1 << ": " << lines[i] << '\n';

			destroy();
			return;
		}

		if (stopToken.stop_requested())
		{
			destroy();
			return;
		}
	}).wait();
}

void Shader::destroy()
{
	ContextQueue::instance().push([id = m_id]
	{
		glDeleteShader(id);
	});

	m_id   = 0;
	m_type = 0;
}

const GLuint& Shader::id() const noexcept
{
	return m_id;
}

_Ret_maybenull_ ShaderResource* Shader::parentResource() noexcept
{
	return m_resource;
}
_Ret_maybenull_ const ShaderResource* Shader::parentResource() const noexcept
{
	return m_resource;
}
