#ifndef CHECKHEADER_SLIB_MATH_TRIANGLE
#define CHECKHEADER_SLIB_MATH_TRIANGLE

#include "definition.h"

#include "point.h"
#include "matrix3.h"

SLIB_MATH_NAMESPACE_BEGIN

template <class T>
class SLIB_EXPORT TriangleT
{
public:
	PointT<T> point1;
	PointT<T> point2;
	PointT<T> point3;
	
public:
	TriangleT() = default;
	
	TriangleT(const TriangleT<T>& other) = default;
	
	template <class O>
	TriangleT(const TriangleT<O>& other);
	
	TriangleT(const PointT<T>& point1, const PointT<T>& point2, const PointT<T>& point3);
	
public:
	static T getCross(const PointT<T>& point1, const PointT<T>& point2, const PointT<T>& point3);
	
	T getCross() const;
	
	T getSize() const;
	
	void transform(Matrix3T<T>& mat);
	
public:
	TriangleT<T>& operator=(const TriangleT<T>& other) = default;
	
	template <class O>
	TriangleT<T>& operator=(const TriangleT<O>& other);

};

SLIB_DECLARE_GEOMETRY_TYPE(Triangle)

SLIB_MATH_NAMESPACE_END


SLIB_MATH_NAMESPACE_BEGIN

template <class T>
template <class O>
TriangleT<T>::TriangleT(const TriangleT<O>& other) : point1(other.point1), point2(other.point2), point3(other.point3)
{
}

template <class T>
template <class O>
TriangleT<T>& TriangleT<T>::operator=(const TriangleT<O>& other)
{
	point1 = other.point1;
	point2 = other.point2;
	point3 = other.point3;
	return *this;
}

SLIB_MATH_NAMESPACE_END

#endif