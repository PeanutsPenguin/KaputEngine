#include "Application.h"
#include "Inspector/Property.hpp"
#include "Scene/Scene.h"
#include "Text/Xml/Context.hpp"
#include "Text/Xml/Node.hpp"
#include "Utils/RemoveVector.hpp"

using namespace KaputEngine;
using namespace KaputEngine::Text;
using namespace KaputEngine::Text::Xml;

using KaputEngine::Inspector::Property;

using std::cerr;
using std::nullopt;
using std::string;

OBJECTBASE_IMPL(ObjectBase);

// 0 reserved for no Id
Id ObjectBase::s_nextId = 1;

ObjectBase::ObjectBase() : ObjectBase(s_nextId++) { }

ObjectBase::ObjectBase(const Id& id)
	: m_id(id) { }

bool ObjectBase::getCanUpdate() const noexcept
{
	return m_canUpdate;
}

void ObjectBase::setCanUpdate(const bool canUpdate) noexcept
{
	if (canUpdate == m_canUpdate)
		return;

	m_canUpdate = canUpdate;

	if (Scene* scene = parentScene())
	{
		if (m_canUpdate)
			registerUpdate(*scene);
		else
			unregisterUpdate();
	}
}

const Id& ObjectBase::id() const noexcept
{
	return m_id;
}

void ObjectBase::rename(_In_ const char* name)
{
	this->name = name;
}

void ObjectBase::start()
{
	if (validateStart())
		m_started = true;
}

void ObjectBase::destroy()
{
	if (parentScene())
		unregisterUpdate();
}

void ObjectBase::registerQueues(Scene& scene)
{
	registerUpdate(scene);
}

void ObjectBase::unregisterQueues()
{
	if (parentScene())
		unregisterUpdate();
}

sol::state& ObjectBase::getAppLuaState() noexcept
{
	return Application::luaState();
}

_Success_(return) bool ObjectBase::deserializeMap(const XmlNode::Map& map)
{
	// Id parsing is the responsibility of GameObject or Component as it's needed for their creation

	if (const auto it = map.find("CanUpdate"); it != map.end())
	{
		std::optional<bool> op = it->second->parse<bool>();

		if (!op.has_value())
		{
			cerr << __FUNCTION__": Failed to parse CanUpdate.\n";
			return false;
		}

		m_canUpdate = *op;
	}
	else
		m_canUpdate = false;

	if (const auto it = map.find("Name"); it != map.end())
	{
		std::optional<string> op = it->second->parse<string>();

		if (!op.has_value())
		{
			cerr << __FUNCTION__": Failed to parse Name.\n";
			return false;
		}

		name = *op;
	}

	return true;
}

bool ObjectBase::validateStart() const noexcept
{
	if (m_started)
	{
		cerr << typeName() << ": Already started.\n";
		return false;
	}

	if (!parentScene())
	{
		cerr << typeName() << ": Must be part of a scene to start.\n";
		return false;
	}

	return true;
}

_Success_(return) std::optional<Id> ObjectBase::deserializeId(const XmlNode::Map& map)
{
	const auto it = map.find("Id");

	if (it == map.end())
	{
		deserializeErrorStatic(__FUNCTION__, "Id");
		return std::nullopt;
	}

	std::optional<Id> idOp = it->second->parse<Id>();

	if (!idOp)
	{
		cerr << __FUNCTION__": Failed to deserialize Id.\n";
		return nullopt;
	}

	if (!*idOp)
	{
		cerr << __FUNCTION__": Id 0 is invalid.\n";
		return nullopt;
	}

	// Avoid runtime objects using existing deserialized ids
	if (*idOp >= s_nextId)
		s_nextId = *idOp + 1;

	return *idOp;
}

void ObjectBase::deserializeError(_In_ const char* context, _In_opt_ const char* value) const noexcept
{
	std::string outValue = value ? value : typeName();

	if (!value && m_id)
		outValue += std::format(" {}", m_id);

	deserializeErrorStatic(context, outValue.c_str());
}

void ObjectBase::deserializeErrorStatic(_In_ const char* context, _In_ const char* value)
{
	cerr << context << ": Failed to deserialize " << value << ".\n";
}

void ObjectBase::registerUpdate(Scene& scene)
{
	scene.m_updateQueue.push_back(this);
}

void ObjectBase::unregisterUpdate()
{
	m_status.castVector<IWorldUpdatable>()->erase(*this);
}

void ObjectBase::getProperties(std::vector<Property>& out) noexcept
{
#ifdef REFLECT_DUMMIES
	static float dummyFloat;
	static bool dummyBool;
	static LibMath::Vector3f dummyVec3;
	static Rendering::Color dummyColor;

	out.emplace_back("Dummy global float", []() -> float*
	{
		return &dummyFloat;
	});

	out.emplace_back("Dummy global bool", []() -> bool*
	{
		return &dummyBool;
	});

	out.emplace_back("Dummy global Vec3", []() -> LibMath::Vector3f*
	{
		return &dummyVec3;
	});

	out.emplace_back("Dummy global color", []() -> Rendering::Color*
	{
		return &dummyColor;
	});
#endif

	out.emplace_back("Id", [this]() -> Id { return m_id; });

	out.emplace_back("Name",
	[this]() { return name; },
	[this](const string& value)
	{
		// For some reason, operator=(const string&) does nothing if the new string is larger
		name = value.c_str();
	});

	out.emplace_back("Can Update",
	[this]() -> bool { return getCanUpdate(); },
	[this](const bool& value) { setCanUpdate(value); });
}

const char* ObjectBase::xmlTypeName() const noexcept
{
	return typeName();
}

void ObjectBase::serializeValues(XmlSerializeContext& context) const
{
	context.value("Id", m_id);
	context.value("CanUpdate", m_canUpdate);

	if (!name.empty())
		context.value("Name", name);
}
