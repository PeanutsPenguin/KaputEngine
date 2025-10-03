#pragma once

namespace KaputEngine::Rendering
{
	struct VertexAttribute
	{
		unsigned int index;

		explicit VertexAttribute() = default;
		explicit VertexAttribute(const unsigned int index) : index(index) { }
	};
}
