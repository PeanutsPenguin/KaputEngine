#version 330 core

#include "Kaput/Shader/Dependency/Vertex.glsl" //! #include "../Dependency/Vertex.glsl"
#include "Kaput/Shader/Dependency/Camera.glsl" //! #include "../Dependency/Camera.glsl"
#include "Kaput/Shader/Dependency/Sampler.glsl" //! #include "../Dependency/Sampler.glsl"

out vec4 albedoData;

void main()
{
    albedoData = pushAlbedo(aAlbedo, aTexCoords);
    gl_Position = cameraTransform(aPos);
}
