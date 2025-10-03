#pragma once

#include <cstdint>

namespace KaputEngine
{
    /// <summary>
    /// Container for allocating raw objects without invoking a constructor
    /// </summary>
    /// <typeparam name="T">Type of buffered object</typeparam>
    /// <typeparam name="Managed">Zero the buffer on initialization and pass destructor calls to the buffered object.</typeparam>
	/// <remarks>The size of rawalloc is always sizeof(T) bytes. rawalloc{T}[] should be preferred to rawalloc{T[]} to reduce instantiations but is otherwise equivalent.</remarks>
    template <typename T, bool Managed = true>
    struct rawalloc
    {
        /// <summary>
        /// Backing byte buffer
        /// </summary>
        uint8_t buffer[sizeof(T)];

        rawalloc() requires (Managed);
        rawalloc() requires (!Managed) = default;

        /// <summary>
        /// Passes the destruction to the buffered object if managed.
        /// </summary>
        ~rawalloc();

        _NODISCARD T* ptr() noexcept;
        _NODISCARD const T* ptr() const noexcept;

        _NODISCARD T& val() noexcept;
        _NODISCARD const T& val() const noexcept;

        _NODISCARD operator T& () noexcept;
        _NODISCARD operator const T& () const noexcept;

        _NODISCARD T* operator->() noexcept;
        _NODISCARD const T* operator->() const noexcept;

        rawalloc& operator=(const T& value);
        rawalloc& operator=(T&& value);

        rawalloc<T>& operator=(const rawalloc<T>&) = delete;
        rawalloc<T>& operator=(rawalloc<T>&&) = delete;
    };
}
