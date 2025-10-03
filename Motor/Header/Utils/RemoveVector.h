#pragma once

#include "Pointer.h"

#include <type_traits>
#include <vcruntime.h>
#include <vector>

namespace KaputEngine
{
	// TODO Consider using a field pointer instead of function, or make RemoveVector friend and access the field directly
	template <typename T, IsPointer<T> Ptr = T*, bool FuncsFromPtr = false>
		requires (std::is_class_v<T> || (FuncsFromPtr && std::is_class_v<Ptr>))
	class RemoveVector;

	/// <summary>
	/// Wrapper object for storing the status of an item in a <see cref="RemoveVector"/>
	/// </summary>
	struct RemoveVectorStatus
	{
		/// <summary>
		/// Pointer to the vector. Null if the item is not currently in a vector
		/// </summary>
		void* vector = nullptr;

		/// <summary>
		/// Index of the item in the vector
		/// </summary>
		size_t index = 0;

		/// <summary>
		/// Casts the vector pointer.
		/// </summary>
		/// <typeparam name="T">Type of items in the vector</typeparam>
		/// <typeparam name="Pointer">Type of pointer used in the vector</typeparam>
		/// <typeparam name="FuncsFromPtr">Use the pointer type instead of the item type for notify functions</typeparam>
		/// <returns>Pointer cast to the target vector type</returns>
		template <typename T, IsPointer<T> Pointer = T*, bool FuncsFromPtr = false>
		_NODISCARD _Ret_maybenull_ RemoveVector<T, Pointer, FuncsFromPtr>* castVector() noexcept;

		/// <summary>
		/// Casts the vector pointer.
		/// </summary>
		/// <typeparam name="T">Type of items in the vector</typeparam>
		/// <typeparam name="Pointer">Type of pointer used in the vector</typeparam>
		/// <typeparam name="FuncsFromPtr">Use the pointer type instead of the item type for notify functions</typeparam>
		/// <returns>Pointer cast to the target vector type</returns>
		template <typename T, IsPointer<T> Pointer = T*, bool FuncsFromPtr = false>
		_NODISCARD _Ret_maybenull_ const RemoveVector<T, Pointer, FuncsFromPtr>* castVector() const noexcept;
	};

	/// <summary>
	/// Base class for objects responsible for tracking the state of an item in a <see cref="RemoveVector"/>. Either the item or the pointer.
	/// </summary>
	/// <typeparam name="T">Type of items in the vector</typeparam>
	template <typename T>
	class RemoveVectorStatusSource
	{
	public:
		RemoveVectorStatusSource() = default;
		RemoveVectorStatusSource(const RemoveVectorStatusSource&) = delete;
		RemoveVectorStatusSource(RemoveVectorStatusSource&&) = default;

		virtual ~RemoveVectorStatusSource() = default;

		RemoveVectorStatusSource& operator=(RemoveVectorStatusSource&&) noexcept = default;

		/// <summary>
		/// Creates a corresponding <see cref="RemoveVector"/> with the status function.
		/// </summary>
		/// <typeparam name="Pointer">Type of pointer used to store items</typeparam>
		/// <typeparam name="FuncsFromPtr">The status source is the pointer rather than the item</typeparam>
		/// <returns>Created vector</returns>
		template <IsPointer<T> Pointer = T*, bool FuncsFromPtr = false>
		_NODISCARD static RemoveVector<T, Pointer, FuncsFromPtr> createRemoveVector() noexcept;

	protected:
		mutable RemoveVectorStatus m_status;

		/// <summary>
		/// Gets the status of the responsible item in its <see cref="RemoveVector"/>.
		/// </summary>
		/// <returns></returns>
		RemoveVectorStatus& getStatus() const noexcept;
	};

	template <typename T, IsPointer<T> Pointer = T*>
	class WrappedRemoveVector;

	/// <summary>
	/// Pointer type for RemoveVector handling status responsibility
	/// </summary>
	/// <typeparam name="T">Item type</typeparam>
	/// <typeparam name="Underlying pointer type"></typeparam>
	template <typename T, IsPointer<T> Pointer = T*>
	class RemoveVectorPointer : public RemoveVectorStatusSource<T>
	{
		friend WrappedRemoveVector<T, Pointer>;

	public:
		RemoveVectorPointer() = default;
		RemoveVectorPointer(nullptr_t) noexcept;
		RemoveVectorPointer(const Pointer& ptr);
		RemoveVectorPointer(Pointer&& ptr) noexcept;

