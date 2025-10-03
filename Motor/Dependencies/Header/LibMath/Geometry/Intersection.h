#pragma once

#include "Line.h"
#include "NSphere.h"
#include "Rect.h"

namespace LibMath::Intersection
{
	/// <summary>
	/// Checks for intersection between two axis-aligned bounding boxes.
	/// </summary>
	/// <typeparam name="D">Number of dimensions</typeparam>
	/// <typeparam name="TData">Data type</typeparam>
	template <ArrIndex D, ArrayData TData>
	bool AABB(const Rect<D, TData>& a, const Rect<D, TData>& b)
	{
		for (ArrIndex axis = 0; axis < D; axis++)
		{
			const Extents<TData>
				aExt = a.extents(axis),
				bExt = b.extents(axis);

			if (aExt.max >= bExt.min && bExt.max >= aExt.min)
				return true;
		}

		return false;
	}

	/// <summary>
	/// Checks for intersection between two n-spheres.
	/// </summary>
	/// <typeparam name="D">Number of dimensions</typeparam>
	/// <typeparam name="TData">Data type</typeparam>
	template <ArrIndex D, ArrayData TData>
	bool NSphereNSphere(const NSphere<D, TData>& a, const NSphere<D, TData>& b)
	{
		const TData
			distanceSquared = (a.origin() - b.origin()).magnitudeSquared(),
			maxDistance = a.radius() + b.radius();

		return distanceSquared <= maxDistance * maxDistance;
	}

	/// <summary>
	/// Checks for intersection between a cartesian point and an axis-aligned bounding box.
	/// </summary>
	/// <typeparam name="D">Number of dimensions</typeparam>
	/// <typeparam name="TData">Data type</typeparam>
	template <ArrIndex D, ArrayData TData>
	bool PointAABB(const Cartesian<D, TData>& point, const Rect<D, TData>& rect)
	{
		for (ArrIndex axis = 0; axis < D; axis++)
		{
			const Extents<TData> ext = rect.extents(axis);
			const TData pointCoord = point.raw()[axis];

			if (pointCoord < ext.min || pointCoord > ext.max)
				return false;
		}

		return true;
	}

	/// <summary>
	/// Checks for intersection between a cartesian point and an n-sphere.
	/// </summary>
	/// <typeparam name="D">Number of dimensions</typeparam>
	/// <typeparam name="TData">Data type</typeparam>
	template <ArrIndex D, ArrayData TData>
	bool PointNSphere(const Cartesian<D, TData>& point, const NSphere<D, TData>& sphere)
	{
		return (sphere.origin() - point).magnitudeSquared() <= sphere.radiusSquared();
	}

	/// <summary>
	/// Checks for intersection between an axis-aligned bounding box and an n-sphere.
	/// </summary>
	/// <typeparam name="D">Number of dimensions</typeparam>
	/// <typeparam name="TData">Data type</typeparam>
	template <ArrIndex D, ArrayData TData>	
	bool RectNSphere(const Rect<D, TData>& rect, const NSphere<D, TData>& sphere)
	{
		Cartesian<D, TData> closePoint;

		for (ArrIndex axis = 0; axis < D; axis++)
		{
			Extents<TData> ext = rect.extents(axis);
			closePoint.raw()[axis] = max(ext.min, min(sphere.origin().raw()[axis], ext.max));
		}

		Vector<D, TData> closeDelta = closePoint - sphere.origin();

		return closeDelta.magnitudeSquared() <= sphere.radiusSquared();
	}
}
