#pragma once

#include "Utils/RemoveVector.h"

#define VECTOR_TEMPLATE \
template <typename T, IsPointer<T> Ptr, bool FuncsFromPtr> \
requires (std::is_class_v<T> || (FuncsFromPtr && std::is_class_v<Ptr>))

#define REMOVEVECTOR RemoveVector<T, Ptr, FuncsFromPtr>
#define ITERATOR RemoveVector<T, Ptr, FuncsFromPtr>::iterator

#define REMOVEVECTORPOINTER RemoveVectorPointer<T, Pointer>
#define REMOVEVECTORPOINTER_TEMPLATE template <typename T, IsPointer<T> Pointer>

#define REMOVABLEWRAPPER RemovableWrapper<T>
#define REMOVABLE_WRAPPER_TEMPLATE template <typename T>

#define WRAPPEDREMOVEVECTOR WrappedRemoveVector<T, Pointer>
#define WRAPPEDREMOVEVECTOR_TEMPLATE template <typename T, IsPointer<T> Pointer>

namespace KaputEngine
{
#pragma region RemoveVectorStatus
	template <typename T, IsPointer<T> Pointer, bool FuncsFromPtr>
	_Ret_maybenull_ RemoveVector<T, Pointer, FuncsFromPtr>* RemoveVectorStatus::castVector() noexcept
	{
		return static_cast<RemoveVector<T, Pointer, FuncsFromPtr>*>(vector);
	}

	template <typename T, IsPointer<T> Pointer, bool FuncsFromPtr>
	_Ret_maybenull_ const RemoveVector<T, Pointer, FuncsFromPtr>* RemoveVectorStatus::castVector() const noexcept
	{
		return static_cast<const RemoveVector<T, Pointer, FuncsFromPtr>*>(vector);
	}
#pragma endregion

#pragma region RemoveVectorStatusSource
	template <typename T>
	template <IsPointer<T> Pointer, bool FuncsFromPtr>
	RemoveVector<T, Pointer, FuncsFromPtr> RemoveVectorStatusSource<T>::createRemoveVector() noexcept
	{
		return RemoveVector<T, Pointer, FuncsFromPtr>(&RemoveVectorStatusSource::getStatus);
	}

	template <typename T>
	RemoveVectorStatus& RemoveVectorStatusSource<T>::getStatus() const noexcept
	{
		return m_status;
	}
#pragma endregion

#pragma region iterator
	VECTOR_TEMPLATE
	ITERATOR::iterator(const RemoveVectorStatus& status)
		: m_status(status)
	{
		//_ASSERT(status.vector == nullptr || status.index() < status.castVector<RemoveVector>()->m_vector.size());
	}

	VECTOR_TEMPLATE
	ITERATOR::iterator(_In_ const RemoveVector* vec, const size_t index) noexcept
		: iterator(RemoveVectorStatus { .vector = const_cast<RemoveVector*>(vec), .index = index }) { }

	VECTOR_TEMPLATE
	T& ITERATOR::operator*() noexcept
	{
		return *pointer();
	}

	VECTOR_TEMPLATE
	T* ITERATOR::operator->() noexcept
	{
		return pointer();
	}

	VECTOR_TEMPLATE
	ITERATOR& ITERATOR::operator++() noexcept
	{
		++m_status.index;
		update();

		return *this;
	}

	VECTOR_TEMPLATE
	ITERATOR ITERATOR::operator++(int) noexcept
	{
		iterator copy = *this;
		++*this;

		return copy;
	}

	VECTOR_TEMPLATE
	ITERATOR ITERATOR::operator+(const int offset) noexcept
	{
		return { vector(), index() + offset };
	}

	VECTOR_TEMPLATE
	ITERATOR ITERATOR::operator-(int offset) noexcept
	{
		return { vector(), index() + offset };
	}

	VECTOR_TEMPLATE
	size_t ITERATOR::operator+(const iterator& other) noexcept
	{
		return index() + other.m_ptr;
	}

