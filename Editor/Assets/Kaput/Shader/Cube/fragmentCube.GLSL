#version 330 core

#include "Kaput/Shader/Dependency/Albedo.glsl" //! #include "../Dependency/Albedo.glsl"

in vec4 albedoData;
out vec4 FragColor;

void main()
{
    FragColor = pullAlbedo(albedoData);
}
