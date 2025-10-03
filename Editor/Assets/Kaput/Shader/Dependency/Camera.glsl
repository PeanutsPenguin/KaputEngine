struct Camera
{
	vec3 position;
	mat4 viewProjection;
};

vec4 cameraTransform(Camera camera, mat4 model, vec3 vertPos)
{
	return camera.viewProjection * model * vec4(vertPos, 1.0);
}

vec4 cameraTransformWorld(Camera camera, vec3 worldPos)
{
	return camera.viewProjection * vec4(worldPos, 1.0);
}
