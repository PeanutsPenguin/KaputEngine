#pragma once

#include <concepts>

#define STATIC_DEF(...) decltype(__VA_ARGS__) __VA_ARGS__

namespace KaputEngine
{
    /// <summary>
    /// Helper function for implementing copy assignment operators.
    /// </summary>
    /// <typeparam name="T">Object type</typeparam>
    /// <param name="_this">Reference to this</param>
    /// <param name="value">R-value of the operator</param>
    /// <returns>Passthrough of _this for returning</returns>
    template <std::copy_constructible T>
    T& copyAssign(T& _this, const T& value)
    {
        if (&_this == &value)
            return _this;

        _this.~T();
        new (&_this) T(value);

        return _this;
    }

    /// <summary>
    /// Helper function for implementing move assignment operators.
    /// </summary>
    /// <typeparam name="T">Object type</typeparam>
    /// <param name="_this">Reference to this</param>
    /// <param name="value">R-value of the operator</param>
    /// <returns>Passthrough of _this for returning</returns>
    template <std::move_constructible T>
    T& moveAssign(T& _this, T&& value)
    {
        if (&_this == &value)
            return _this;

        _this.~T();
        new (&_this) T(std::forward<T>(value));

        return _this;
    }
}