		RemoveVectorPointer(const RemoveVectorPointer&) = default;
		RemoveVectorPointer(RemoveVectorPointer&&) noexcept = default;

		RemoveVectorPointer& operator=(nullptr_t) noexcept;
		RemoveVectorPointer& operator=(RemoveVectorPointer&&) noexcept = default;

		_NODISCARD bool operator==(nullptr_t) const noexcept;
		_NODISCARD bool operator!=(nullptr_t) const noexcept;

		operator bool() const noexcept;

		_NODISCARD const RemoveVectorStatus& status() const noexcept;

		operator Pointer& () noexcept;
		operator const Pointer& () const noexcept;

		_NODISCARD T& operator*() const noexcept;
		_NODISCARD T* operator->() const noexcept;

	private:
		Pointer m_ptr;
	};

	/// <summary>
	/// Vector implementing pop and swap, moving the last item when removing an item
	/// </summary>
	/// <remarks>
	/// Items can be notified when added, moved or removed to keep track of their location for fast removal.
	/// </remarks>
	/// <typeparam name="T">Item type</typeparam>
	/// <typeparam name="Pointer">Pointer type</typeparam>
	/// <typeparam name="FuncsFromPtr">Use the pointer type instead of the item type for notify functions</typeparam>
	template <typename T, IsPointer<T> Ptr, bool FuncsFromPtr>
		requires (std::is_class_v<T> || (FuncsFromPtr && std::is_class_v<Ptr>))
	class RemoveVector
	{
	public:
		/// <summary>
		/// Type of items
		/// </summary>
		using Type = T;

		/// <summary>
		/// Type of pointer used to store items
		/// </summary>
		using Pointer = Ptr;

		/// <summary>
		/// Use the pointer type instead of the item type for notify functions
		/// </summary>
		static constexpr bool FuncsFromPointer = FuncsFromPtr;

		/// <summary>
		/// Type responsible for tracking the state of an item. Either the item or the pointer type.
		/// </summary>
		using FuncSource = std::conditional_t<FuncsFromPtr, const Ptr, T>;

		/// <summary>
		/// Function signature for status functions
		/// </summary>
		using GetStatusFunc = RemoveVectorStatus& (FuncSource::*)() const;

		friend class iterator;

		/// <summary>
		/// Custom iterator type to safely iterate the vector while items are being added or removed.
		/// </summary>
		class iterator : public std::iterator<std::forward_iterator_tag, T>
		{
		public:
			iterator() = default;
			iterator(const RemoveVectorStatus& status);
			iterator(_In_ const RemoveVector* vec, size_t index) noexcept;

			T& operator*()  noexcept;
			T* operator->() noexcept;

			/// <summary>
			/// Moves the iterator forward through ++it and updates the vector.
			/// </summary>
			iterator& operator++() noexcept;

			/// <summary>
			/// Moves the iterator forward through it++ and updates the vector.
			/// </summary>
			_NODISCARD iterator operator++(int) noexcept;

			_NODISCARD iterator operator+(int offset) noexcept;
			_NODISCARD iterator operator-(int offset) noexcept;

			_NODISCARD size_t operator+(const iterator& other) noexcept;
			_NODISCARD size_t operator-(const iterator& other) noexcept;

			_NODISCARD bool operator!=(const iterator& other) const noexcept;

#define REMOVE_VECTOR_ITERATOR_COMPARISON(op) _NODISCARD bool operator op(const iterator& other) const noexcept;
			REMOVE_VECTOR_ITERATOR_COMPARISON(==)
			REMOVE_VECTOR_ITERATOR_COMPARISON(<)
			REMOVE_VECTOR_ITERATOR_COMPARISON(>)
			REMOVE_VECTOR_ITERATOR_COMPARISON(<=)
			REMOVE_VECTOR_ITERATOR_COMPARISON(>=)
#undef REMOVE_VECTOR_ITERATOR_COMPARISON

			/// <summary>
			/// Checks if the iterator corresponds to the end of the vector.
			/// </summary>
			_NODISCARD bool operator==(const std::default_sentinel_t&) const noexcept;

			/// <summary>
			/// Checks if the iterator corresponds to an item in the vector.
			/// </summary>
			_NODISCARD bool operator!=(const std::default_sentinel_t&) const noexcept;

			_NODISCARD RemoveVectorStatus& status() noexcept;
			_NODISCARD const RemoveVectorStatus& status() const noexcept;

