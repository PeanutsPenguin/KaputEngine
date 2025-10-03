#pragma once

#include <glad/glad.h>
#include <stop_token>
#include <string_view>

namespace KaputEngine::Resource
{
    class ShaderResource;
}

namespace KaputEngine::Rendering
{
    class Shader
    {
    public:
        Shader() = default;
		Shader(Resource::ShaderResource& parent);
        Shader(const Shader&) = delete;
        Shader(Shader&&) = delete;

        ~Shader();

        Shader& operator=(const Shader&) = delete;
        Shader& operator=(Shader&&) = delete;

        void create(GLenum type, const std::string_view& source = {}, const std::stop_token& stopToken = {});
        void destroy();

        _NODISCARD const GLuint& id() const noexcept;

		_NODISCARD _Ret_maybenull_ Resource::ShaderResource* parentResource() noexcept;
		_NODISCARD _Ret_maybenull_ const Resource::ShaderResource* parentResource() const noexcept;

    protected:
        unsigned int m_id = 0;
        GLenum m_type = GL_INVALID_ENUM;
		Resource::ShaderResource* m_resource = nullptr;
    };
}
