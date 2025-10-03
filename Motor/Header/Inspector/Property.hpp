#pragma once

#include "Property.h"

#include "Utils/rawalloc.hpp"

#include <iostream>

#define PROPERTY KaputEngine::Inspector::Property

template <typename Getter, typename T>
PROPERTY::Property(
	std::string name, Getter&& getter) noexcept :
	m_name(name),
	m_typeInfo(typeid(T)),
	m_size(sizeof(T)),
	m_getter([getter = std::move(getter)](void* out) -> void
	{
		new(reinterpret_cast<T*>(out)) T(getter());
	}) { }

template <typename Getter, typename Setter, typename T>
PROPERTY::Property(
	std::string name, Getter&& getter, Setter&& setter) noexcept :
	m_name(name),
	m_typeInfo(typeid(T)),
	m_size(sizeof(T)),
	m_getter([getter = std::move(getter)](void* out) -> void
	{
		new(reinterpret_cast<T*>(out)) T(getter());
	}),
	m_setter([setter = std::move(setter)](const void* value) -> void
	{
		setter(*reinterpret_cast<const T*>(value));
	}) { }

template <typename T>
bool PROPERTY::canRead() const noexcept
{
	if (typeid(T) == m_typeInfo)
		return true;

	if (typeid(T*) == m_typeInfo)
		return true;

	if (typeid(const T*) == m_typeInfo)
		return true;

	return false;
}

template <typename T>
bool PROPERTY::canWrite() const noexcept
{
	if (typeid(T) == m_typeInfo && m_setter.has_value())
		return true;

	if (typeid(const T*) == m_typeInfo && m_setter.has_value())
		return true;

	if (typeid(T*) == m_typeInfo)
		return true;

	return false;
}

template <typename T>
T PROPERTY::get()
{
	if (!this->canRead<T>())
	{
		std::cerr << __FUNCTION__": Unable to read field \"" << name() << "\" as " << typeid(T).name() << ".\n";
		return { };
	}

	if (typeid(T) == type())
	{
		rawalloc<T, false> valueAlloc;
		m_getter(valueAlloc.ptr());

		return valueAlloc.val();
	}

	if (typeid(T*) == type() || typeid(const T*) == type())
	{
		const T* ptr = nullptr;
		m_getter(&ptr);

		return *ptr;
	}

	return { };
}

template <typename T>
void PROPERTY::set(const T& value)
{
	if (!this->canWrite<T>())
	{
		std::cerr << __FUNCTION__": Unable to write field \"" << name() << "\" as " << typeid(T).name() << ".\n";
		return;
	}

	if (typeid(T) == type())
		(*m_setter)(&value);
	else if (typeid(T*) == type())
	{
		T* ptr = nullptr;
		m_getter(&ptr);

		*ptr = value;
	}
	else if (typeid(const T*) == type())
	{
		const T* ptr = nullptr;
		m_getter(&ptr);

		(*m_setter)(ptr);
	}
	else
		std::cerr << __FUNCTION__": Failed to set field \"" << name() << "\" as " << typeid(T).name() << ".\n";
}

#undef PROPERTY_TEMPLATE
#undef PROPERTY