			_NODISCARD size_t index() const noexcept;

			_NODISCARD _Ret_maybenull_ RemoveVector* vector() noexcept;
			_NODISCARD _Ret_maybenull_ const RemoveVector* vector() const noexcept;

			_NODISCARD Ptr& pointer();

			/// <summary>
			/// Updates the vector by popping and swapping items until the iterator points to a non-null item.
			/// </summary>
			void update() noexcept;

		private:
			RemoveVectorStatus m_status;
		};

		RemoveVector(_In_ const GetStatusFunc getStatus);

		RemoveVector(const RemoveVector&) = delete;
		RemoveVector(RemoveVector&&) = delete;

		~RemoveVector() = default;

		RemoveVector& operator=(const RemoveVector&) = delete;
		RemoveVector& operator=(RemoveVector&&) = delete;

		/// <summary>
		/// Pushes an item to the back of the vector from an l-value pointer and notifies the status source.
		/// </summary>
		/// <param name="ptr">Moved pointer of the specified pointer type</param>
		/// <returns>Reference to the placed pointer</returns>
		Ptr& push_back(const Ptr& ptr) requires (!FuncsFromPtr && std::copy_constructible<Pointer>);

		/// <summary>
		/// Pushes an item to the back of the vector from an r-value and notifies the status source.
		/// </summary>
		/// <param name="ptr">Moved pointer of the specified pointer type</param>
		/// <returns>Reference to the placed pointer</returns>
		Ptr& push_back(Ptr&& ptr);

		/// <summary>
		/// Pushes an item to the back of the vector from a reference by pushing its raw pointer.
		/// </summary>
		/// <param name="item">Reference to the item</param>
		/// <returns>Raw pointer to the item</returns>
		/// <remarks>Requires <see cref="Pointer"/> to be a raw pointer.</remarks>
		T* push_back(T& item) requires (std::same_as<Ptr, T*>);

		/// <summary>
		/// Erases an item from the vector from an iterator pointing to it and updates the responsible object.
		/// </summary>
		/// <param name="it"></param>
		/// <returns>True if the iterator corresponded to an item</returns>
		bool erase(iterator it);

		/// <summary>
		/// Erases an item from the vector from a reference of the object responsible of the status and updates it.
		/// </summary>
		/// <param name="source">Object responsible of the status for the item</param>
		/// <returns>True if the status corresponded to this vector</returns>
		bool erase(FuncSource& source);

		bool erase(T& item) requires (FuncsFromPtr);

		_NODISCARD _Ret_maybenull_ Pointer& operator[](size_t index) noexcept;
		_NODISCARD _Ret_maybenull_ const Pointer& operator[](size_t index) const noexcept;

		/// <summary>
		/// Get the iterator pointing to the first item.
		/// </summary>
		/// <returns>created iterator</returns>
		_NODISCARD iterator begin() const noexcept;

		/// <summary>
		/// Creates a dummy sentinel representing the end iterator.
		/// </summary>
		/// <returns>Creates sentinel</returns>
		_NODISCARD static std::default_sentinel_t end() noexcept;

		_NODISCARD iterator cbegin() const noexcept;

		_NODISCARD static std::default_sentinel_t cend();

		_NODISCARD size_t size() const noexcept;

		_NODISCARD bool empty() const noexcept;

		void reserve(size_t size) noexcept;

	private:
		mutable std::vector<Ptr> m_vector;
		const GetStatusFunc  m_getStatusFunc;
		size_t m_size;

		/// <summary>
		/// Gets the object responsible for status functions from a pointer.
		/// </summary>
		_NODISCARD static FuncSource& getFuncSource(Ptr& ptr) noexcept;

		/// <summary>
		/// Gets the item from an object responsible for status functions.
		/// </summary>
		_NODISCARD static T& getFuncSourceItem(FuncSource& source) noexcept;
	};

	/// <summary>
	/// RemoveVector type using wrapped RemoveVectorPointer
	/// </summary>
	template <typename T, IsPointer<T> Pointer>
	class WrappedRemoveVector : public RemoveVector<T, RemoveVectorPointer<T, Pointer>, true>
	{
		using RemovePointer = RemoveVectorPointer<T, Pointer>;
		using Base = RemoveVector<T, RemovePointer, true>;

	public:
		WrappedRemoveVector();

		RemovePointer& push_back(const Pointer& ptr) requires (std::copy_constructible<Pointer>);
		RemovePointer& push_back(Pointer&& ptr);
	};
}