	VECTOR_TEMPLATE
	size_t ITERATOR::operator-(const iterator& other) noexcept
	{
		return index() - other.m_ptr;
	}

	VECTOR_TEMPLATE
	bool ITERATOR::operator!=(const iterator& other) const noexcept
	{
		return vector() != other.vector() || index() != other.index();
	}

#define REMOVE_VECTOR_ITERATOR_COMPARISON_IMPL(op) \
VECTOR_TEMPLATE bool ITERATOR::operator op(const iterator& other) const noexcept \
{ return m_status.vector == other.m_status.vector && m_status.index op other.m_status.index; }

	REMOVE_VECTOR_ITERATOR_COMPARISON_IMPL(==)
	REMOVE_VECTOR_ITERATOR_COMPARISON_IMPL(<)
	REMOVE_VECTOR_ITERATOR_COMPARISON_IMPL(>)
	REMOVE_VECTOR_ITERATOR_COMPARISON_IMPL(<=)
	REMOVE_VECTOR_ITERATOR_COMPARISON_IMPL(>=)

#undef REMOVE_VECTOR_ITERATOR_COMPARISON_IMPL

	VECTOR_TEMPLATE bool ITERATOR::operator==(const std::default_sentinel_t&) const noexcept
	{
		return !vector() || index() >= vector()->m_vector.size();
	}

	VECTOR_TEMPLATE
	bool ITERATOR::operator!=(const std::default_sentinel_t&) const noexcept
	{
		return vector() && index() < vector()->m_vector.size();
	}

	VECTOR_TEMPLATE
	RemoveVectorStatus& ITERATOR::status() noexcept
	{
		return m_status;
	}

	VECTOR_TEMPLATE
	const RemoveVectorStatus& ITERATOR::status() const noexcept
	{
		return m_status;
	}

	VECTOR_TEMPLATE
	size_t ITERATOR::index() const noexcept
	{
		return m_status.index;
	}

	VECTOR_TEMPLATE
	_Ret_maybenull_ REMOVEVECTOR* ITERATOR::vector() noexcept
	{
		return m_status.castVector<T, Pointer, FuncsFromPointer>();
	}

	VECTOR_TEMPLATE
	_Ret_maybenull_ const REMOVEVECTOR* ITERATOR::vector() const noexcept
	{
		return const_cast<iterator*>(this)->vector();
	}

	VECTOR_TEMPLATE
	Ptr& ITERATOR::pointer()
	{
		return vector()->m_vector[m_status.index];
	}

	VECTOR_TEMPLATE
	void ITERATOR::update() noexcept
	{
		if (m_status.index >= vector()->m_vector.size())
			return;

		Ptr& ptr = pointer();

		if (ptr != nullptr)
			return;

		// Pop until a non-null item is found
		// Could be optimized by tracking the ending null segment and popping it all at once (Check if std::vector has such a function)
		while (!vector()->m_vector.empty() && vector()->m_vector.back() == nullptr)
			vector()->m_vector.pop_back();

		if (index() >= vector()->m_vector.size())
			// No swap candidate found
			// Iterator is now on end - Will exit the loop
			return;

		ptr = std::move(vector()->m_vector.back());
		std::invoke(vector()->m_getStatusFunc, getFuncSource(ptr)).index = index();
		vector()->m_vector.pop_back();
	}
#pragma endregion

#pragma region RemoveVector
	VECTOR_TEMPLATE
	REMOVEVECTOR::RemoveVector(_In_ const GetStatusFunc getStatus) :
		m_getStatusFunc(getStatus),
		m_size(0) { }

	VECTOR_TEMPLATE
	Ptr& REMOVEVECTOR::push_back(const Ptr& ptr)
		requires (!FuncsFromPtr && std::copy_constructible<Pointer>)
	{
		return push_back(Ptr(ptr));
	}

