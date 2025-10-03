#pragma once

#include "Color.h"

#include <LibMath/Point/Cartesian.h>
#include <LibMath/Vector/Vector3.h>

namespace KaputEngine::Rendering
{
	struct Vertex
	{
		Color albedo;
		LibMath::Cartesian3f position;
		LibMath::Cartesian2f textureUV;
		LibMath::Vector3f normal;
		LibMath::Vector3f tangent;
		LibMath::Vector3f bitangent;
	};
}
