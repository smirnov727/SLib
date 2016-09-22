#ifndef CHECKHEADER_SLIB_CORE_ITERATOR
#define CHECKHEADER_SLIB_CORE_ITERATOR

#include "definition.h"

#include "reference.h"

SLIB_NAMESPACE_BEGIN

class IIteratorBase : public Referable
{
	SLIB_DECLARE_OBJECT
};

template <class TYPE>
class SLIB_EXPORT IIterator : public IIteratorBase
{
public:
	virtual sl_bool hasNext() = 0;
	
	virtual sl_bool next(TYPE* _out = sl_null) = 0;
	
	virtual sl_reg getIndex() = 0;
	
};

template <class TYPE>
class SafeIterator;

#define SLIB_TEMPLATE_PARAMS_IIterator T
#define SLIB_TEMPLATE_DEF_PARAMS_IIterator class T

/** auto-referencing object **/
template <class T>
class SLIB_EXPORT Iterator
{
public:
	Ref< IIterator<T> > ref;
	SLIB_DECLARE_TEMPLATE_REF_WRAPPER(Iterator, SafeIterator, IIterator)

public:
	sl_bool hasNext() const;

	sl_reg getIndex() const;

	sl_bool next(T* _out) const;
	
};

/** auto-referencing object **/
template <class T>
class SLIB_EXPORT SafeIterator
{
	SafeRef< IIterator<T> > ref;
	SLIB_DECLARE_TEMPLATE_REF_WRAPPER(SafeIterator, Iterator, IIterator)
	
public:
	sl_bool hasNext() const;
	
	sl_reg getIndex() const;
	
	sl_bool next(T* _out) const;
	
};

SLIB_NAMESPACE_END


SLIB_NAMESPACE_BEGIN

SLIB_DEFINE_TEMPLATE_REF_WRAPPER(Iterator, SafeIterator, IIterator, ref)

template <class T>
SLIB_INLINE sl_bool Iterator<T>::hasNext() const
{
	IIterator<T>* obj = ref.ptr;
	if (obj) {
		return obj->hasNext();
	}
	return sl_false;
}

template <class T>
SLIB_INLINE sl_reg Iterator<T>::getIndex() const
{
	IIterator<T>* obj = ref.ptr;
	if (obj) {
		return obj->getIndex();
	}
	return -1;
}

template <class T>
SLIB_INLINE sl_bool Iterator<T>::next(T* _out) const
{
	IIterator<T>* obj = ref.ptr;
	if (obj) {
		return obj->next(_out);
	}
	return sl_false;
};


SLIB_DEFINE_TEMPLATE_REF_WRAPPER(SafeIterator, Iterator, IIterator, ref)

template <class T>
SLIB_INLINE sl_bool SafeIterator<T>::hasNext() const
{
	Ref< IIterator<T> > obj(ref);
	if (obj.isNotNull()) {
		return obj->hasNext();
	}
	return sl_false;
}

template <class T>
SLIB_INLINE sl_reg SafeIterator<T>::getIndex() const
{
	Ref< IIterator<T> > obj(ref);
	if (obj.isNotNull()) {
		return obj->getIndex();
	}
	return -1;
}

template <class T>
SLIB_INLINE sl_bool SafeIterator<T>::next(T* _out) const
{
	Ref< IIterator<T> > obj(ref);
	if (obj.isNotNull()) {
		return obj->next(_out);
	}
	return sl_false;
};

SLIB_NAMESPACE_END

#endif