	VECTOR_TEMPLATE
	Ptr& REMOVEVECTOR::push_back(Ptr&& ptr)
	{
		_ASSERT(ptr != nullptr);

		Ptr& emplacedPtr = m_vector.emplace_back(std::move(ptr));

		RemoveVectorStatus& status = std::invoke(m_getStatusFunc, getFuncSource(emplacedPtr));
		_ASSERT(status.vector == nullptr);

		status = { .vector = this, .index = m_vector.size() - 1 };

		++m_size;
		return emplacedPtr;
	}

	VECTOR_TEMPLATE
	T* REMOVEVECTOR::push_back(T& item)
		requires (std::same_as<Ptr, T*>)
	{
		return push_back(&item);
	}

	VECTOR_TEMPLATE
	bool REMOVEVECTOR::erase(iterator it)
	{
		Ptr& pointer = it.pointer();

		_ASSERT(it.vector() == nullptr || it.vector() == this);

		// Check index is in bounds and not a removed item
		if (it.vector() == nullptr || it.index() > m_vector.size())
			return false;

		// If null, keep the null pointer to not break ongoing iterations
		if (pointer == nullptr)
			return false;

		std::invoke(m_getStatusFunc, getFuncSource(pointer)) = { };
		pointer = nullptr;

		--m_size;

		return true;
	}

	VECTOR_TEMPLATE
	bool REMOVEVECTOR::erase(FuncSource& source)
	{
		if (m_getStatusFunc != nullptr)
			return erase({ std::invoke(m_getStatusFunc, source) });

		// No function provided - Manually search for the pointer
		auto vectorIt = std::ranges::find_if(m_vector, [&source](const Ptr& ptr) -> bool
		{
			// Compare the raw pointers
			return std::to_address(ptr) == &getFuncSourceItem(source);
		});

		return erase({ this, static_cast<size_t>(std::to_address(vectorIt) - m_vector.data()) });
	}

	VECTOR_TEMPLATE
	void REMOVEVECTOR::reserve(const size_t size) noexcept
	{
		m_vector.reserve(size);
	}

	VECTOR_TEMPLATE
	_Ret_maybenull_ Ptr& REMOVEVECTOR::operator[](size_t index) noexcept
	{
		if (index >= m_vector.size())
			return nullptr;

		return &m_vector[index];
	}

	VECTOR_TEMPLATE
	_Ret_maybenull_ const Ptr& REMOVEVECTOR::operator[](size_t index) const noexcept
	{
		return const_cast<RemoveVector*>(this)->operator[](index);
	}

	VECTOR_TEMPLATE
	bool REMOVEVECTOR::erase(T& item)
		requires (FuncsFromPtr)
	{
		for (Ptr& ptr : m_vector)
			if (std::to_address(ptr) == &item)
			{
				erase(ptr);
				return true;
			}

		return false;
	}

	VECTOR_TEMPLATE
	ITERATOR REMOVEVECTOR::begin() const noexcept
	{
		iterator it(this, 0);
		it.update();

		return it;
	}

	VECTOR_TEMPLATE
	std::default_sentinel_t REMOVEVECTOR::end() noexcept
	{
		return { };
	}

	VECTOR_TEMPLATE
	ITERATOR REMOVEVECTOR::cbegin() const noexcept
	{
		return begin();
	}

	VECTOR_TEMPLATE
	std::default_sentinel_t REMOVEVECTOR::cend()
	{
		return { };
	}

	VECTOR_TEMPLATE
	size_t REMOVEVECTOR::size() const noexcept
	{
		return m_size;
	}

	VECTOR_TEMPLATE
	bool REMOVEVECTOR::empty() const noexcept
	{
		return !m_size;
	}

	VECTOR_TEMPLATE
	REMOVEVECTOR::FuncSource& REMOVEVECTOR::getFuncSource(Ptr& ptr) noexcept
	{
		if constexpr (FuncsFromPtr)
			return ptr;
		else
			return *ptr;
	}

