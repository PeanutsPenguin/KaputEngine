#include "Component/RenderComponent.h"

#include "Component/Component.hpp"
#include "GameObject/Camera.h"
#include "Rendering/ShaderProgram.hpp"
#include "Resource/Manager.hpp"
#include "Resource/Material.h"
#include "Resource/Mesh.h"
#include "Resource/ShaderProgram.h"
#include "Scene/Scene.h"
#include "Text/Xml/Context.hpp"
#include "Text/Xml/Node.hpp"
#include "Utils/RemoveVector.hpp"

using namespace KaputEngine;
using namespace KaputEngine::Rendering;
using namespace KaputEngine::Resource;
using namespace KaputEngine::Text::Xml;

using KaputEngine::Inspector::Property;

using std::cerr;
using std::string;

COMPONENT_IMPL(RenderComponent)

RenderComponent::RenderComponent(GameObject& parent, const Id& id,
	_In_ const std::shared_ptr<const Mesh>& mesh, _In_ const std::shared_ptr<Material>& material)
	: Component(parent, id), IWorldRenderable(), m_mesh(mesh), m_material(material)
{
	this->m_program = ShaderProgramResource::defaultProgram()->dataPtr();
}

RenderComponent::RenderComponent(GameObject& parent, const Id& id) : Component(parent, id)
{
	this->m_program = ShaderProgramResource::defaultProgram()->dataPtr();
}

RenderComponent::RenderComponent(GameObject& parent,
	_In_ const std::shared_ptr<const Mesh>& mesh, _In_ const std::shared_ptr<Material>& material)
	: RenderComponent(parent, s_nextId++, mesh, material) { }

RenderComponent::RenderComponent(GameObject& parent)
	: RenderComponent(parent, s_nextId++) { }

void RenderComponent::destroy()
{
	unregisterRender();
	Component::destroy();
}

bool RenderComponent::getCanRender() const noexcept
{
	return m_canRender;
}

void RenderComponent::setCanRender(bool canRender) noexcept
{
	if (m_canRender == canRender)
		return;

	m_canRender = canRender;

	if (Scene* scene = parentScene(); scene)
	{
		if (m_canRender)
			registerRender(*scene);
		else
			unregisterRender();
	}
}

void RenderComponent::setShaderProgram(const std::shared_ptr<const ShaderProgram>& prog)
{
	this->m_program = prog;
}

void RenderComponent::render(const Camera& camera)
{
	if (!this->m_program->use())
		return;

	if (!m_mesh)
		return;

	if (Scene* scene = parentScene(); scene)
	{
		scene->directionalLightBuffer().buffer().bind();
		scene->pointLightBuffer().buffer().bind();
	}

	m_program->setUniform("worldPosition", m_parentObject.getWorldTransform().position);
	m_program->setUniform("camera.position", camera);

	if (this->m_material)
		m_mesh->draw(m_parentObject, *m_material, *m_program);
	else
	{
		std::shared_ptr<const MaterialResource> mat = MaterialResource::defaultMaterial();
		m_mesh->draw(m_parentObject, mat->data(), *m_program);
	}

	if (Scene* scene = parentScene(); scene)
	{
		scene->directionalLightBuffer().buffer().unbind();
		scene->pointLightBuffer().buffer().unbind();
	}
}

_Ret_maybenull_ std::shared_ptr<const Mesh>& RenderComponent::mesh() noexcept
{
	return m_mesh;
}

_Ret_maybenull_ const std::shared_ptr<const Mesh>& RenderComponent::mesh() const noexcept
{
	return m_mesh;
}

_Ret_maybenull_ std::shared_ptr<Material>& RenderComponent::material() noexcept
{
	return m_material;
}

_Ret_maybenull_ const std::shared_ptr<const Material> RenderComponent::material() const noexcept
{
	return m_material;
}

string RenderComponent::getMeshPath() const noexcept
{
	const MeshResource* res = m_mesh ? m_mesh->parentResource() : nullptr;

	if (res)
		return res->path()->string();
	else
		return "no mesh file";
}

string RenderComponent::getMatPath() const noexcept
{
	const MaterialResource* res = m_material ? m_material->parentResource() : nullptr;

	if (res)
		return res->path()->string();
	else
		return "no material file";
}

void RenderComponent::serializeValues(XmlSerializeContext& context) const
{
	Component::serializeValues(context);

	context.value("CanRender", m_canRender);

	if (const MeshResource* meshRes = m_mesh ? m_mesh->parentResource() : nullptr)
		context.value("Mesh", *meshRes->path());
	if (const MaterialResource* matRes = m_material ? m_material->parentResource() : nullptr)
		context.value("Material", *matRes->path());
	if (const ShaderProgramResource* progRes = m_program ? m_program->parentResource() : nullptr)
		context.value("ShaderProgram", *progRes->path());
}

_Success_(return) bool RenderComponent::deserializeMap(_In_ const XmlNode::Map& map)
{
	if (!Component::deserializeMap(map))
		return false;

	if (const auto it = map.find("CanRender"); it != map.end())
	{
		const std::optional<bool> op = it->second->parse<bool>();

		if (!op)
		{
			cerr << __FUNCTION__ ": Failed to parse CanRender.\n";
			return false;
		}

		m_canRender = *op;
	}
	else
		m_canRender = false;

	if (const auto it = map.find("Mesh"); it != map.end())
	{
		const std::optional<string> sourceOp = it->second->parse<string>();

		if (!sourceOp)
		{
			cerr << __FUNCTION__ ": Failed to parse Mesh.\n";
			return false;
		}

		m_mesh = ResourceManager::get<MeshResource>(*sourceOp)->root();
	}

	if (const auto it = map.find("Material"); it != map.end())
	{
		const std::optional<string> sourceOp = it->second->parse<string>();

		if (!sourceOp)
		{
			cerr << __FUNCTION__ ": Failed to parse Material.\n";
			return false;
		}

		m_material = ResourceManager::get<MaterialResource>(*sourceOp)->dataPtr();
	}

	if (const auto it = map.find("ShaderProgram"); it != map.end())
	{
		const std::optional<string> sourceOp = it->second->parse<string>();

		if (!sourceOp)
		{
			cerr << __FUNCTION__": Failed to parse ShaderProgram.\n";
			return false;
		}

		m_program = ResourceManager::get<ShaderProgramResource>(*sourceOp)->dataPtr();
	}

	return true;
}

void RenderComponent::defineLuaMembers(sol::usertype<RenderComponent>& type)
{
	Component::defineLuaMembers(type);

	type["canRender"] = sol::property(&getCanRender, &setCanRender);
}

void RenderComponent::registerRender(Scene& scene)
{
	scene.m_renderQueue.push_back(*this);
}

void RenderComponent::unregisterRender()
{
	IWorldRenderable::m_status.castVector<IWorldRenderable>()->erase(*this);
}

void RenderComponent::registerQueues(Scene& scene)
{
	Component::registerQueues(scene);

	if (m_canRender)
		registerRender(scene);
}

void RenderComponent::unregisterQueues()
{
	Component::unregisterQueues();
	unregisterRender();
}

void RenderComponent::getProperties(std::vector<Property>& out) noexcept
{
	Base::getProperties(out);

	// TODO Potentially block in playing
	out.emplace_back("Can Render",
	[this]() { return getCanRender(); },
	[this](const bool& value) { setCanRender(value); });
}