	VECTOR_TEMPLATE
	T& REMOVEVECTOR::getFuncSourceItem(FuncSource& source) noexcept
	{
		if constexpr (FuncsFromPtr)
			return *source;
		else
			return source;
	}
#pragma endregion

#pragma region RemoveVectorPointer
	REMOVEVECTORPOINTER_TEMPLATE
	REMOVEVECTORPOINTER::RemoveVectorPointer(nullptr_t) noexcept
		: RemoveVectorStatusSource<T>() { }

	REMOVEVECTORPOINTER_TEMPLATE
	REMOVEVECTORPOINTER::RemoveVectorPointer(const Pointer& ptr)
		: RemoveVectorStatusSource<T>(), m_ptr(ptr) { }

	REMOVEVECTORPOINTER_TEMPLATE
	REMOVEVECTORPOINTER::RemoveVectorPointer(Pointer&& ptr) noexcept
		: RemoveVectorStatusSource<T>(), m_ptr(std::move(ptr)) { }

	REMOVEVECTORPOINTER_TEMPLATE
	REMOVEVECTORPOINTER& REMOVEVECTORPOINTER::operator=(nullptr_t) noexcept
	{
		new (this) RemoveVectorPointer(nullptr);
		return *this;
	}

	REMOVEVECTORPOINTER_TEMPLATE
	bool REMOVEVECTORPOINTER::operator==(nullptr_t) const noexcept
	{
		return this->m_status.vector == nullptr;
	}

	REMOVEVECTORPOINTER_TEMPLATE
	bool REMOVEVECTORPOINTER::operator!=(nullptr_t) const noexcept
	{
		return this->m_status.vector != nullptr;
	}

	REMOVEVECTORPOINTER_TEMPLATE
	REMOVEVECTORPOINTER::operator bool() const noexcept
	{
		return this->m_status.vector != nullptr;
	}

	REMOVEVECTORPOINTER_TEMPLATE
	const RemoveVectorStatus& REMOVEVECTORPOINTER::status() const noexcept
	{
		return this->m_status;
	}

	REMOVEVECTORPOINTER_TEMPLATE
	REMOVEVECTORPOINTER::operator Pointer& () noexcept
	{
		return m_ptr;
	}

	REMOVEVECTORPOINTER_TEMPLATE
	REMOVEVECTORPOINTER::operator const Pointer& () const noexcept
	{
		return m_ptr;
	}

	REMOVEVECTORPOINTER_TEMPLATE
	T& REMOVEVECTORPOINTER::operator*() const noexcept
	{
		return *std::to_address(*this);
	}

	REMOVEVECTORPOINTER_TEMPLATE
	T* REMOVEVECTORPOINTER::operator->() const noexcept
	{
		return std::to_address(*this);
	}
#pragma endregion

#pragma region WrappedRemoveVector
	WRAPPEDREMOVEVECTOR_TEMPLATE
	WRAPPEDREMOVEVECTOR::WrappedRemoveVector() : Base(&RemovePointer::getStatus) { }

	WRAPPEDREMOVEVECTOR_TEMPLATE
	WRAPPEDREMOVEVECTOR::RemovePointer& WRAPPEDREMOVEVECTOR::push_back(const Pointer& ptr)
		requires (std::copy_constructible<Pointer>)
	{
		return push_back(Pointer(ptr));
	}

	WRAPPEDREMOVEVECTOR_TEMPLATE
	WRAPPEDREMOVEVECTOR::RemovePointer& WRAPPEDREMOVEVECTOR::push_back(Pointer&& ptr)
	{
		return Base::push_back(RemovePointer(std::move(ptr)));
	}
#pragma endregion
}

#undef VECTOR_TEMPLATE
#undef REMOVEVECTOR
#undef ITERATOR
#undef REMOVEVECTORPOINTER
#undef REMOVEVECTORPOINTER_TEMPLATE
#undef WRAPPEDREMOVEVECTOR
#undef WRAPPEDREMOVEVECTOR_TEMPLATE
