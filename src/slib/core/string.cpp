#include "../../../inc/slib/core/string.h"
#include "../../../inc/slib/core/base.h"
#include "../../../inc/slib/core/mio.h"
#include "../../../inc/slib/core/endian.h"
#include "../../../inc/slib/core/scoped_pointer.h"
#include "../../../inc/slib/core/math.h"
#include "../../../inc/slib/core/variant.h"
#include "../../../inc/slib/core/parse.h"

SLIB_NAMESPACE_BEGIN

SLIB_DEFINE_EXPLICIT_INSTANTIATIONS_FOR_LIST(String)
SLIB_DEFINE_EXPLICIT_INSTANTIATIONS_FOR_MAP(String, Ref<Referable>)
SLIB_DEFINE_EXPLICIT_INSTANTIATIONS_FOR_MAP(String, String)

class _TemplateFunc8
{
public:
	SLIB_INLINE static const void* findMemory(const sl_char8* mem, sl_char8 pattern, sl_size count)
	{
		return Base::findMemory(mem, pattern, count);
	}
	
	SLIB_INLINE static const void* findMemoryReverse(const sl_char8* mem, sl_char8 pattern, sl_size count)
	{
		return Base::findMemoryReverse(mem, pattern, count);
	}
	
	SLIB_INLINE static sl_int32 compareMemory(const sl_char8* mem1, const sl_char8* mem2, sl_size count)
	{
		return Base::compareMemory((sl_uint8*)mem1, (sl_uint8*)mem2, count);
	}
	
	SLIB_INLINE static void resetMemory(sl_char8* dst, sl_char8 value, sl_size count)
	{
		return Base::resetMemory(dst, value, count);
	}
};

class _TemplateFunc16
{
public:
	SLIB_INLINE static const void* findMemory(const sl_char16* mem, sl_char16 pattern, sl_size count)
	{
		return Base::findMemory2((sl_uint16*)mem, pattern, count);
	}
	
	SLIB_INLINE static const void* findMemoryReverse(const sl_char16* mem, sl_char16 pattern, sl_size count)
	{
		return Base::findMemoryReverse2((sl_uint16*)mem, pattern, count);
	}
	
	SLIB_INLINE static sl_int16 compareMemory(const sl_char16* mem1, const sl_char16* mem2, sl_size count)
	{
		return Base::compareMemory2((sl_uint16*)mem1, (sl_uint16*)mem2, count);
	}
	
	SLIB_INLINE static void resetMemory(sl_char16* dst, sl_char16 value, sl_size count)
	{
		return Base::resetMemory2((sl_uint16*)dst, value, count);
	}
};


const sl_char8 _g_string8_null_buf[] = {0, 0};

const StringContainer8 _g_string8_null_container = {(sl_char8*)(_g_string8_null_buf), 0, 0, -1};
const _String8_Const _String8_Null = {(StringContainer8*)(&_g_string8_null_container), 0};

const StringContainer8 _g_string8_empty_container = {(sl_char8*)(_g_string8_null_buf), 0, 0, -1};
const _String8_Const _String8_Empty = {(StringContainer8*)(&_g_string8_empty_container), 0};

const sl_char16 _g_string16_null_buf[] = {0, 0};

const StringContainer16 _g_string16_null_container = {(sl_char16*)(_g_string16_null_buf), 0, 0, -1};
const _String16_Const _String16_Null = {(StringContainer16*)(&_g_string16_null_container), 0};

const StringContainer16 _g_string16_empty_container = {(sl_char16*)(_g_string16_null_buf), 0, 0, -1};
const _String16_Const _String16_Empty = {(StringContainer16*)(&_g_string16_empty_container), 0};


SLIB_INLINE sl_reg StringContainer8::increaseReference()
{
	if (ref >= 0) {
		return Base::interlockedIncrement(&ref);
	}
	return 1;
}

SLIB_INLINE sl_reg StringContainer16::increaseReference()
{
	if (ref >= 0) {
		return Base::interlockedIncrement(&ref);
	}
	return 1;
}


SLIB_INLINE sl_reg StringContainer8::decreaseReference()
{
	if (ref > 0) {
		sl_reg nRef = Base::interlockedDecrement(&ref);
		if (nRef == 0) {
			Base::freeMemory(this);
		}
		return nRef;
	}
	return 1;
}

SLIB_INLINE sl_reg StringContainer16::decreaseReference()
{
	if (ref > 0) {
		sl_reg nRef = Base::interlockedDecrement(&ref);
		if (nRef == 0) {
			Base::freeMemory(this);
		}
		return nRef;
	}
	return 1;
}


SLIB_INLINE StringContainer8* String8::_alloc(sl_size len)
{
	if (len == 0) {
		return _String8_Empty.container;
	}
	sl_char8* buf = (sl_char8*)(Base::createMemory(sizeof(StringContainer8) + len + 1));
	if (buf) {
		StringContainer8* container = (StringContainer8*)((void*)buf);
		container->sz = buf + sizeof(StringContainer8);
		container->len = len;
		container->hash = 0;
		container->ref = 1;
		container->sz[len] = 0;
		return container;
	}
	return _String8_Null.container;
}

SLIB_INLINE StringContainer16* String16::_alloc(sl_size len)
{
	if (len == 0) {
		return _String16_Empty.container;
	}
	sl_char8* buf = (sl_char8*)(Base::createMemory(sizeof(StringContainer8) + ((len + 1) << 1)));
	if (buf) {
		StringContainer16* container = (StringContainer16*)((void*)buf);
		container->sz = (sl_char16*)((void*)(buf + sizeof(StringContainer16)));
		container->len = len;
		container->hash = 0;
		container->ref = 1;
		container->sz[len] = 0;
		return container;
	}
	return _String16_Null.container;
}


SLIB_INLINE StringContainer8* SafeString8::_retainContainer() const
{
	if ((void*)(this) == (void*)(&_String8_Null)) {
		return _String8_Null.container;
	} else if ((void*)(this) == (void*)(&_String8_Empty)) {
		return _String8_Empty.container;
	} else {
		SpinLocker lock(&m_lock);
		StringContainer8* container = m_container;
		container->increaseReference();
		return container;
	}
}

SLIB_INLINE StringContainer16* SafeString16::_retainContainer() const
{
	if ((void*)(this) == (void*)(&_String16_Null)) {
		return _String16_Null.container;
	} else if ((void*)(this) == (void*)(&_String16_Empty)) {
		return _String16_Empty.container;
	} else {
		SpinLocker lock(&m_lock);
		StringContainer16* container = m_container;
		container->increaseReference();
		return container;
	}
}


SLIB_INLINE void String8::_replaceContainer(StringContainer8* container)
{
	m_container->decreaseReference();
	m_container = container;
}

SLIB_INLINE void String16::_replaceContainer(StringContainer16* container)
{
	m_container->decreaseReference();
	m_container = container;
}

SLIB_INLINE void SafeString8::_replaceContainer(StringContainer8* container)
{
	StringContainer8* before;
	{
		SpinLocker lock(&m_lock);
		before = m_container;
		m_container = container;
	}
	before->decreaseReference();
}

SLIB_INLINE void SafeString16::_replaceContainer(StringContainer16* container)
{
	StringContainer16* before;
	{
		SpinLocker lock(&m_lock);
		before = m_container;
		m_container = container;
	}
	before->decreaseReference();
}


SLIB_INLINE StringContainer8* String8::_create(sl_char8 ch, sl_size nRepeatCount)
{
	StringContainer8* container = _alloc(nRepeatCount);
	if (container != _String8_Null.container) {
		Base::resetMemory(container->sz, ch, nRepeatCount);
	}
	return container;
}

SLIB_INLINE StringContainer16* String16::_create(sl_char16 ch, sl_size nRepeatCount)
{
	StringContainer16* container = _alloc(nRepeatCount);
	if (container != _String16_Null.container) {
		Base::resetMemory2((sl_uint16*)(container->sz), ch, nRepeatCount);
	}
	return container;
}

SLIB_INLINE StringContainer8* String8::_create(const sl_char8* utf8, sl_reg lenUtf8)
{
	if (utf8) {
		if (lenUtf8 < 0) {
			lenUtf8 = Base::getStringLength(utf8);
		}
		sl_size len = lenUtf8;
		StringContainer8* container = _alloc(len);
		if (container != _String8_Null.container) {
			Base::copyMemory(container->sz, utf8, len);
		}
		return container;
	}
	return _String8_Null.container;
}

SLIB_INLINE StringContainer16* String16::_create(const sl_char8* utf8, sl_reg lenUtf8)
{
	if (utf8) {
		if (lenUtf8 < 0) {
			lenUtf8 = Base::getStringLength(utf8);
		}
		sl_size len = Charsets::utf8ToUtf16(utf8, lenUtf8, sl_null, -1);
		StringContainer16* container = _alloc(len);
		if (container != _String16_Null.container) {
			Charsets::utf8ToUtf16(utf8, lenUtf8, container->sz, len);
			container->sz[len] = 0;
		}
		return container;
	}
	return _String16_Null.container;
}

SLIB_INLINE StringContainer8* String8::_create(const sl_char16* utf16, sl_reg lenUtf16)
{
	if (utf16) {
		if (lenUtf16 < 0) {
			lenUtf16 = Base::getStringLength2(utf16);
		}
		sl_size len = Charsets::utf16ToUtf8(utf16, lenUtf16, sl_null, -1);
		StringContainer8* container = _alloc(len);
		if (container != _String8_Null.container) {
			Charsets::utf16ToUtf8(utf16, lenUtf16, container->sz, len);
			container->sz[len] = 0;
		}
		return container;
	}
	return _String8_Null.container;
}

SLIB_INLINE StringContainer16* String16::_create(const sl_char16* utf16, sl_reg lenUtf16)
{
	if (utf16) {
		if (lenUtf16 < 0) {
			lenUtf16 = Base::getStringLength2(utf16);
		}
		sl_size len = lenUtf16;
		StringContainer16* container = _alloc(len);
		if (container != _String16_Null.container) {
			Base::copyMemory(container->sz, utf16, len*sizeof(sl_char16));
		}
		return container;
	}
	return _String16_Null.container;
}

SLIB_INLINE StringContainer8* String8::_create(const sl_char32* utf32, sl_reg lenUtf32)
{
	if (utf32) {
		if (lenUtf32 < 0) {
			lenUtf32 = Base::getStringLength4(utf32);
		}
		sl_size len = Charsets::utf32ToUtf8(utf32, lenUtf32, sl_null, -1);
		StringContainer8* container = _alloc(len);
		if (container != _String8_Null.container) {
			Charsets::utf32ToUtf8(utf32, lenUtf32, container->sz, len);
			container->sz[len] = 0;
		}
		return container;
	}
	return _String8_Null.container;
}

SLIB_INLINE StringContainer16* String16::_create(const sl_char32* utf32, sl_reg lenUtf32)
{
	if (utf32) {
		if (lenUtf32 < 0) {
			lenUtf32 = Base::getStringLength4(utf32);
		}
		sl_size len = Charsets::utf32ToUtf16(utf32, lenUtf32, sl_null, -1);
		StringContainer16* container = _alloc(len);
		if (container != _String16_Null.container) {
			Charsets::utf32ToUtf16(utf32, lenUtf32, container->sz, len);
			container->sz[len] = 0;
		}
		return container;
	}
	return _String16_Null.container;
}


SLIB_INLINE StringContainer8* String8::_merge8(const sl_char8* s1, sl_reg len1, const sl_char8* s2, sl_reg len2)
{
	if (len1 < 0) {
		len1 = Base::getStringLength(s1);
	}
	if (len2 < 0) {
		len2 = Base::getStringLength(s2);
	}
	sl_size len = len1 + len2;
	StringContainer8* s = _alloc(len);
	if (s != _String8_Null.container) {
		Base::copyMemory(s->sz, s1, len1);
		Base::copyMemory(s->sz + len1, s2, len2);
		s->sz[len] = 0;
	}
	return s;
}

SLIB_INLINE StringContainer16* String16::_merge16(const sl_char16* s1, sl_reg len1, const sl_char16* s2, sl_reg len2)
{
	if (len1 < 0) {
		len1 = Base::getStringLength2(s1);
	}
	if (len2 < 0) {
		len2 = Base::getStringLength2(s2);
	}
	sl_size len = len1 + len2;
	StringContainer16* s = _alloc(len);
	if (s != _String16_Null.container) {
		Base::copyMemory(s->sz, s1, len1*sizeof(sl_char16));
		Base::copyMemory(s->sz + len1, s2, len2*sizeof(sl_char16));
		s->sz[len] = 0;
	}
	return s;
}

SLIB_INLINE StringContainer8* String8::_merge16(const sl_char8* s1, sl_reg len1, const sl_char16* s2_u16, sl_reg len2_u16)
{
	if (len1 < 0) {
		len1 = Base::getStringLength(s1);
	}
	if (len2_u16 < 0) {
		len2_u16 = Base::getStringLength2(s2_u16);
	}
	sl_size len2 = Charsets::utf16ToUtf8(s2_u16, len2_u16, sl_null, -1);
	sl_size len = len1 + len2;
	StringContainer8* s = _alloc(len);
	if (s != _String8_Null.container) {
		Base::copyMemory(s->sz, s1, len1);
		Charsets::utf16ToUtf8(s2_u16, len2_u16, s->sz + len1, len2);
		s->sz[len] = 0;
	}
	return s;
}

SLIB_INLINE StringContainer8* String8::_merge16(const sl_char16* s1_u16, sl_reg len1_u16, const sl_char8* s2, sl_reg len2)
{
	if (len1_u16 < 0) {
		len1_u16 = Base::getStringLength2(s1_u16);
	}
	if (len2 < 0) {
		len2 = Base::getStringLength(s2);
	}
	sl_size len1 = Charsets::utf16ToUtf8(s1_u16, len1_u16, sl_null, -1);
	sl_size len = len1 + len2;
	StringContainer8* s = _alloc(len);
	if (s != _String8_Null.container) {
		Charsets::utf16ToUtf8(s1_u16, len1_u16, s->sz, len1);
		Base::copyMemory(s->sz + len1, s2, len2);
		s->sz[len] = 0;
	}
	return s;
}

SLIB_INLINE StringContainer8* String8::_merge32(const sl_char8* s1, sl_reg len1, const sl_char32* s2_u32, sl_reg len2_u32)
{
	if (len1 < 0) {
		len1 = Base::getStringLength(s1);
	}
	if (len2_u32 < 0) {
		len2_u32 = Base::getStringLength4(s2_u32);
	}
	sl_size len2 = Charsets::utf32ToUtf8(s2_u32, len2_u32, sl_null, -1);
	sl_size len = len1 + len2;
	StringContainer8* s = _alloc(len);
	if (s != _String8_Null.container) {
		Base::copyMemory(s->sz, s1, len1*sizeof(sl_char8));
		Charsets::utf32ToUtf8(s2_u32, len2_u32, s->sz + len1, len2);
		s->sz[len] = 0;
	}
	return s;
}

SLIB_INLINE StringContainer8* String8::_merge32(const sl_char32* s1_u32, sl_reg len1_u32, const sl_char8* s2, sl_reg len2)
{
	if (len1_u32 < 0) {
		len1_u32 = Base::getStringLength4(s1_u32);
	}
	if (len2 < 0) {
		len2 = Base::getStringLength(s2);
	}
	sl_size len1 = Charsets::utf32ToUtf8(s1_u32, len1_u32, sl_null, -1);
	sl_size len = len1 + len2;
	StringContainer8* s = _alloc(len);
	if (s != _String8_Null.container) {
		Charsets::utf32ToUtf8(s1_u32, len1_u32, s->sz, len1);
		Base::copyMemory(s->sz + len1, s2, len2);
		s->sz[len] = 0;
	}
	return s;
}

SLIB_INLINE StringContainer16* String16::_merge8(const sl_char16* s1, sl_reg len1, const sl_char8* s2_u8, sl_reg len2_u8)
{
	if (len1 < 0) {
		len1 = Base::getStringLength2(s1);
	}
	if (len2_u8 < 0) {
		len2_u8 = Base::getStringLength(s2_u8);
	}
	sl_size len2 = Charsets::utf8ToUtf16(s2_u8, len2_u8, sl_null, -1);
	sl_size len = len1 + len2;
	StringContainer16* s = _alloc(len);
	if (s != _String16_Null.container) {
		Base::copyMemory(s->sz, s1, len1*sizeof(sl_char16));
		Charsets::utf8ToUtf16(s2_u8, len2_u8, s->sz + len1, len2);
		s->sz[len] = 0;
	}
	return s;
}

SLIB_INLINE StringContainer16* String16::_merge8(const sl_char8* s1_u8, sl_reg len1_u8, const sl_char16* s2, sl_reg len2)
{
	if (len1_u8 < 0) {
		len1_u8 = Base::getStringLength(s1_u8);
	}
	if (len2 < 0) {
		len2 = Base::getStringLength2(s2);
	}
	sl_size len1 = Charsets::utf8ToUtf16(s1_u8, len1_u8, sl_null, -1);
	sl_size len = len1 + len2;
	StringContainer16* s = _alloc(len);
	if (s != _String16_Null.container) {
		Charsets::utf8ToUtf16(s1_u8, len1_u8, s->sz, len1);
		Base::copyMemory(s->sz + len1, s2, len2*sizeof(sl_char16));
		s->sz[len] = 0;
	}
	return s;
}

SLIB_INLINE StringContainer16* String16::_merge32(const sl_char16* s1, sl_reg len1, const sl_char32* s2_u32, sl_reg len2_u32)
{
	if (len1 < 0) {
		len1 = Base::getStringLength2(s1);
	}
	if (len2_u32 < 0) {
		len2_u32 = Base::getStringLength4(s2_u32);
	}
	sl_size len2 = Charsets::utf32ToUtf16(s2_u32, len2_u32, sl_null, -1);
	sl_size len = len1 + len2;
	StringContainer16* s = _alloc(len);
	if (s != _String16_Null.container) {
		Base::copyMemory(s->sz, s1, len1*sizeof(sl_char16));
		Charsets::utf32ToUtf16(s2_u32, len2_u32, s->sz + len1, len2);
		s->sz[len] = 0;
	}
	return s;
}

SLIB_INLINE StringContainer16* String16::_merge32(const sl_char32* s1_u32, sl_reg len1_u32, const sl_char16* s2, sl_reg len2)
{
	if (len1_u32 < 0) {
		len1_u32 = Base::getStringLength4(s1_u32);
	}
	if (len2 < 0) {
		len2 = Base::getStringLength2(s2);
	}
	sl_size len1 = Charsets::utf32ToUtf16(s1_u32, len1_u32, sl_null, -1);
	sl_size len = len1 + len2;
	StringContainer16* s = _alloc(len);
	if (s != _String16_Null.container) {
		Charsets::utf32ToUtf16(s1_u32, len1_u32, s->sz, len1);
		Base::copyMemory(s->sz + len1, s2, len2*sizeof(sl_char16));
		s->sz[len] = 0;
	}
	return s;
}

SLIB_INLINE sl_bool String8::_equals8(const sl_char8* str1, sl_reg len1, const sl_char8* str2, sl_reg len2)
{
	if (len1 < 0) {
		len1 = SLIB_REG_MAX;
	}
	if (len2 < 0) {
		len2 = SLIB_REG_MAX;
	}
	sl_size len = SLIB_MIN(len1, len2);
	sl_uint8* s1 = (sl_uint8*)str1;
	sl_uint8* s2 = (sl_uint8*)str2;
	for (sl_size i = 0; i < len; i++) {
		sl_uint8 c1 = s1[i];
		sl_uint8 c2 = s2[i];
		if (c1 != c2) {
			return sl_false;
		}
		if (c1 == 0) {
			return sl_true;
		}
	}
	if (len1 < len2) {
		if (s2[len1] == 0) {
			return sl_true;
		} else {
			return sl_false;
		}
	}
	if (len1 > len2) {
		if (s1[len2] == 0) {
			return sl_true;
		} else {
			return sl_false;
		}
	}
	return sl_true;
}

SLIB_INLINE sl_bool String16::_equals16(const sl_char16* str1, sl_reg len1, const sl_char16* str2, sl_reg len2)
{
	if (len1 < 0) {
		len1 = SLIB_REG_MAX;
	}
	if (len2 < 0) {
		len2 = SLIB_REG_MAX;
	}
	sl_size len = SLIB_MIN(len1, len2);
	sl_uint16* s1 = (sl_uint16*)str1;
	sl_uint16* s2 = (sl_uint16*)str2;
	for (sl_size i = 0; i < len; i++) {
		sl_uint16 c1 = s1[i];
		sl_uint16 c2 = s2[i];
		if (c1 != c2) {
			return sl_false;
		}
		if (c1 == 0) {
			return sl_true;
		}
	}
	if (len1 < len2) {
		if (s2[len1] == 0) {
			return sl_true;
		} else {
			return sl_false;
		}
	}
	if (len1 > len2) {
		if (s1[len2] == 0) {
			return sl_true;
		} else {
			return sl_false;
		}
	}
	return sl_true;
}

SLIB_INLINE sl_bool String8::_equals16(const sl_char8* str1, sl_reg len1, const sl_char16* str2, sl_reg len2)
{
	String8 other(str2, len2);
	return _equals8(str1, len1, other.getData(), other.getLength());
}

SLIB_INLINE sl_bool String16::_equals8(const sl_char16* str1, sl_reg len1, const sl_char8* str2, sl_reg len2)
{
	String16 other(str2, len2);
	return _equals16(str1, len1, other.getData(), other.getLength());
}

SLIB_INLINE sl_bool String8::_equals32(const sl_char8* str1, sl_reg len1, const sl_char32* str2, sl_reg len2)
{
	String8 other(str2, len2);
	return _equals8(str1, len1, other.getData(), other.getLength());
}

SLIB_INLINE sl_bool String16::_equals32(const sl_char16* str1, sl_reg len1, const sl_char32* str2, sl_reg len2)
{
	return _compare32(str1, len1, str2, len2) == 0;
}


SLIB_INLINE sl_int32 String8::_compare8(const sl_char8* str1, sl_reg len1, const sl_char8* str2, sl_reg len2)
{
	if (len1 < 0) {
		len1 = SLIB_REG_MAX;
	}
	if (len2 < 0) {
		len2 = SLIB_REG_MAX;
	}
	sl_size len = SLIB_MIN(len1, len2);
	sl_uint8* s1 = (sl_uint8*)str1;
	sl_uint8* s2 = (sl_uint8*)str2;
	for (sl_size i = 0; i < len; i++) {
		sl_uint8 c1 = s1[i];
		sl_uint8 c2 = s2[i];
		if (c1 < c2) {
			return -1;
		}
		if (c1 > c2) {
			return 1;
		}
		if (c1 == 0) {
			return 0;
		}
	}
	if (len1 < len2) {
		if (s2[len1] == 0) {
			return 0;
		} else {
			return -1;
		}
	}
	if (len1 > len2) {
		if (s1[len2] == 0) {
			return 0;
		} else {
			return 1;
		}
	}
	return 0;
}

SLIB_INLINE sl_int32 String16::_compare16(const sl_char16* str1, sl_reg len1, const sl_char16* str2, sl_reg len2)
{
	if (len1 < 0) {
		len1 = SLIB_REG_MAX;
	}
	if (len2 < 0) {
		len2 = SLIB_REG_MAX;
	}
	sl_size len = SLIB_MIN(len1, len2);
	sl_uint16* s1 = (sl_uint16*)str1;
	sl_uint16* s2 = (sl_uint16*)str2;
	for (sl_size i = 0; i < len; i++) {
		sl_uint16 c1 = s1[i];
		sl_uint16 c2 = s2[i];
		if (c1 < c2) {
			return -1;
		}
		if (c1 > c2) {
			return 1;
		}
		if (c1 == 0) {
			return 0;
		}
	}
	if (len1 < len2) {
		if (s2[len1] == 0) {
			return 0;
		} else {
			return -1;
		}
	}
	if (len1 > len2) {
		if (s1[len2] == 0) {
			return 0;
		} else {
			return 1;
		}
	}
	return 0;
}

SLIB_INLINE sl_int32 String8::_compare16(const sl_char8* str1, sl_reg len1, const sl_char16* str2, sl_reg len2)
{
	String8 other(str2, len2);
	return _compare8(str1, len1, other.getData(), other.getLength());
}

SLIB_INLINE sl_int32 String16::_compare8(const sl_char16* str1, sl_reg len1, const sl_char8* str2, sl_reg len2)
{
	String16 other(str2, len2);
	return _compare16(str1, len1, other.getData(), other.getLength());
}

SLIB_INLINE sl_int32 String8::_compare32(const sl_char8* str1, sl_reg len1, const sl_char32* str2, sl_reg len2)
{
	String8 other(str2, len2);
	return _compare8(str1, len1, other.getData(), other.getLength());
}

SLIB_INLINE sl_int32 String16::_compare32(const sl_char16* m1, sl_reg len1, const sl_char32* m2, sl_reg len2)
{
	if (len1 < 0) {
		len1 = SLIB_REG_MAX;
	}
	if (len2 < 0) {
		len2 = SLIB_REG_MAX;
	}
	sl_size len = SLIB_MIN(len1, len2);
	sl_uint16* s1 = (sl_uint16*)m1;
	sl_uint32* s2 = (sl_uint32*)m2;
	for (sl_size i = 0; i < len; i++) {
		sl_uint32 c1 = s1[i];
		sl_uint32 c2 = s2[i];
		if (c1 < c2) {
			return -1;
		}
		if (c1 > c2) {
			return 1;
		}
		if (c1 == 0) {
			return 0;
		}
	}
	if (len1 < len2) {
		if (s2[len1] == 0) {
			return 0;
		} else {
			return -1;
		}
	}
	if (len1 > len2) {
		if (s1[len2] == 0) {
			return 0;
		} else {
			return 1;
		}
	}
	return 0;
}


String8::String8(const String8& src)
{
	StringContainer8* container = src.m_container;
	container->increaseReference();
	m_container= container;
}

String16::String16(const String16& src)
{
	StringContainer16* container = src.m_container;
	container->increaseReference();
	m_container = container;
}

SafeString8::SafeString8(const String8& src)
{
	StringContainer8* container = src.m_container;
	container->increaseReference();
	m_container = container;
}

SafeString16::SafeString16(const String16& src)
{
	StringContainer16* container = src.m_container;
	container->increaseReference();
	m_container = container;
}


String8::String8(const SafeString8& src)
{
	m_container = src._retainContainer();
}

String16::String16(const SafeString16& src)
{
	m_container = src._retainContainer();
}

SafeString8::SafeString8(const SafeString8& src)
{
	m_container = src._retainContainer();
}

SafeString16::SafeString16(const SafeString16& src)
{
	m_container = src._retainContainer();
}


String8::String8(const String16& src)
{
	if (src.isNull()) {
		m_container = _String8_Null.container;
	} else {
		sl_size len = src.getLength();
		if (len == 0) {
			m_container = _String8_Empty.container;
		} else {
			m_container = _create(src.getData(), len);
		}
	}
}

String16::String16(const String8& src)
{
	if (src.isNull()) {
		m_container = _String16_Null.container;
	} else {
		sl_size len = src.getLength();
		if (len == 0) {
			m_container = _String16_Empty.container;
		} else {
			m_container = _create(src.getData(), len);
		}
	}
}

SafeString8::SafeString8(const String16& src)
{
	if (src.isNull()) {
		m_container = _String8_Null.container;
	} else {
		sl_size len = src.getLength();
		if (len == 0) {
			m_container = _String8_Empty.container;
		} else {
			m_container = String8::_create(src.getData(), len);
		}
	}
}

SafeString16::SafeString16(const String8& src)
{
	if (src.isNull()) {
		m_container = _String16_Null.container;
	} else {
		sl_size len = src.getLength();
		if (len == 0) {
			m_container = _String16_Empty.container;
		} else {
			m_container = String16::_create(src.getData(), len);
		}
	}
}


String8::String8(const SafeString16& _src)
{
	String16 src(_src);
	if (src.isNull()) {
		m_container = _String8_Null.container;
	} else {
		sl_size len = src.getLength();
		if (len == 0) {
			m_container = _String8_Empty.container;
		} else {
			m_container = _create(src.getData(), len);
		}
	}
}

String16::String16(const SafeString8& _src)
{
	String8 src(_src);
	if (src.isNull()) {
		m_container = _String16_Null.container;
	} else {
		sl_size len = src.getLength();
		if (len == 0) {
			m_container = _String16_Empty.container;
		} else {
			m_container = _create(src.getData(), len);
		}
	}
}

SafeString8::SafeString8(const SafeString16& _src)
{
	String16 src(_src);
	if (src.isNull()) {
		m_container = _String8_Null.container;
	} else {
		sl_size len = src.getLength();
		if (len == 0) {
			m_container = _String8_Empty.container;
		} else {
			m_container = String8::_create(src.getData(), len);
		}
	}
}

SafeString16::SafeString16(const SafeString8& _src)
{
	String8 src(_src);
	if (src.isNull()) {
		m_container = _String16_Null.container;
	} else {
		sl_size len = src.getLength();
		if (len == 0) {
			m_container = _String16_Empty.container;
		} else {
			m_container = String16::_create(src.getData(), len);
		}
	}
}


String8::~String8()
{
	m_container->decreaseReference();
}

String16::~String16()
{
	m_container->decreaseReference();
}

SafeString8::~SafeString8()
{
	m_container->decreaseReference();
}

SafeString16::~SafeString16()
{
	m_container->decreaseReference();
}


String8::String8(sl_char8 ch, sl_size nRepeatCount)
{
	m_container = _create(ch, nRepeatCount);
}

SafeString8::SafeString8(sl_char8 ch, sl_size nRepeatCount)
{
	m_container = String8::_create(ch, nRepeatCount);
}

String16::String16(sl_char16 ch, sl_size nRepeatCount)
{
	m_container = _create(ch, nRepeatCount);
}

SafeString16::SafeString16(sl_char16 ch, sl_size nRepeatCount)
{
	m_container = String16::_create(ch, nRepeatCount);
}


String8::String8(const sl_char8* strUtf8)
{
	m_container = _create(strUtf8, -1);
}

String8::String8(const sl_char8* strUtf8, sl_reg length)
{
	m_container = _create(strUtf8, length);
}

SafeString8::SafeString8(const sl_char8* strUtf8)
{
	m_container = String8::_create(strUtf8, -1);
}

SafeString8::SafeString8(const sl_char8* strUtf8, sl_reg length)
{
	m_container = String8::_create(strUtf8, length);
}

String16::String16(const sl_char8* strUtf8)
{
	m_container = _create(strUtf8, -1);
}

String16::String16(const sl_char8* strUtf8, sl_reg length)
{
	m_container = _create(strUtf8, length);
}

SafeString16::SafeString16(const sl_char8* strUtf8)
{
	m_container = String16::_create(strUtf8, -1);
}

SafeString16::SafeString16(const sl_char8* strUtf8, sl_reg length)
{
	m_container = String16::_create(strUtf8, length);
}


String8::String8(const sl_char16* strUtf16)
{
	m_container = _create(strUtf16, -1);
}

String8::String8(const sl_char16* strUtf16, sl_reg length)
{
	m_container = _create(strUtf16, length);
}

SafeString8::SafeString8(const sl_char16* strUtf16)
{
	m_container = String8::_create(strUtf16, -1);
}

SafeString8::SafeString8(const sl_char16* strUtf16, sl_reg length)
{
	m_container = String8::_create(strUtf16, length);
}

String16::String16(const sl_char16* strUtf16)
{
	m_container = _create(strUtf16, -1);
}

String16::String16(const sl_char16* strUtf16, sl_reg length)
{
	m_container = _create(strUtf16, length);
}

SafeString16::SafeString16(const sl_char16* strUtf16)
{
	m_container = String16::_create(strUtf16, -1);
}

SafeString16::SafeString16(const sl_char16* strUtf16, sl_reg length)
{
	m_container = String16::_create(strUtf16, length);
}


String8::String8(const sl_char32* strUtf32)
{
	m_container = _create(strUtf32, -1);
}

String8::String8(const sl_char32* strUtf32, sl_reg length)
{
	m_container = _create(strUtf32, length);
}

SafeString8::SafeString8(const sl_char32* strUtf32)
{
	m_container = String8::_create(strUtf32, -1);
}

SafeString8::SafeString8(const sl_char32* strUtf32, sl_reg length)
{
	m_container = String8::_create(strUtf32, length);
}

String16::String16(const sl_char32* strUtf32)
{
	m_container = _create(strUtf32, -1);
}

String16::String16(const sl_char32* strUtf32, sl_reg length)
{
	m_container = _create(strUtf32, length);
}

SafeString16::SafeString16(const sl_char32* strUtf32)
{
	m_container = String16::_create(strUtf32, -1);
}

SafeString16::SafeString16(const sl_char32* strUtf32, sl_reg length)
{
	m_container = String16::_create(strUtf32, length);
}


String8 String8::allocate(sl_size len)
{
	return _alloc(len);
}

String16 String16::allocate(sl_size len)
{
	return _alloc(len);
}


String8 String8::fromStatic(const sl_char8 *sz, sl_reg len)
{
	if (sz) {
		if (len == 0) {
			return _String8_Empty.container;
		}
		StringContainer8* container = (StringContainer8*)(Base::createMemory(sizeof(StringContainer8)));
		if (container) {
			container->sz = (sl_char8*)sz;
			container->len = len;
			container->hash = 0;
			container->ref = 0;
			return container;
		}
	}
	return _String8_Null.container;
}

String16 String16::fromStatic(const sl_char16 *sz, sl_reg len)
{
	if (sz) {
		if (len == 0) {
			return _String16_Empty.container;
		}
		StringContainer16* container = (StringContainer16*)(Base::createMemory(sizeof(StringContainer16)));
		if (container) {
			container->sz = (sl_char16*)sz;
			container->len = len;
			container->hash = 0;
			container->ref = 0;
			return container;
		}
	}
	return _String16_Null.container;
}


String8 String8::fromUtf8(const void* utf8, sl_reg len)
{
	return String8((const sl_char8*)utf8, len);
}

String8 String8::fromUtf8(const Memory& mem)
{
	return fromUtf8(mem.getData(), mem.getSize());
}

String16 String16::fromUtf8(const void* utf8, sl_reg len)
{
	return String16((const sl_char8*)utf8, len);
}

String16 String16::fromUtf8(const Memory& mem)
{
	return fromUtf8(mem.getData(), mem.getSize());
}


String8 String8::fromUtf16(const sl_char16* utf16, sl_reg len)
{
	return String8(utf16, len);
}

String16 String16::fromUtf16(const sl_char16* utf16, sl_reg len)
{
	String16 str(utf16, len);
	return str;
}


String8 String8::fromUtf32(const sl_char32* utf32, sl_reg len)
{
	return String8(utf32, len);
}

String16 String16::fromUtf32(const sl_char32* utf32, sl_reg len)
{
	String16 str(utf32, len);
	return str;
}


String8 String8::fromUtf16BE(const void* _utf16, sl_reg len)
{
	if (!_utf16) {
		return String::null();
	}
	if (len == 0) {
		return String::getEmpty();
	}
	if (((((sl_reg)(_utf16)) & 1) == 0) && Endian::isBE()) {
		return String8((sl_char16*)_utf16, len);
	}
	SLIB_SCOPED_BUFFER(sl_char16, 4096, utf16, len);
	if (utf16) {
		Base::copyMemory(utf16, _utf16, len<<1);
		if (Endian::isLE()) {
			for (sl_reg i = 0; i < len; i++) {
				utf16[i] = Endian::swap16(utf16[i]);
			}
		}
		return String8(utf16, len);
	}
	return String8::null();
}

String8 String8::fromUtf16BE(const Memory& mem)
{
	return fromUtf16BE(mem.getData(), mem.getSize());
}

String16 String16::fromUtf16BE(const void* _utf16, sl_reg len)
{
	if (!_utf16) {
		return String::null();
	}
	if (len == 0) {
		return String::getEmpty();
	}
	if (((((sl_reg)(_utf16)) & 1) == 0) && Endian::isBE()) {
		return String16((sl_char16*)_utf16, len);
	}
	String16 str = String16::allocate(len);
	if (str.isNotNull()) {
		sl_char16* utf16 = str.getData();
		const sl_uint8* c = (const sl_uint8*)_utf16;
		for (sl_reg i = 0; i < len; i++) {
			utf16[i] = MIO::readUint16BE(c);
			c += 2;
		}
		return str;
	}
	return String16::null();
}

String16 String16::fromUtf16BE(const Memory& mem)
{
	return fromUtf16BE(mem.getData(), mem.getSize());
}


String8 String8::fromUtf16LE(const void* _utf16, sl_reg len)
{
	if (!_utf16) {
		return String::null();
	}
	if (len == 0) {
		return String::getEmpty();
	}
	if ((((sl_reg)(_utf16)) & 1) == 0 && Endian::isLE()) {
		return String8((sl_char16*)_utf16, len);
	}
	SLIB_SCOPED_BUFFER(sl_char16, 4096, utf16, len);
	if (utf16) {
		Base::copyMemory(utf16, _utf16, len<<1);
		if (Endian::isBE()) {
			for (sl_reg i = 0; i < len; i++) {
				utf16[i] = Endian::swap16(utf16[i]);
			}
		}
		return String8(utf16, len);
	}
	return String8::null();
}

String8 String8::fromUtf16LE(const Memory& mem)
{
	return fromUtf16LE(mem.getData(), mem.getSize());
}

String16 String16::fromUtf16LE(const void* _utf16, sl_reg len)
{
	if (!_utf16) {
		return String::null();
	}
	if (len == 0) {
		return String::getEmpty();
	}
	if ((((sl_reg)(_utf16)) & 1) == 0 && Endian::isLE()) {
		return String16((sl_char16*)_utf16, len);
	}

	String16 str = String16::allocate(len);
	if (str.isNotNull()) {
		sl_char16* utf16 = str.getData();
		const sl_uint8* c = (const sl_uint8*)_utf16;
		for (sl_reg i = 0; i < len; i++) {
			utf16[i] = MIO::readUint16LE(c);
			c += 2;
		}
		return str;
	}
	return String16::null();
}

String16 String16::fromUtf16LE(const Memory& mem)
{
	return fromUtf16LE(mem.getData(), mem.getSize());
}


String8 String8::fromUtf(const void* _buf, sl_size len)
{
	sl_char8* buf = (sl_char8*)_buf;
	if (!buf) {
		return String8::null();
	}
	if (len == 0) {
		return String8::getEmpty();
	}
	if (len >= 2) {
		if (buf[0] == (sl_char8)0xFF && buf[1] == (sl_char8)0xFE) {
			return String8::fromUtf16LE(buf, (len - 2) >> 1);
		}
		if (buf[0] == (sl_char8)0xFE && buf[1] == (sl_char8)0xFF) {
			return String8::fromUtf16BE(buf, (len - 2) >> 1);
		}
	}
	if (len >= 3) {
		if (buf[0] == (sl_char8)0xEF && buf[1] == (sl_char8)0xBB && buf[2] == (sl_char8)0xBF) {
			return String8(buf, len - 3);
		}
	}
	return String8(buf, len);
}

String8 String8::fromUtf(const Memory& mem)
{
	return fromUtf(mem.getData(), mem.getSize());
}

String16 String16::fromUtf(const void* _buf, sl_size len)
{
	sl_char8* buf = (sl_char8*)_buf;
	if (!buf) {
		return String16::null();
	}
	if (len == 0) {
		return String16::getEmpty();
	}
	if (len >= 2) {
		if (buf[0] == (sl_char8)0xFF && buf[1] == (sl_char8)0xFE) {
			return String16::fromUtf16LE(buf, (len - 2) >> 1);
		}
		if (buf[0] == (sl_char8)0xFE && buf[1] == (sl_char8)0xFF) {
			return String16::fromUtf16BE(buf, (len - 2) >> 1);
		}
	}
	if (len >= 3) {
		if (buf[0] == (sl_char8)0xEF && buf[1] == (sl_char8)0xBB && buf[2] == (sl_char8)0xBF) {
			return String16(buf, len - 3);
		}
	}
	return String16(buf, len);
}

String16 String16::fromUtf(const Memory& mem)
{
	return fromUtf(mem.getData(), mem.getSize());
}


sl_bool SafeString8::isEmpty() const
{
	String8 s(*this);
	return s.getLength() == 0;
}

sl_bool SafeString16::isEmpty() const
{
	String16 s(*this);
	return s.getLength() == 0;
}


sl_bool SafeString8::isNotEmpty() const
{
	String8 s(*this);
	return s.getLength() != 0;
}

sl_bool SafeString16::isNotEmpty() const
{
	String16 s(*this);
	return s.getLength() != 0;
}


void String8::setNull()
{
	if (m_container != _String8_Null.container) {
		_replaceContainer(_String8_Null.container);
	}
}

void String16::setNull()
{
	if (m_container != _String16_Null.container) {
		_replaceContainer(_String16_Null.container);
	}
}

void SafeString8::setNull()
{
	if (m_container != _String8_Null.container) {
		_replaceContainer(_String8_Null.container);
	}
}

void SafeString16::setNull()
{
	if (m_container != _String16_Null.container) {
		_replaceContainer(_String16_Null.container);
	}
}


void String8::setEmpty()
{
	if (m_container != _String8_Empty.container) {
		_replaceContainer(_String8_Empty.container);
	}
}

void String16::setEmpty()
{
	if (m_container != _String16_Empty.container) {
		_replaceContainer(_String16_Empty.container);
	}
}

void SafeString8::setEmpty()
{
	if (m_container != _String8_Empty.container) {
		_replaceContainer(_String8_Empty.container);
	}
}

void SafeString16::setEmpty()
{
	if (m_container != _String16_Empty.container) {
		_replaceContainer(_String16_Empty.container);
	}
}


sl_size SafeString8::getLength() const
{
	String8 s(*this);
	return s.getLength();
}

sl_size SafeString16::getLength() const
{
	String16 s(*this);
	return s.getLength();
}


void String8::setLength(sl_size len)
{
	m_container->len = len;
}

void String16::setLength(sl_size len)
{
	m_container->len = len;
}


template <class CT>
SLIB_INLINE sl_uint32 _String_calcHash(const CT* buf, sl_size len)
{
	sl_uint32 hash = 0;
	for (sl_size i = 0; i < len; i++) {
		sl_uint32 ch = buf[i];
		hash = hash * 31 + ch;
	}
	hash = sl_rehash(hash);
	return hash;
}

sl_uint32 String8::getHashCode() const
{
	sl_size n = m_container->len;
	if (n == 0) {
		return 0;
	}
	sl_uint32 hash = m_container->hash;
	if (hash == 0) {
		hash = _String_calcHash(m_container->sz, n);
		m_container->hash = hash;
	}
	return hash;
}

sl_uint32 String16::getHashCode() const
{
	sl_size n = m_container->len;
	if (n == 0) {
		return 0;
	}
	sl_uint32 hash = m_container->hash;
	if (hash == 0) {
		hash = _String_calcHash(m_container->sz, n);
		m_container->hash = hash;
	}
	return hash;
}

sl_uint32 SafeString8::getHashCode() const
{
	String8 s(*this);
	return s.getHashCode();
}

sl_uint32 SafeString16::getHashCode() const
{
	String16 s(*this);
	return s.getHashCode();
}


void String8::setHashCode(sl_uint32 hash)
{
	m_container->hash = hash;
}

void String16::setHashCode(sl_uint32 hash)
{
	m_container->hash = hash;
}


sl_char8 String8::getAt(sl_reg index) const
{
	if (index >= 0 && index < (sl_reg)(m_container->len)) {
		return m_container->sz[index];
	}
	return 0;
}

sl_char16 String16::getAt(sl_reg index) const
{
	if (index >= 0 && index < (sl_reg)(m_container->len)) {
		return m_container->sz[index];
	}
	return 0;
}


sl_bool String8::setAt(sl_reg index, sl_char8 ch)
{
	if (index >= 0 && index < (sl_reg)(m_container->len)) {
		m_container->sz[index] = ch;
		return sl_true;
	}
	return sl_false;
}

sl_bool String16::setAt(sl_reg index, sl_char16 ch)
{
	if (index >= 0 && index < (sl_reg)(m_container->len)) {
		m_container->sz[index] = ch;
		return sl_true;
	}
	return sl_false;
}


String8& String8::operator=(String8&& other)
{
	if (this != &other) {
		_replaceContainer(other.m_container);
		other.m_container = _String8_Null.container;
	}
	return *this;
}

String16& String16::operator=(String16&& other)
{
	if (this != &other) {
		_replaceContainer(other.m_container);
		other.m_container = _String16_Null.container;
	}
	return *this;
}

SafeString8& SafeString8::operator=(SafeString8&& other)
{
	if (this != &other) {
		_replaceContainer(other.m_container);
		other.m_container = _String8_Null.container;
	}
	return *this;
}

SafeString16& SafeString16::operator=(SafeString16&& other)
{
	if (this != &other) {
		_replaceContainer(other.m_container);
		other.m_container = _String16_Null.container;
	}
	return *this;
}


String8& String8::operator=(SafeString8&& other)
{
	if ((void*)this != (void*)(&other)) {
		_replaceContainer(other.m_container);
		other.m_container = _String8_Null.container;
	}
	return *this;
}

String16& String16::operator=(SafeString16&& other)
{
	if ((void*)this != (void*)(&other)) {
		_replaceContainer(other.m_container);
		other.m_container = _String16_Null.container;
	}
	return *this;
}

SafeString8& SafeString8::operator=(String8&& other)
{
	if ((void*)this != (void*)(&other)) {
		_replaceContainer(other.m_container);
		other.m_container = _String8_Null.container;
	}
	return *this;
}

SafeString16& SafeString16::operator=(String16&& other)
{
	if ((void*)this != (void*)(&other)) {
		_replaceContainer(other.m_container);
		other.m_container = _String16_Null.container;
	}
	return *this;
}


String8& String8::operator=(const String8& other)
{
	StringContainer8* container = other.m_container;
	if (m_container != container) {
		container->increaseReference();
		_replaceContainer(container);
	}
	return *this;
}

String16& String16::operator=(const String16& other)
{
	StringContainer16* container = other.m_container;
	if (m_container != container) {
		container->increaseReference();
		_replaceContainer(container);
	}
	return *this;
}

SafeString8& SafeString8::operator=(const String8& other)
{
	StringContainer8* container = other.m_container;
	if (m_container != container) {
		container->increaseReference();
		_replaceContainer(container);
	}
	return *this;
}

SafeString16& SafeString16::operator=(const String16& other)
{
	StringContainer16* container = other.m_container;
	if (m_container != container) {
		container->increaseReference();
		_replaceContainer(container);
	}
	return *this;
}


String8& String8::operator=(const SafeString8& other)
{
	if (m_container != other.m_container) {
		_replaceContainer(other._retainContainer());
	}
	return *this;
}

String16& String16::operator=(const SafeString16& other)
{
	if (m_container != other.m_container) {
		_replaceContainer(other._retainContainer());
	}
	return *this;
}

SafeString8& SafeString8::operator=(const SafeString8& other)
{
	if (m_container != other.m_container) {
		_replaceContainer(other._retainContainer());
	}
	return *this;
}

SafeString16& SafeString16::operator=(const SafeString16& other)
{
	if (m_container != other.m_container) {
		_replaceContainer(other._retainContainer());
	}
	return *this;
}


String8& String8::operator=(const String16& other)
{
	if (other.isNull()) {
		setNull();
	} else {
		sl_size len = other.getLength();
		if (len == 0) {
			setEmpty();
		} else {
			_replaceContainer(_create(other.getData(), len));
		}
	}
	return *this;
}

String16& String16::operator=(const String8& other)
{
	if (other.isNull()) {
		setNull();
	} else {
		sl_size len = other.getLength();
		if (len == 0) {
			setEmpty();
		} else {
			_replaceContainer(_create(other.getData(), len));
		}
	}
	return *this;
}

SafeString8& SafeString8::operator=(const String16& other)
{
	if (other.isNull()) {
		setNull();
	} else {
		sl_size len = other.getLength();
		if (len == 0) {
			setEmpty();
		} else {
			_replaceContainer(String8::_create(other.getData(), len));
		}
	}
	return *this;
}

SafeString16& SafeString16::operator=(const String8& other)
{
	if (other.isNull()) {
		setNull();
	} else {
		sl_size len = other.getLength();
		if (len == 0) {
			setEmpty();
		} else {
			_replaceContainer(String16::_create(other.getData(), len));
		}
	}
	return *this;
}


String8& String8::operator=(const SafeString16& _other)
{
	String16 other(_other);
	if (other.isNull()) {
		setNull();
	} else {
		sl_size len = other.getLength();
		if (len == 0) {
			setEmpty();
		} else {
			_replaceContainer(_create(other.getData(), len));
		}
	}
	return *this;
}

String16& String16::operator=(const SafeString8& _other)
{
	String8 other(_other);
	if (other.isNull()) {
		setNull();
	} else {
		sl_size len = other.getLength();
		if (len == 0) {
			setEmpty();
		} else {
			_replaceContainer(_create(other.getData(), len));
		}
	}
	return *this;
}

SafeString8& SafeString8::operator=(const SafeString16& _other)
{
	String16 other(_other);
	if (other.isNull()) {
		setNull();
	} else {
		sl_size len = other.getLength();
		if (len == 0) {
			setEmpty();
		} else {
			_replaceContainer(String8::_create(other.getData(), len));
		}
	}
	return *this;
}

SafeString16& SafeString16::operator=(const SafeString8& _other)
{
	String8 other(_other);
	if (other.isNull()) {
		setNull();
	} else {
		sl_size len = other.getLength();
		if (len == 0) {
			setEmpty();
		} else {
			_replaceContainer(String16::_create(other.getData(), len));
		}
	}
	return *this;
}


String8& String8::operator=(const sl_char8* utf8)
{
	if (utf8) {
		_replaceContainer(_create(utf8, -1));
	} else {
		setNull();
	}
	return *this;
}

String16& String16::operator=(const sl_char8* utf8)
{
	if (utf8) {
		_replaceContainer(_create(utf8, -1));
	} else {
		setNull();
	}
	return *this;
}

SafeString8& SafeString8::operator=(const sl_char8* utf8)
{
	if (utf8) {
		_replaceContainer(String8::_create(utf8, -1));
	} else {
		setNull();
	}
	return *this;
}

SafeString16& SafeString16::operator=(const sl_char8* utf8)
{
	if (utf8) {
		_replaceContainer(String16::_create(utf8, -1));
	} else {
		setNull();
	}
	return *this;
}


String8& String8::operator=(const sl_char16* utf16)
{
	if (utf16) {
		_replaceContainer(_create(utf16, -1));
	} else {
		setNull();
	}
	return *this;
}

String16& String16::operator=(const sl_char16* utf16)
{
	if (utf16) {
		_replaceContainer(_create(utf16, -1));
	} else {
		setNull();
	}
	return *this;
}

SafeString8& SafeString8::operator=(const sl_char16* utf16)
{
	if (utf16) {
		_replaceContainer(String8::_create(utf16, -1));
	} else {
		setNull();
	}
	return *this;
}

SafeString16& SafeString16::operator=(const sl_char16* utf16)
{
	if (utf16) {
		_replaceContainer(String16::_create(utf16, -1));
	} else {
		setNull();
	}
	return *this;
}


String8& String8::operator=(const sl_char32* utf32)
{
	if (utf32) {
		_replaceContainer(_create(utf32, -1));
	} else {
		setNull();
	}
	return *this;
}

String16& String16::operator=(const sl_char32* utf32)
{
	if (utf32) {
		_replaceContainer(_create(utf32, -1));
	} else {
		setNull();
	}
	return *this;
}

SafeString8& SafeString8::operator=(const sl_char32* utf32)
{
	if (utf32) {
		_replaceContainer(String8::_create(utf32, -1));
	} else {
		setNull();
	}
	return *this;
}

SafeString16& SafeString16::operator=(const sl_char32* utf32)
{
	if (utf32) {
		_replaceContainer(String16::_create(utf32, -1));
	} else {
		setNull();
	}
	return *this;
}


String8 String8::operator+(const String8& other) const
{
	sl_size n = getLength();
	if (n == 0) {
		return other.getNotNull();
	}
	if (other.isNotEmpty()) {
		return _merge8(getData(), n, other.getData(), other.getLength());
	}
	return getNotNull();
}

String8& String8::operator+=(const String8& other)
{
	sl_size n = getLength();
	if (n == 0) {
		return (*this = other.getNotNull());
	}
	if (other.isNotEmpty()) {
		_replaceContainer(_merge8(getData(), n, other.getData(), other.getLength()));
	}
	return *this;
}

String16 String16::operator+(const String16& other) const
{
	sl_size n = getLength();
	if (n == 0) {
		return other.getNotNull();
	}
	if (other.isNotEmpty()) {
		return _merge16(getData(), n, other.getData(), other.getLength());
	}
	return getNotNull();
}

String16& String16::operator+=(const String16& other)
{
	sl_size n = getLength();
	if (n == 0) {
		return (*this = other.getNotNull());
	}
	if (other.isNotEmpty()) {
		_replaceContainer(_merge16(getData(), n, other.getData(), other.getLength()));
	}
	return *this;
}

String8 SafeString8::operator+(const String8& other) const
{
	String8 s(*this);
	sl_size n = s.getLength();
	if (n == 0) {
		return other.getNotNull();
	}
	if (other.isNotEmpty()) {
		return String8::_merge8(s.getData(), n, other.getData(), other.getLength());
	}
	return s.getNotNull();
}

SafeString8& SafeString8::operator+=(const String8& other)
{
	String8 s(*this);
	sl_size n = s.getLength();
	if (n == 0) {
		return (*this = other.getNotNull());
	}
	if (other.isNotEmpty()) {
		_replaceContainer(String8::_merge8(s.getData(), n, other.getData(), other.getLength()));
	}
	return *this;
}

String16 SafeString16::operator+(const String16& other) const
{
	String16 s(*this);
	sl_size n = s.getLength();
	if (n == 0) {
		return other.getNotNull();
	}
	if (other.isNotEmpty()) {
		return String16::_merge16(s.getData(), n, other.getData(), other.getLength());
	}
	return s.getNotNull();
}

SafeString16& SafeString16::operator+=(const String16& other)
{
	String16 s(*this);
	sl_size n = s.getLength();
	if (n == 0) {
		return (*this = other.getNotNull());
	}
	if (other.isNotEmpty()) {
		_replaceContainer(String16::_merge16(s.getData(), n, other.getData(), other.getLength()));
	}
	return *this;
}


String8 String8::operator+(const String16& other) const
{
	if (other.isNotEmpty()) {
		return _merge16(getData(), getLength(), other.getData(), other.getLength());
	}
	return getNotNull();
}

String8& String8::operator+=(const String16& other)
{
	if (other.isNotEmpty()) {
		_replaceContainer(_merge16(getData(), getLength(), other.getData(), other.getLength()));
	}
	return *this;
}

String16 String16::operator+(const String8& other) const
{
	if (other.isNotEmpty()) {
		return _merge8(getData(), getLength(), other.getData(), other.getLength());
	}
	return getNotNull();
}

String16& String16::operator+=(const String8& other)
{
	if (other.isNotEmpty()) {
		_replaceContainer(_merge8(getData(), getLength(), other.getData(), other.getLength()));
	}
	return *this;
}

String8 SafeString8::operator+(const String16& other) const
{
	String8 s(*this);
	if (other.isNotEmpty()) {
		return String8::_merge16(s.getData(), s.getLength(), other.getData(), other.getLength());
	}
	return s.getNotNull();
}

SafeString8& SafeString8::operator+=(const String16& other)
{
	if (other.isNotEmpty()) {
		String8 s(*this);
		_replaceContainer(String8::_merge16(s.getData(), s.getLength(), other.getData(), other.getLength()));
	}
	return *this;
}

String16 SafeString16::operator+(const String8& other) const
{
	String16 s(*this);
	if (other.isNotEmpty()) {
		return String16::_merge8(s.getData(), s.getLength(), other.getData(), other.getLength());
	}
	return s.getNotNull();
}

SafeString16& SafeString16::operator+=(const String8& other)
{
	if (other.isNotEmpty()) {
		String16 s(*this);
		_replaceContainer(String16::_merge8(s.getData(), s.getLength(), other.getData(), other.getLength()));
	}
	return *this;
}


String8 String8::operator+(const SafeString8& _other) const
{
	sl_size n = getLength();
	String8 other(_other);
	if (n == 0) {
		return other.getNotNull();
	}
	if (other.isNotEmpty()) {
		return _merge8(getData(), n, other.getData(), other.getLength());
	}
	return getNotNull();
}

String8& String8::operator+=(const SafeString8& _other)
{
	sl_size n = getLength();
	String8 other(_other);
	if (n == 0) {
		return (*this = other.getNotNull());
	}
	if (other.isNotEmpty()) {
		_replaceContainer(_merge8(getData(), n, other.getData(), other.getLength()));
	}
	return *this;
}

String16 String16::operator+(const SafeString16& _other) const
{
	sl_size n = getLength();
	String16 other(_other);
	if (n == 0) {
		return other.getNotNull();
	}
	if (other.isNotEmpty()) {
		return _merge16(getData(), n, other.getData(), other.getLength());
	}
	return getNotNull();
}

String16& String16::operator+=(const SafeString16& _other)
{
	sl_size n = getLength();
	String16 other(_other);
	if (n == 0) {
		return (*this = other.getNotNull());
	}
	if (other.isNotEmpty()) {
		_replaceContainer(_merge16(getData(), n, other.getData(), other.getLength()));
	}
	return *this;
}

String8 SafeString8::operator+(const SafeString8& _other) const
{
	String8 s(*this);
	sl_size n = s.getLength();
	String8 other(_other);
	if (n == 0) {
		return other.getNotNull();
	}
	if (other.isNotEmpty()) {
		return String8::_merge8(s.getData(), n, other.getData(), other.getLength());
	}
	return s.getNotNull();
}

SafeString8& SafeString8::operator+=(const SafeString8& _other)
{
	String8 s(*this);
	sl_size n = s.getLength();
	String8 other(_other);
	if (n == 0) {
		return (*this = other.getNotNull());
	}
	if (other.isNotEmpty()) {
		_replaceContainer(String8::_merge8(s.getData(), n, other.getData(), other.getLength()));
	}
	return *this;
}

String16 SafeString16::operator+(const SafeString16& _other) const
{
	String16 s(*this);
	sl_size n = s.getLength();
	String16 other(_other);
	if (n == 0) {
		return other.getNotNull();
	}
	if (other.isNotEmpty()) {
		return String16::_merge16(s.getData(), n, other.getData(), other.getLength());
	}
	return s.getNotNull();
}

SafeString16& SafeString16::operator+=(const SafeString16& _other)
{
	String16 s(*this);
	sl_size n = s.getLength();
	String16 other(_other);
	if (n == 0) {
		return (*this = other.getNotNull());
	}
	if (other.isNotEmpty()) {
		_replaceContainer(String16::_merge16(s.getData(), n, other.getData(), other.getLength()));
	}
	return *this;
}


String8 String8::operator+(const SafeString16& _other) const
{
	String16 other(_other);
	if (other.isNotEmpty()) {
		return _merge16(getData(), getLength(), other.getData(), other.getLength());
	}
	return getNotNull();
}

String8& String8::operator+=(const SafeString16& _other)
{
	String16 other(_other);
	if (other.isNotEmpty()) {
		_replaceContainer(_merge16(getData(), getLength(), other.getData(), other.getLength()));
	}
	return *this;
}

String16 String16::operator+(const SafeString8& _other) const
{
	String8 other(_other);
	if (other.isNotEmpty()) {
		return _merge8(getData(), getLength(), other.getData(), other.getLength());
	}
	return getNotNull();
}

String16& String16::operator+=(const SafeString8& _other)
{
	String8 other(_other);
	if (other.isNotEmpty()) {
		_replaceContainer(_merge8(getData(), getLength(), other.getData(), other.getLength()));
	}
	return *this;
}

String8 SafeString8::operator+(const SafeString16& _other) const
{
	String8 s(*this);
	String16 other(_other);
	if (other.isNotEmpty()) {
		return String8::_merge16(s.getData(), s.getLength(), other.getData(), other.getLength());
	}
	return s.getNotNull();
}

SafeString8& SafeString8::operator+=(const SafeString16& _other)
{
	String16 other(_other);
	if (other.isNotEmpty()) {
		String8 s(*this);
		_replaceContainer(String8::_merge16(s.getData(), s.getLength(), other.getData(), other.getLength()));
	}
	return *this;
}

String16 SafeString16::operator+(const SafeString8& _other) const
{
	String16 s(*this);
	String8 other(_other);
	if (other.isNotEmpty()) {
		return String16::_merge8(s.getData(), s.getLength(), other.getData(), other.getLength());
	}
	return s.getNotNull();
}

SafeString16& SafeString16::operator+=(const SafeString8& _other)
{
	String16 s(*this);
	String8 other(_other);
	if (other.isNotEmpty()) {
		_replaceContainer(String16::_merge8(s.getData(), s.getLength(), other.getData(), other.getLength()));
	}
	return *this;
}


String8 String8::operator+(const sl_char8* utf8) const
{
	if (utf8) {
		return _merge8(getData(), getLength(), utf8, -1);
	}
	return getNotNull();
}

String8& String8::operator+=(const sl_char8* utf8)
{
	if (utf8) {
		_replaceContainer(_merge8(getData(), getLength(), utf8, -1));
	}
	return *this;
}

String16 String16::operator+(const sl_char8* utf8) const
{
	if (utf8) {
		return _merge8(getData(), getLength(), utf8, -1);
	}
	return getNotNull();
}

String16& String16::operator+=(const sl_char8* utf8)
{
	if (utf8) {
		_replaceContainer(_merge8(getData(), getLength(), utf8, -1));
	}
	return *this;
}

String8 SafeString8::operator+(const sl_char8* utf8) const
{
	String8 s(*this);
	if (utf8) {
		return String8::_merge8(s.getData(), s.getLength(), utf8, -1);
	}
	return s.getNotNull();
}

SafeString8& SafeString8::operator+=(const sl_char8* utf8)
{
	String8 s(*this);
	if (utf8) {
		_replaceContainer(String8::_merge8(s.getData(), s.getLength(), utf8, -1));
	}
	return *this;
}

String16 SafeString16::operator+(const sl_char8* utf8) const
{
	String16 s(*this);
	if (utf8) {
		return String16::_merge8(s.getData(), s.getLength(), utf8, -1);
	}
	return s.getNotNull();
}

SafeString16& SafeString16::operator+=(const sl_char8* utf8)
{
	String16 s(*this);
	if (utf8) {
		_replaceContainer(String16::_merge8(s.getData(), s.getLength(), utf8, -1));
	}
	return *this;
}


String8 String8::operator+(const sl_char16* utf16) const
{
	if (utf16) {
		return _merge16(getData(), getLength(), utf16, -1);
	}
	return getNotNull();
}

String8& String8::operator+=(const sl_char16* utf16)
{
	if (utf16) {
		_replaceContainer(_merge16(getData(), getLength(), utf16, -1));
	}
	return *this;
}

String16 String16::operator+(const sl_char16* utf16) const
{
	if (utf16) {
		return _merge16(getData(), getLength(), utf16, -1);
	}
	return getNotNull();
}

String16& String16::operator+=(const sl_char16* utf16)
{
	if (utf16) {
		_replaceContainer(_merge16(getData(), getLength(), utf16, -1));
	}
	return *this;
}

String8 SafeString8::operator+(const sl_char16* utf16) const
{
	String8 s(*this);
	if (utf16) {
		return String8::_merge16(s.getData(), s.getLength(), utf16, -1);
	}
	return s.getNotNull();
}

SafeString8& SafeString8::operator+=(const sl_char16* utf16)
{
	String8 s(*this);
	if (utf16) {
		_replaceContainer(String8::_merge16(s.getData(), s.getLength(), utf16, -1));
	}
	return *this;
}

String16 SafeString16::operator+(const sl_char16* utf16) const
{
	String16 s(*this);
	if (utf16) {
		return String16::_merge16(s.getData(), s.getLength(), utf16, -1);
	}
	return s.getNotNull();
}

SafeString16& SafeString16::operator+=(const sl_char16* utf16)
{
	String16 s(*this);
	if (utf16) {
		_replaceContainer(String16::_merge16(s.getData(), s.getLength(), utf16, -1));
	}
	return *this;
}


String8 String8::operator+(const sl_char32* utf32) const
{
	if (utf32) {
		return _merge32(getData(), getLength(), utf32, -1);
	}
	return getNotNull();
}

String8& String8::operator+=(const sl_char32* utf32)
{
	if (utf32) {
		_replaceContainer(_merge32(getData(), getLength(), utf32, -1));
	}
	return *this;
}

String16 String16::operator+(const sl_char32* utf32) const
{
	if (utf32) {
		return _merge32(getData(), getLength(), utf32, -1);
	}
	return getNotNull();
}

String16& String16::operator+=(const sl_char32* utf32)
{
	if (utf32) {
		_replaceContainer(_merge32(getData(), getLength(), utf32, -1));
	}
	return *this;
}

String8 SafeString8::operator+(const sl_char32* utf32) const
{
	String8 s(*this);
	if (utf32) {
		return String8::_merge32(s.getData(), s.getLength(), utf32, -1);
	}
	return s.getNotNull();
}

SafeString8& SafeString8::operator+=(const sl_char32* utf32)
{
	String8 s(*this);
	if (utf32) {
		_replaceContainer(String8::_merge32(s.getData(), s.getLength(), utf32, -1));
	}
	return *this;
}

String16 SafeString16::operator+(const sl_char32* utf32) const
{
	String16 s(*this);
	if (utf32) {
		return String16::_merge32(s.getData(), s.getLength(), utf32, -1);
	}
	return s.getNotNull();
}

SafeString16& SafeString16::operator+=(const sl_char32* utf32)
{
	String16 s(*this);
	if (utf32) {
		_replaceContainer(String16::_merge32(s.getData(), s.getLength(), utf32, -1));
	}
	return *this;
}


String8 operator+(const sl_char8* utf8, const String8& s)
{
	if (utf8) {
		return String8::_merge8(utf8, -1, s.getData(), s.getLength());
	}
	return s.getNotNull();
}

String16 operator+(const sl_char8* utf8, const String16& s)
{
	if (utf8) {
		return String16::_merge8(utf8, -1, s.getData(), s.getLength());
	}
	return s.getNotNull();
}

String8 operator+(const sl_char8* utf8, const SafeString8& s)
{
	return utf8 + String8(s);
}

String16 operator+(const sl_char8* utf8, const SafeString16& s)
{
	return utf8 + String16(s);
}


String8 operator+(const sl_char16* utf16, const String8& s)
{
	if (utf16) {
		return String8::_merge16(utf16, -1, s.getData(), s.getLength());
	}
	return s.getNotNull();
}

String16 operator+(const sl_char16* utf16, const String16& s)
{
	if (utf16) {
		return String16::_merge16(utf16, -1, s.getData(), s.getLength());
	}
	return s.getNotNull();
}

String8 operator+(const sl_char16* utf16, const SafeString8& s)
{
	return utf16 + String8(s);
}

String16 operator+(const sl_char16* utf16, const SafeString16& s)
{
	return utf16 + String16(s);
}


String8 operator+(const sl_char32* utf32, const String8& s)
{
	if (utf32) {
		return String8::_merge32(utf32, -1, s.getData(), s.getLength());
	}
	return s.getNotNull();
}

String16 operator+(const sl_char32* utf32, const String16& s)
{
	if (utf32) {
		return String16::_merge32(utf32, -1, s.getData(), s.getLength());
	}
	return s.getNotNull();
}

String8 operator+(const sl_char32* utf32, const SafeString8& s)
{
	return utf32 + String8(s);
}

String16 operator+(const sl_char32* utf32, const SafeString16& s)
{
	return utf32 + String16(s);
}


String8 String8::operator+(sl_int32 number) const
{
	return *this + String8::fromInt32(number);
}

String8& String8::operator+=(sl_int32 number)
{
	return (*this += String8::fromInt32(number));
}

String8 operator+(sl_int32 number, const String8& other)
{
	return String8::fromInt32(number) + other;
}

String16 String16::operator+(sl_int32 number) const
{
	return *this + String16::fromInt32(number);
}

String16& String16::operator+=(sl_int32 number)
{
	return (*this += String16::fromInt32(number));
}

String16 operator+(sl_int32 number, const String16& other)
{
	return String16::fromInt32(number) + other;
}

String8 SafeString8::operator+(sl_int32 number) const
{
	return *this + String8::fromInt32(number);
}

SafeString8& SafeString8::operator+=(sl_int32 number)
{
	return (*this += String8::fromInt32(number));
}

String8 operator+(sl_int32 number, const SafeString8& other)
{
	return String8::fromInt32(number) + other;
}

String16 SafeString16::operator+(sl_int32 number) const
{
	return *this + String16::fromInt32(number);
}

SafeString16& SafeString16::operator+=(sl_int32 number)
{
	return (*this += String16::fromInt32(number));
}

String16 operator+(sl_int32 number, const SafeString16& other)
{
	return String16::fromInt32(number) + other;
}


String8 String8::operator+(sl_uint32 number) const
{
	return *this + String8::fromUint32(number);
}

String8& String8::operator+=(sl_uint32 number)
{
	return (*this += String8::fromUint32(number));
}

String8 operator+(sl_uint32 number, const String8& other)
{
	return String8::fromUint32(number) + other;
}

String16 String16::operator+(sl_uint32 number) const
{
	return *this + String16::fromUint32(number);
}

String16& String16::operator+=(sl_uint32 number)
{
	return (*this += String16::fromUint32(number));
}

String16 operator+(sl_uint32 number, const String16& other)
{
	return String16::fromUint32(number) + other;
}

String8 SafeString8::operator+(sl_uint32 number) const
{
	return *this + String8::fromUint32(number);
}

SafeString8& SafeString8::operator+=(sl_uint32 number)
{
	return (*this += String8::fromUint32(number));
}

String8 operator+(sl_uint32 number, const SafeString8& other)
{
	return String8::fromUint32(number) + other;
}

String16 SafeString16::operator+(sl_uint32 number) const
{
	return *this + String16::fromUint32(number);
}

SafeString16& SafeString16::operator+=(sl_uint32 number)
{
	return (*this += String16::fromUint32(number));
}

String16 operator+(sl_uint32 number, const SafeString16& other)
{
	return String16::fromUint32(number) + other;
}


String8 String8::operator+(sl_int64 number) const
{
	return *this + String8::fromInt64(number);
}

String8& String8::operator+=(sl_int64 number)
{
	return (*this += String8::fromInt64(number));
}

String8 operator+(sl_int64 number, const String8& other)
{
	return String8::fromInt64(number) + other;
}

String16 String16::operator+(sl_int64 number) const
{
	return *this + String16::fromInt64(number);
}

String16& String16::operator+=(sl_int64 number)
{
	return (*this += String16::fromInt64(number));
}

String16 operator+(sl_int64 number, const String16& other)
{
	return String16::fromInt64(number) + other;
}

String8 SafeString8::operator+(sl_int64 number) const
{
	return *this + String8::fromInt64(number);
}

SafeString8& SafeString8::operator+=(sl_int64 number)
{
	return (*this += String8::fromInt64(number));
}

String8 operator+(sl_int64 number, const SafeString8& other)
{
	return String8::fromInt64(number) + other;
}

String16 SafeString16::operator+(sl_int64 number) const
{
	return *this + String16::fromInt64(number);
}

SafeString16& SafeString16::operator+=(sl_int64 number)
{
	return (*this += String16::fromInt64(number));
}

String16 operator+(sl_int64 number, const SafeString16& other)
{
	return String16::fromInt64(number) + other;
}


String8 String8::operator+(sl_uint64 number) const
{
	return *this + String8::fromUint64(number);
}

String8& String8::operator+=(sl_uint64 number)
{
	return (*this += String8::fromUint64(number));
}

String8 operator+(sl_uint64 number, const String8& other)
{
	return String8::fromUint64(number) + other;
}

String16 String16::operator+(sl_uint64 number) const
{
	return *this + String16::fromUint64(number);
}

String16& String16::operator+=(sl_uint64 number)
{
	return (*this += String16::fromUint64(number));
}

String16 operator+(sl_uint64 number, const String16& other)
{
	return String16::fromUint64(number) + other;
}

String8 SafeString8::operator+(sl_uint64 number) const
{
	return *this + String8::fromUint64(number);
}

SafeString8& SafeString8::operator+=(sl_uint64 number)
{
	return (*this += String8::fromUint64(number));
}

String8 operator+(sl_uint64 number, const SafeString8& other)
{
	return String8::fromUint64(number) + other;
}

String16 SafeString16::operator+(sl_uint64 number) const
{
	return *this + String16::fromUint64(number);
}

SafeString16& SafeString16::operator+=(sl_uint64 number)
{
	return (*this += String16::fromUint64(number));
}

String16 operator+(sl_uint64 number, const SafeString16& other)
{
	return String16::fromUint64(number) + other;
}


String8 String8::operator+(float number) const
{
	return *this + String8::fromFloat(number);
}

String8& String8::operator+=(float number)
{
	return (*this += String8::fromFloat(number));
}

String8 operator+(float number, const String8& other)
{
	return String8::fromFloat(number) + other;
}

String16 String16::operator+(float number) const
{
	return *this + String16::fromFloat(number);
}

String16& String16::operator+=(float number)
{
	return (*this += String16::fromFloat(number));
}

String16 operator+(float number, const String16& other)
{
	return String16::fromFloat(number) + other;
}

String8 SafeString8::operator+(float number) const
{
	return *this + String8::fromFloat(number);
}

SafeString8& SafeString8::operator+=(float number)
{
	return (*this += String8::fromFloat(number));
}

String8 operator+(float number, const SafeString8& other)
{
	return String8::fromFloat(number) + other;
}

String16 SafeString16::operator+(float number) const
{
	return *this + String16::fromFloat(number);
}

SafeString16& SafeString16::operator+=(float number)
{
	return (*this += String16::fromFloat(number));
}

String16 operator+(float number, const SafeString16& other)
{
	return String16::fromFloat(number) + other;
}


String8 String8::operator+(double number) const
{
	return *this + String8::fromDouble(number);
}

String8& String8::operator+=(double number)
{
	return (*this += String8::fromDouble(number));
}

String8 operator+(double number, const String8& other)
{
	return String8::fromDouble(number) + other;
}

String16 String16::operator+(double number) const
{
	return *this + String16::fromDouble(number);
}

String16& String16::operator+=(double number)
{
	return (*this += String16::fromDouble(number));
}

String16 operator+(double number, const String16& other)
{
	return String16::fromDouble(number) + other;
}

String8 SafeString8::operator+(double number) const
{
	return *this + String8::fromDouble(number);
}

SafeString8& SafeString8::operator+=(double number)
{
	return (*this += String8::fromDouble(number));
}

String8 operator+(double number, const SafeString8& other)
{
	return String8::fromDouble(number) + other;
}

String16 SafeString16::operator+(double number) const
{
	return *this + String16::fromDouble(number);
}

SafeString16& SafeString16::operator+=(double number)
{
	return (*this += String16::fromDouble(number));
}

String16 operator+(double number, const SafeString16& other)
{
	return String16::fromDouble(number) + other;
}


String8 String8::operator+(sl_bool value) const
{
	return *this + String8::fromBoolean(value);
}

String8& String8::operator+=(sl_bool value)
{
	return (*this += String8::fromBoolean(value));
}

String8 operator+(sl_bool value, const String8& other)
{
	return String8::fromBoolean(value) + other;
}

String16 String16::operator+(sl_bool value) const
{
	return *this + String16::fromBoolean(value);
}

String16& String16::operator+=(sl_bool value)
{
	return (*this += String16::fromBoolean(value));
}

String16 operator+(sl_bool value, const String16& other)
{
	return String8::fromBoolean(value) + other;
}

String8 SafeString8::operator+(sl_bool value) const
{
	return *this + String8::fromBoolean(value);
}

SafeString8& SafeString8::operator+=(sl_bool value)
{
	return (*this += String8::fromBoolean(value));
}

String8 operator+(sl_bool value, const SafeString8& other)
{
	return String8::fromBoolean(value) + other;
}

String16 SafeString16::operator+(sl_bool value) const
{
	return *this + String16::fromBoolean(value);
}

SafeString16& SafeString16::operator+=(sl_bool value)
{
	return (*this += String16::fromBoolean(value));
}

String16 operator+(sl_bool value, const SafeString16& other)
{
	return String8::fromBoolean(value) + other;
}


sl_bool String8::equals(const String8& other) const
{
	sl_char8* s1 = getData();
	sl_char8* s2 = other.getData();
	if (s1 == s2) {
		return sl_true;
	}
	sl_size l1 = getLength();
	sl_size l2 = other.getLength();
	if (l1 != l2) {
		return sl_false;
	}
	return _equals8(s1, l1, s2, l2);
}

sl_bool String16::equals(const String16& other) const
{
	sl_char16* s1 = getData();
	sl_char16* s2 = other.getData();
	if (s1 == s2) {
		return sl_true;
	}
	sl_size l1 = getLength();
	sl_size l2 = other.getLength();
	if (l1 != l2) {
		return sl_false;
	}
	return _equals16(s1, l1, s2, l2);
}

sl_bool SafeString8::equals(const String8& other) const
{
	String8 s(*this);
	return s.equals(other);
}

sl_bool SafeString16::equals(const String16& other) const
{
	String16 s(*this);
	return s.equals(other);
}


sl_bool String8::equals(const String16& _other) const
{
	String8 other(_other);
	sl_size l1 = getLength();
	sl_size l2 = other.getLength();
	if (l1 != l2) {
		return sl_false;
	}
	sl_char8* s1 = getData();
	sl_char8* s2 = other.getData();
	return _equals8(s1, l1, s2, l2);
}

sl_bool String16::equals(const String8& _other) const
{
	String16 other(_other);
	sl_size l1 = getLength();
	sl_size l2 = other.getLength();
	if (l1 != l2) {
		return sl_false;
	}
	sl_char16* s1 = getData();
	sl_char16* s2 = other.getData();
	return _equals16(s1, l1, s2, l2);
}

sl_bool SafeString8::equals(const String16& other) const
{
	String8 s(*this);
	return s.equals(other);
}

sl_bool SafeString16::equals(const String8& other) const
{
	String16 s(*this);
	return s.equals(other);
}


sl_bool String8::equals(const SafeString8& _other) const
{
	String8 other(_other);
	return equals(other);
}

sl_bool String16::equals(const SafeString16& _other) const
{
	String16 other(_other);
	return equals(other);
}

sl_bool SafeString8::equals(const SafeString8& _other) const
{
	String8 s(*this);
	String8 other(_other);
	return s.equals(other);
}

sl_bool SafeString16::equals(const SafeString16& _other) const
{
	String16 s(*this);
	String16 other(_other);
	return s.equals(other);
}


sl_bool String8::equals(const SafeString16& _other) const
{
	String16 other(_other);
	return equals(other);
}

sl_bool String16::equals(const SafeString8& _other) const
{
	String8 other(_other);
	return equals(other);
}

sl_bool SafeString8::equals(const SafeString16& _other) const
{
	String8 s(*this);
	String16 other(_other);
	return s.equals(other);
}

sl_bool SafeString16::equals(const SafeString8& _other) const
{
	String16 s(*this);
	String8 other(_other);
	return s.equals(other);
}


sl_bool String8::equals(const sl_char8* utf8) const
{
	sl_char8* sz = getData();
	if (sz == utf8) {
		return sl_true;
	}
	return _equals8(sz, getLength(), utf8, -1);
}

sl_bool String16::equals(const sl_char8* utf8) const
{
	return _equals8(getData(), getLength(), utf8, -1);
}

sl_bool SafeString8::equals(const sl_char8* utf8) const
{
	String8 s(*this);
	sl_char8* sz = s.getData();
	if (sz == utf8) {
		return sl_true;
	}
	return String8::_equals8(sz, s.getLength(), utf8, -1);
}

sl_bool SafeString16::equals(const sl_char8* utf8) const
{
	String16 s(*this);
	return String16::_equals8(s.getData(), s.getLength(), utf8, -1);
}


sl_bool String8::equals(const sl_char16* utf16) const
{
	return _equals16(getData(), getLength(), utf16, -1);
}

sl_bool String16::equals(const sl_char16* utf16) const
{
	sl_char16* sz = getData();
	if (sz == utf16) {
		return sl_true;
	}
	return _equals16(sz, getLength(), utf16, -1);
}

sl_bool SafeString8::equals(const sl_char16* utf16) const
{
	String8 s(*this);
	return String8::_equals16(s.getData(), s.getLength(), utf16, -1);
}

sl_bool SafeString16::equals(const sl_char16* utf16) const
{
	String16 s(*this);
	sl_char16* sz = s.getData();
	if (sz == utf16) {
		return sl_true;
	}
	return String16::_equals16(sz, s.getLength(), utf16, -1);
}


sl_bool String8::equals(const sl_char32* utf32) const
{
	return _equals32(getData(), getLength(), utf32, -1);
}

sl_bool String16::equals(const sl_char32* utf32) const
{
	return _equals32(getData(), getLength(), utf32, -1);
}

sl_bool SafeString8::equals(const sl_char32* utf32) const
{
	String8 s(*this);
	return String8::_equals32(s.getData(), s.getLength(), utf32, -1);
}

sl_bool SafeString16::equals(const sl_char32* utf32) const
{
	String16 s(*this);
	return String16::_equals32(s.getData(), s.getLength(), utf32, -1);
}


sl_int32 String8::compare(const String8& other) const
{
	sl_char8* s1 = getData();
	sl_char8* s2 = other.getData();
	if (s1 == s2) {
		return 0;
	}
	return _compare8(s1, getLength(), s2, other.getLength());
}

sl_int32 String16::compare(const String16& other) const
{
	sl_char16* s1 = getData();
	sl_char16* s2 = other.getData();
	if (s1 == s2) {
		return 0;
	}
	return _compare16(s1, getLength(), s2, other.getLength());
}

sl_int32 SafeString8::compare(const String8& other) const
{
	String s(*this);
	return s.compare(other);
}

sl_int32 SafeString16::compare(const String16& other) const
{
	String16 s(*this);
	return s.compare(other);
}


sl_int32 String8::compare(const String16& _other) const
{
	String8 other(_other);
	return _compare8(getData(), getLength(), other.getData(), other.getLength());
}

sl_int32 String16::compare(const String8& _other) const
{
	String16 other(_other);
	return _compare16(getData(), getLength(), other.getData(), other.getLength());
}

sl_int32 SafeString8::compare(const String16& other) const
{
	String8 s(*this);
	return s.compare(other);
}

sl_int32 SafeString16::compare(const String8& other) const
{
	String16 s(*this);
	return s.compare(other);
}


sl_int32 String8::compare(const SafeString8& _other) const
{
	String8 other(_other);
	return compare(other);
}

sl_int32 String16::compare(const SafeString16& _other) const
{
	String16 other(_other);
	return compare(other);
}

sl_int32 SafeString8::compare(const SafeString8& _other) const
{
	String8 s(*this);
	String8 other(_other);
	return s.compare(other);
}

sl_int32 SafeString16::compare(const SafeString16& _other) const
{
	String16 s(*this);
	String16 other(_other);
	return s.compare(other);
}


sl_int32 String8::compare(const SafeString16& _other) const
{
	String16 other(_other);
	return compare(other);
}

sl_int32 String16::compare(const SafeString8& _other) const
{
	String8 other(_other);
	return compare(other);
}

sl_int32 SafeString8::compare(const SafeString16& _other) const
{
	String8 s(*this);
	String16 other(_other);
	return s.compare(other);
}

sl_int32 SafeString16::compare(const SafeString8& _other) const
{
	String16 s(*this);
	String8 other(_other);
	return s.compare(other);
}


sl_int32 String8::compare(const sl_char8* utf8) const
{
	sl_char8* sz = getData();
	if (sz == utf8) {
		return sl_true;
	}
	return _compare8(sz, getLength(), utf8, -1);
}

sl_int32 String16::compare(const sl_char8* utf8) const
{
	return _compare8(getData(), getLength(), utf8, -1);
}

sl_int32 SafeString8::compare(const sl_char8* utf8) const
{
	String8 s(*this);
	return s.compare(utf8);
}

sl_int32 SafeString16::compare(const sl_char8* utf8) const
{
	String16 s(*this);
	return s.compare(utf8);
}


sl_int32 String8::compare(const sl_char16* utf16) const
{
	return _compare16(getData(), getLength(), utf16, -1);
}

sl_int32 String16::compare(const sl_char16* utf16) const
{
	sl_char16* sz = getData();
	if (sz == utf16) {
		return sl_true;
	}
	return _compare16(sz, getLength(), utf16, -1);
}

sl_int32 SafeString8::compare(const sl_char16* utf16) const
{
	String8 s(*this);
	return s.compare(utf16);
}

sl_int32 SafeString16::compare(const sl_char16* utf16) const
{
	String16 s(*this);
	return s.compare(utf16);
}


sl_int32 String8::compare(const sl_char32* utf32) const
{
	return _compare32(getData(), getLength(), utf32, -1);
}

sl_int32 String16::compare(const sl_char32* utf32) const
{
	return _compare32(getData(), getLength(), utf32, -1);
}

sl_int32 SafeString8::compare(const sl_char32* utf32) const
{
	String8 s(*this);
	return s.compare(utf32);
}

sl_int32 SafeString16::compare(const sl_char32* utf32) const
{
	String16 s(*this);
	return s.compare(utf32);
}


sl_int32 String8::compare(const String8& other, sl_size len) const
{
	sl_char8* s1 = getData();
	sl_char8* s2 = other.getData();
	if (s1 == s2) {
		return 0;
	}
	sl_size l1 = getLength();
	if (l1 > len) {
		l1 = len;
	}
	sl_size l2 = other.getLength();
	if (l2 > len) {
		l2 = len;
	}
	return _compare8(s1, l1, s2, l2);
}

sl_int32 String16::compare(const String16& other, sl_size len) const
{
	sl_char16* s1 = getData();
	sl_char16* s2 = other.getData();
	if (s1 == s2) {
		return 0;
	}
	sl_size l1 = getLength();
	if (l1 > len) {
		l1 = len;
	}
	sl_size l2 = other.getLength();
	if (l2 > len) {
		l2 = len;
	}
	return _compare16(s1, l1, s2, l2);
}

sl_int32 SafeString8::compare(const String8& other, sl_size len) const
{
	String8 s(*this);
	return s.compare(other, len);
}

sl_int32 SafeString16::compare(const String16& other, sl_size len) const
{
	String16 s(*this);
	return s.compare(other, len);
}


#define _DEFINE_STRING_COMPARE_FUNCS(CLASS, OP, BODY, BODY_FRIEND) \
sl_bool CLASS::OP(const String8& other) const \
{ \
	return BODY; \
} \
sl_bool CLASS::OP(const String16& other) const \
{ \
	return BODY; \
} \
sl_bool CLASS::OP(const SafeString8& other) const \
{ \
	return BODY; \
} \
sl_bool CLASS::OP(const SafeString16& other) const \
{ \
	return BODY; \
} \
sl_bool CLASS::OP(const sl_char8* other) const \
{ \
	return BODY; \
} \
sl_bool CLASS::OP(const sl_char16* other) const \
{ \
	return BODY; \
} \
sl_bool CLASS::OP(const sl_char32* other) const \
{ \
	return BODY; \
} \
sl_bool OP(const sl_char8* other, const CLASS& s) \
{ \
	return BODY_FRIEND; \
} \
sl_bool OP(const sl_char16* other, const CLASS& s) \
{ \
	return BODY_FRIEND; \
} \
sl_bool OP(const sl_char32* other, const CLASS& s) \
{ \
	return BODY_FRIEND; \
}


_DEFINE_STRING_COMPARE_FUNCS(String8, operator==, equals(other), s.equals(other))
_DEFINE_STRING_COMPARE_FUNCS(String8, operator!=, !(equals(other)), !(s.equals(other)))
_DEFINE_STRING_COMPARE_FUNCS(String8, operator>=, compare(other)>=0, s.compare(other)<=0)
_DEFINE_STRING_COMPARE_FUNCS(String8, operator<=, compare(other)<=0, s.compare(other)>=0)
_DEFINE_STRING_COMPARE_FUNCS(String8, operator>, compare(other)>0, s.compare(other)<0)
_DEFINE_STRING_COMPARE_FUNCS(String8, operator<, compare(other)<0, s.compare(other)>0)

_DEFINE_STRING_COMPARE_FUNCS(String16, operator==, equals(other), s.equals(other))
_DEFINE_STRING_COMPARE_FUNCS(String16, operator!=, !(equals(other)), !(s.equals(other)))
_DEFINE_STRING_COMPARE_FUNCS(String16, operator>=, compare(other)>=0, s.compare(other)<=0)
_DEFINE_STRING_COMPARE_FUNCS(String16, operator<=, compare(other)<=0, s.compare(other)>=0)
_DEFINE_STRING_COMPARE_FUNCS(String16, operator>, compare(other)>0, s.compare(other)<0)
_DEFINE_STRING_COMPARE_FUNCS(String16, operator<, compare(other)<0, s.compare(other)>0)

_DEFINE_STRING_COMPARE_FUNCS(SafeString8, operator==, equals(other), s.equals(other))
_DEFINE_STRING_COMPARE_FUNCS(SafeString8, operator!=, !(equals(other)), !(s.equals(other)))
_DEFINE_STRING_COMPARE_FUNCS(SafeString8, operator>=, compare(other)>=0, s.compare(other)<=0)
_DEFINE_STRING_COMPARE_FUNCS(SafeString8, operator<=, compare(other)<=0, s.compare(other)>=0)
_DEFINE_STRING_COMPARE_FUNCS(SafeString8, operator>, compare(other)>0, s.compare(other)<0)
_DEFINE_STRING_COMPARE_FUNCS(SafeString8, operator<, compare(other)<0, s.compare(other)>0)

_DEFINE_STRING_COMPARE_FUNCS(SafeString16, operator==, equals(other), s.equals(other))
_DEFINE_STRING_COMPARE_FUNCS(SafeString16, operator!=, !(equals(other)), !(s.equals(other)))
_DEFINE_STRING_COMPARE_FUNCS(SafeString16, operator>=, compare(other)>=0, s.compare(other)<=0)
_DEFINE_STRING_COMPARE_FUNCS(SafeString16, operator<=, compare(other)<=0, s.compare(other)>=0)
_DEFINE_STRING_COMPARE_FUNCS(SafeString16, operator>, compare(other)>0, s.compare(other)<0)
_DEFINE_STRING_COMPARE_FUNCS(SafeString16, operator<, compare(other)<0, s.compare(other)>0)

String8 String8::duplicate() const
{
    return String8(getData(), getLength());
}

String16 String16::duplicate() const
{
    return String16(getData(), getLength());
}

String8 SafeString8::duplicate() const
{
    String8 s(*this);
    return s.duplicate();
}

String16 SafeString16::duplicate() const
{
    String16 s(*this);
    return s.duplicate();
}

Memory String8::toMemory() const
{
	return Memory::create(getData(), getLength()*sizeof(sl_char8));
}

Memory String16::toMemory() const
{
	return Memory::create(getData(), getLength()*sizeof(sl_char16));
}

Memory SafeString8::toMemory() const
{
    String8 s(*this);
    return s.toMemory();
}

Memory SafeString16::toMemory() const
{
	String16 s(*this);
	return s.toMemory();
}

sl_size String8::getUtf16(sl_char16* utf16, sl_size len) const
{
	return Charsets::utf8ToUtf16(getData(), getLength(), utf16, len);
}

sl_size String16::getUtf8(sl_char8* utf8, sl_size len) const
{
	return Charsets::utf16ToUtf8(getData(), getLength(), utf8, len);
}

sl_bool String8::getUtf16(StringData& output) const
{
	if (isEmpty()) {
		output.sz16 = (sl_char16*)("\0\0");
		output.len = 0;
		return sl_true;
	}
	Memory mem = toUtf16();
	output.refer = mem.ref;
	if (mem.isNotEmpty()) {
		output.sz16 = (const sl_char16*)(mem.getData());
		output.len = mem.getSize() / 2 - 1;
		if (output.sz16) {
			return sl_true;
		}
	}
	output.sz16 = (sl_char16*)("\0\0");
	output.len = 0;
	return sl_false;
}

sl_bool String16::getUtf8(StringData& output) const
{
	if (isEmpty()) {
		output.sz8 = "";
		output.len = 0;
		return sl_true;
	}
	Memory mem = toUtf8();
	if (mem.isNotEmpty()) {
		output.sz8 = (const sl_char8*)(mem.getData());
		output.len = mem.getSize() - 1;
		if (output.sz8) {
			return sl_true;
		}
	}
	output.sz8 = "";
	output.len = 0;
	return sl_false;
}

sl_size SafeString8::getUtf16(sl_char16* utf16, sl_size len) const
{
    String8 s(*this);
    return s.getUtf16(utf16, len);
}

sl_bool SafeString8::getUtf16(StringData& output) const
{
    String8 s(*this);
    return s.getUtf16(output);
}

sl_size SafeString16::getUtf8(sl_char8* utf8, sl_size len) const
{
	String16 s(*this);
	return s.getUtf8(utf8, len);
}

sl_bool SafeString16::getUtf8(StringData& output) const
{
	String16 s(*this);
	return s.getUtf8(output);
}

Memory String8::toUtf16() const
{
	sl_char8* sz8 = getData();
	sl_size len8 = getLength();
	sl_size len16 = Charsets::utf8ToUtf16(sz8, len8, sl_null, -1);
	Memory memory = Memory::create(2 * (len16 + 1));
	if (memory.isNotEmpty()) {
		sl_char16* buf = (sl_char16*)(memory.getData());
		Charsets::utf8ToUtf16(sz8, len8, buf, len16);
		buf[len16] = 0;
	}
	return memory;
}

Memory String16::toUtf8() const
{
	sl_char16* sz16 = getData();
	sl_size len16 = getLength();
	sl_size len8 = Charsets::utf16ToUtf8(sz16, len16, sl_null, -1);
	Memory memory = Memory::create(len8 + 1);
	if (memory.isNotEmpty()) {
		sl_char8* buf = (sl_char8*)(memory.getData());
		Charsets::utf16ToUtf8(sz16, len16, buf, len8);
		buf[len8] = 0;
	}
	return memory;
}

Memory SafeString8::toUtf16() const
{
    String8 s(*this);
    return s.toUtf16();
}

Memory SafeString16::toUtf8() const
{
	String16 s(*this);
	return s.toUtf8();
}

sl_size String8::getUtf32(sl_char32* utf32, sl_size len) const
{
	return Charsets::utf8ToUtf32(getData(), getLength(), utf32, len);
}

sl_bool String8::getUtf32(StringData& output) const
{
	if (isEmpty()) {
		output.sz32 = (sl_char32*)("\0\0\0\0");
		output.len = 0;
		return sl_true;
	}
	Memory mem = toUtf32();
	if (mem.isNotEmpty()) {
		output.sz32 = (const sl_char32*)(mem.getData());
		output.len = mem.getSize() / 4 - 1;
		if (output.sz32) {
			return sl_true;
		}
	}
	output.sz32 = (sl_char32*)("\0\0\0\0");
	output.len = 0;
	return sl_false;
}

sl_size SafeString8::getUtf32(sl_char32* utf32, sl_size len) const
{
    String8 s(*this);
    return s.getUtf32(utf32, len);
}

sl_bool SafeString8::getUtf32(StringData& output) const
{
    String8 s(*this);
    return s.getUtf32(output);
}

Memory String8::toUtf32() const
{
	sl_char8* sz8 = getData();
	sl_size len8 = getLength();
	sl_size len32 = Charsets::utf8ToUtf32(sz8, len8, sl_null, -1);
	Memory memory = Memory::create(4 * (len32 + 1));
	if (memory.isNotEmpty()) {
		sl_char32* buf = (sl_char32*)(memory.getData());
		Charsets::utf8ToUtf32(sz8, len8, buf, len32);
		buf[len32] = 0;
	}
	return memory;
}

Memory SafeString8::toUtf32() const
{
    String8 s(*this);
    return s.toUtf32();
}

String8 String8::substring(sl_reg start, sl_reg end) const
{
	sl_reg count = getLength();
	if (start < 0) {
		start = 0;
	}
	if (end < 0 || end > count) {
		end = count;
	}
	if (start >= end) {
		return String8::null();
	}
	if (start == 0 && end == count) {
		return *this;
	}
	return String8(getData() + start, end - start);
}

String16 String16::substring(sl_reg start, sl_reg end) const
{
	sl_reg count = getLength();
	if (start < 0) {
		start = 0;
	}
	if (end < 0 || end > count) {
		end = count;
	}
	if (start >= end) {
		return String16::null();
	}
	if (start == 0 && end == count) {
		return *this;
	}
	return String16(getData() + start, end - start);
}

String8 SafeString8::substring(sl_reg start, sl_reg end) const
{
    String8 s(*this);
    return s.substring(start, end);
}

String16 SafeString16::substring(sl_reg start, sl_reg end) const
{
	String16 s(*this);
	return s.substring(start, end);
}

String8 String8::left(sl_reg len) const
{
    return substring(0, len);
}

String16 String16::left(sl_reg len) const
{
    return substring( 0, len );
}

String8 SafeString8::left(sl_reg len) const
{
    String8 s(*this);
    return s.left(len);
}

String16 SafeString16::left(sl_reg len) const
{
	String16 s(*this);
	return s.left(len);
}

String8 String8::right(sl_reg len) const
{
    return substring(getLength() - len);
}

String16 String16::right(sl_reg len) const
{
    return substring(getLength() - len);
}

String8 SafeString8::right(sl_reg len) const
{
	String8 s(*this);
	return s.right(len);
}

String16 SafeString16::right(sl_reg len) const
{
	String16 s(*this);
	return s.right(len);
}

String8 String8::mid(sl_reg start, sl_reg len) const
{
    return substring(start, start + len);
}
	
String16 String16::mid(sl_reg start, sl_reg len) const
{
    return substring(start, start+len);
}

String8 SafeString8::mid(sl_reg start, sl_reg len) const
{
	String8 s(*this);
	return s.mid(start, len);
}

String16 SafeString16::mid(sl_reg start, sl_reg len) const
{
	String16 s(*this);
	return s.mid(start, len);
}

sl_reg String8::indexOf(sl_char8 ch, sl_reg _start) const
{
	sl_size count = getLength();
	if (count == 0) {
		return -1;
	}
	sl_size start;
	if (_start < 0) {
		start = 0;
	} else {
		start = _start;
		if (start >= count) {
			return -1;
		}
	}
	sl_char8* sz = getData();
	sl_char8* pt = (sl_char8*)(Base::findMemory(sz + start, ch, count - start));
	if (pt == sl_null) {
		return -1;
	} else {
		return (sl_reg)(pt - sz);
	}
}

sl_reg String16::indexOf(sl_char16 ch, sl_reg _start) const
{
	sl_size count = getLength();
	if (count == 0) {
		return -1;
	}
	sl_size start;
	if (_start < 0) {
		start = 0;
	} else {
		start = _start;
		if (start >= count) {
			return -1;
		}
	}
	sl_char16* sz = getData();
	sl_char16* pt = (sl_char16*)(Base::findMemory2((sl_uint16*)(sz + start), ch, count - start));
	if (pt == sl_null) {
		return -1;
	} else {
		return (sl_reg)(pt - sz);
	}
}

sl_reg SafeString8::indexOf(sl_char8 ch, sl_reg start) const
{
	String8 s(*this);
	return s.indexOf(ch, start);
}

sl_reg SafeString16::indexOf(sl_char16 ch, sl_reg start) const
{
	String16 s(*this);
	return s.indexOf(ch, start);
}

template <class ST, class CT, class TT>
SLIB_INLINE sl_reg _String_indexOf(const ST& str, const CT* bufPat, sl_size countPat, sl_reg _start)
{
	if (countPat == 0) {
		return 0;
	}
	if (countPat == 1) {
		return str.indexOf(bufPat[0], _start);
	}
	sl_size count = str.getLength();
	if (count < countPat) {
		return -1;
	}
	const CT* buf = str.getData();
	sl_size start;
	if (_start < 0) {
		start = 0;
	} else {
		start = _start;
		if (start > count - countPat) {
			return -1;
		}
	}
	while (start <= count - countPat) {
		const CT* pt = (const CT*)(TT::findMemory(buf + start, bufPat[0], count - start - countPat + 1));
		if (pt == sl_null) {
			return -1;
		}
		if (TT::compareMemory(pt + 1, bufPat + 1, countPat - 1) == 0) {
			return (sl_reg)(pt - buf);
		} else {
			start = (sl_size)(pt - buf + 1);
		}
	}
	return -1;
}

sl_reg String8::indexOf(const String8& pattern, sl_reg start) const
{
	return _String_indexOf<String8, sl_char8, _TemplateFunc8>(*this, pattern.getData(), pattern.getLength(), start);
}

sl_reg String16::indexOf(const String16& pattern, sl_reg start) const
{
	return _String_indexOf<String16, sl_char16, _TemplateFunc16>(*this, pattern.getData(), pattern.getLength(), start);
}

sl_reg SafeString8::indexOf(const String8& str, sl_reg start) const
{
	String8 s(*this);
	return s.indexOf(str, start);
}

sl_reg SafeString16::indexOf(const String16& str, sl_reg start) const
{
	String16 s(*this);
	return s.indexOf(str, start);
}

sl_reg String8::indexOf(const sl_char8* pattern, sl_reg start) const
{
	return _String_indexOf<String8, sl_char8, _TemplateFunc8>(*this, pattern, Base::getStringLength(pattern), start);
}

sl_reg String16::indexOf(const sl_char16* pattern, sl_reg start) const
{
	return _String_indexOf<String16, sl_char16, _TemplateFunc16>(*this, pattern, Base::getStringLength2(pattern), start);
}

sl_reg SafeString8::indexOf(const sl_char8* pattern, sl_reg start) const
{
	String8 s(*this);
	return s.indexOf(pattern, start);
}

sl_reg SafeString16::indexOf(const sl_char16* pattern, sl_reg start) const
{
	String16 s(*this);
	return s.indexOf(pattern, start);
}

sl_reg String8::lastIndexOf(sl_char8 ch, sl_reg _start) const
{
	sl_size count = getLength();
	if (count == 0) {
		return -1;
	}
	sl_size start;
	if (_start < 0) {
		start = count - 1;
	} else {
		start = _start;
		if (start >= count) {
			start = count - 1;
		}
	}
	sl_char8* sz = getData();
	sl_char8* pt = (sl_char8*)(Base::findMemoryReverse(sz, ch, start + 1));
	if (pt == sl_null) {
		return -1;
	} else {
		return (sl_reg)(pt - sz);
	}
}

sl_reg String16::lastIndexOf(sl_char16 ch, sl_reg _start) const
{
	sl_size count = getLength();
	if (count == 0) {
		return -1;
	}
	sl_size start;
	if (_start < 0) {
		start = count - 1;
	} else {
		start = _start;
		if (start >= count) {
			start = count - 1;
		}
	}
	sl_char16* sz = getData();
	sl_char16* pt = (sl_char16*)(Base::findMemoryReverse2((sl_uint16*)sz, ch, start + 1));
	if (pt == sl_null) {
		return -1;
	} else {
		return (sl_reg)(pt - sz);
	}
}

sl_reg SafeString8::lastIndexOf(sl_char8 ch, sl_reg start) const
{
	String8 s(*this);
	return s.lastIndexOf(ch, start);
}

sl_reg SafeString16::lastIndexOf(sl_char16 ch, sl_reg start) const
{
	String16 s(*this);
	return s.lastIndexOf(ch, start);
}

template <class ST, class CT, class TT>
SLIB_INLINE sl_reg _String_lastIndexOf(const ST& str, const CT* bufPat, sl_size countPat, sl_reg _start)
{
	if (countPat == 0) {
		return 0;
	}
	if (countPat == 1) {
		return str.lastIndexOf(bufPat[0], _start);
	}
	sl_size count = str.getLength();
	if (count < countPat) {
		return -1;
	}
	const CT* buf = str.getData();
	sl_size s;
	if (_start < 0) {
		s = count - countPat + 1;
	} else {
		s = _start;
		sl_size n = count - countPat + 1;
		if (s > n) {
			s = n;
		}
	}
	while (s > 0) {
		const CT* pt = (const CT*)(TT::findMemoryReverse(buf, bufPat[0], s));
		if (pt == sl_null) {
			return -1;
		}
		if (TT::compareMemory(pt + 1, bufPat + 1, countPat - 1) == 0) {
			return (sl_reg)(pt - buf);
		} else {
			s = (sl_size)(pt - buf);
		}
	}
	return -1;
}

sl_reg String8::lastIndexOf(const String8& pattern, sl_reg start) const
{
	return _String_lastIndexOf<String8, sl_char8, _TemplateFunc8>(*this, pattern.getData(), pattern.getLength(), start);
}

sl_reg String16::lastIndexOf(const String16& pattern, sl_reg start) const
{
	return _String_lastIndexOf<String16, sl_char16, _TemplateFunc16>(*this, pattern.getData(), pattern.getLength(), start);
}

sl_reg SafeString8::lastIndexOf(const String8& str, sl_reg start) const
{
	String8 s(*this);
	return s.indexOf(str, start);
}

sl_reg SafeString16::lastIndexOf(const String16& str, sl_reg start) const
{
	String16 s(*this);
	return s.indexOf(str, start);
}

sl_reg String8::lastIndexOf(const sl_char8* pattern, sl_reg start) const
{
	return _String_lastIndexOf<String8, sl_char8, _TemplateFunc8>(*this, pattern, Base::getStringLength(pattern), start);
}

sl_reg String16::lastIndexOf(const sl_char16* pattern, sl_reg start) const
{
	return _String_lastIndexOf<String16, sl_char16, _TemplateFunc16>(*this, pattern, Base::getStringLength2(pattern), start);
}

sl_reg SafeString8::lastIndexOf(const sl_char8* pattern, sl_reg start) const
{
	String8 s(*this);
	return s.indexOf(pattern, start);
}

sl_reg SafeString16::lastIndexOf(const sl_char16* pattern, sl_reg start) const
{
	String16 s(*this);
	return s.indexOf(pattern, start);
}

sl_bool String8::startsWith(sl_char8 ch) const
{
	if (isEmpty()) {
		return sl_false;
	} else {
		return m_container->sz[0] == ch;
	}
}

sl_bool String16::startsWith(sl_char16 ch) const
{
	if (isEmpty()) {
		return sl_false;
	} else {
		return m_container->sz[0] == ch;
	}
}

sl_bool SafeString8::startsWith(sl_char8 ch) const
{
	String8 s(*this);
	return s.startsWith(ch);
}

sl_bool SafeString16::startsWith(sl_char16 ch) const
{
	String16 s(*this);
	return s.startsWith(ch);
}


sl_bool String8::startsWith(const String8& str) const
{
	sl_size count2 = str.getLength();
	if (count2 == 0) {
		return sl_true;
	}
	sl_size count1 = getLength();
	if (count1 < count2) {
		return sl_false;
	} else {
		return Base::equalsMemory(getData(), str.getData(), count2);
	}
}

sl_bool String16::startsWith(const String16& str) const
{
	sl_size count2 = str.getLength();
	if (count2 == 0) {
		return sl_true;
	}
	sl_size count1 = getLength();
	if (count1 < count2) {
		return sl_false;
	} else {
		return Base::equalsMemory2((sl_uint16*)(getData()), (sl_uint16*)(str.getData()), count2);
	}
}

sl_bool SafeString8::startsWith(const String8& str) const
{
	String8 s(*this);
	return s.startsWith(str);
}

sl_bool SafeString16::startsWith(const String16& str) const
{
	String16 s(*this);
	return s.startsWith(str);
}

sl_bool String8::startsWith(const sl_char8* str) const
{
	const sl_char8* s = getData();
	sl_size count = getLength();
	for (sl_size i = 0; i < count; i++) {
		if (str[i] == 0) {
			return sl_true;
		}
		if (s[i] != str[i]) {
			return sl_false;
		}
	}
	if (str[count] == 0) {
		return sl_true;
	}
	return sl_false;

}

sl_bool String16::startsWith(const sl_char16* str) const
{
	const sl_char16* s = getData();
	sl_size count = getLength();
	for (sl_size i = 0; i < count; i++) {
		if (str[i] == 0) {
			return sl_true;
		}
		if (s[i] != str[i]) {
			return sl_false;
		}
	}
	if (str[count] == 0) {
		return sl_true;
	}
	return sl_false;
}

sl_bool SafeString8::startsWith(const sl_char8* str) const
{
	String8 s(*this);
	return s.startsWith(str);
}

sl_bool SafeString16::startsWith(const sl_char16* str) const
{
	String16 s(*this);
	return s.startsWith(str);
}

sl_bool String8::endsWith(sl_char8 ch) const
{
	sl_size count = getLength();
	if (count < 1) {
		return sl_false;
	} else {
		return m_container->sz[count - 1] == ch;
	}
}

sl_bool String16::endsWith(sl_char16 ch) const
{
	sl_size count = getLength();
	if (count < 1) {
		return sl_false;
	} else {
		return m_container->sz[count - 1] == ch;
	}
}

sl_bool SafeString8::endsWith(sl_char8 ch) const
{
	String8 s(*this);
	return s.endsWith(ch);
}

sl_bool SafeString16::endsWith(sl_char16 ch) const
{
	String16 s(*this);
	return s.endsWith(ch);
}


sl_bool String8::endsWith(const String8& str) const
{
	sl_size count2 = str.getLength();
	if (count2 == 0) {
		return sl_true;
	}
	sl_size count1 = getLength();
	if (count1 < count2) {
		return sl_false;
	} else {
		return Base::equalsMemory(getData() + count1 - count2, str.getData(), count2);
	}
}

sl_bool String16::endsWith(const String16& str) const
{
	sl_size count2 = str.getLength();
	if (count2 == 0) {
		return sl_true;
	}
	sl_size count1 = getLength();
	if (count1 < count2) {
		return sl_false;
	} else {
		return Base::equalsMemory2((sl_uint16*)(getData() + count1 - count2), (sl_uint16*)(str.getData()), count2);
	}
}

sl_bool SafeString8::endsWith(const String8& str) const
{
	String8 s(*this);
	return s.endsWith(str);
}

sl_bool SafeString16::endsWith(const String16& str) const
{
	String16 s(*this);
	return s.endsWith(str);
}

sl_bool String8::endsWith(const sl_char8* str) const
{
	sl_size count2 = Base::getStringLength(str);
	if (count2 == 0) {
		return sl_true;
	}
	sl_size count1 = getLength();
	if (count1 < count2) {
		return sl_false;
	} else {
		return Base::equalsMemory(getData() + count1 - count2, str, count2);
	}
}

sl_bool String16::endsWith(const sl_char16* str) const
{
	sl_size count2 = Base::getStringLength2(str);
	if (count2 == 0) {
		return sl_true;
	}
	sl_size count1 = getLength();
	if (count1 < count2) {
		return sl_false;
	} else {
		return Base::equalsMemory2((sl_uint16*)(getData() + count1 - count2), (sl_uint16*)str, count2);
	}
}

sl_bool SafeString8::endsWith(const sl_char8* str) const
{
	String8 s(*this);
	return s.endsWith(str);
}

sl_bool SafeString16::endsWith(const sl_char16* str) const
{
	String16 s(*this);
	return s.endsWith(str);
}

sl_bool String8::contains(sl_char8 ch) const
{
    return indexOf(ch) >= 0;
}

sl_bool String16::contains(sl_char16 ch) const
{
    return indexOf(ch) >= 0;
}

sl_bool SafeString8::constains(sl_char8 ch) const
{
	String8 s(*this);
	return s.contains(ch);
}

sl_bool SafeString16::contains(sl_char16 ch) const
{
	String16 s(*this);
	return s.contains(ch);
}

sl_bool String8::contains(const String8& str) const
{
    return indexOf(str) >= 0;
}

sl_bool String16::contains(const String16& str) const
{
    return indexOf(str) >= 0;
}

sl_bool SafeString8::contains(const String8& str) const
{
	String8 s(*this);
	return s.contains(str);
}

sl_bool SafeString16::contains(const String16& str) const
{
	String16 s(*this);
	return s.contains(str);
}

sl_bool String8::contains(const sl_char8* str) const
{
	return indexOf(str) >= 0;
}

sl_bool String16::contains(const sl_char16* str) const
{
	return indexOf(str) >= 0;
}

sl_bool SafeString8::contains(const sl_char8* str) const
{
	String8 s(*this);
	return s.contains(str);
}

sl_bool SafeString16::contains(const sl_char16* str) const
{
	String16 s(*this);
	return s.contains(str);
}

template <class CT>
SLIB_INLINE void _String_copyMakingUpper(CT* dst, const CT* src, sl_size len)
{
	for (sl_size i = 0; i < len; i++) {
		dst[i] = SLIB_CHAR_LOWER_TO_UPPER(src[i]);
	}
}

template <class CT>
SLIB_INLINE void _String_copyMakingLower(CT* dst, const CT* src, sl_size len)
{
	for (sl_size i = 0; i < len; i++) {
		dst[i] = SLIB_CHAR_UPPER_TO_LOWER(src[i]);
	}
}

void String8::makeUpper()
{
	_String_copyMakingUpper(getData(), getData(), getLength());
}

void String16::makeUpper()
{
	_String_copyMakingUpper(getData(), getData(), getLength());
}

void SafeString8::makeUpper()
{
	String8 s(*this);
	_String_copyMakingUpper(s.getData(), s.getData(), s.getLength());
}

void SafeString16::makeUpper()
{
	String16 s(*this);
	_String_copyMakingUpper(s.getData(), s.getData(), s.getLength());
}

void String8::makeLower()
{
	_String_copyMakingLower(getData(), getData(), getLength());
}

void String16::makeLower()
{
	_String_copyMakingLower(getData(), getData(), getLength());
}

void SafeString8::makeLower()
{
	String8 s(*this);
	_String_copyMakingLower(s.getData(), s.getData(), s.getLength());
}

void SafeString16::makeLower()
{
	String16 s(*this);
	_String_copyMakingLower(s.getData(), s.getData(), s.getLength());
}

String8 String8::toUpper(const sl_char8* sz, sl_reg _len)
{
	if (_len == 0) {
		return String8::getEmpty();
	}
	sl_size len;
	if (_len < 0) {
		len = Base::getStringLength(sz);
	} else {
		len = _len;
	}
	String8 ret = String8::allocate(len);
	if (ret.isNull()) {
		return ret;
	}
	_String_copyMakingUpper(ret.getData(), sz, len);
	return ret;
}

String16 String16::toUpper(const sl_char16* sz, sl_reg _len)
{
	if (_len == 0) {
		return String16::getEmpty();
	}
	sl_size len;
	if (_len < 0) {
		len = Base::getStringLength2(sz);
	} else {
		len = _len;
	}
	String16 ret = String16::allocate(len);
	if (ret.isNull()) {
		return ret;
	}
	_String_copyMakingUpper(ret.getData(), sz, len);
	return ret;
}

String8 String8::toLower(const sl_char8* sz, sl_reg _len)
{
	if (_len == 0) {
		return String8::getEmpty();
	}
	sl_size len;
	if (_len < 0) {
		len = Base::getStringLength(sz);
	} else {
		len = _len;
	}
	String8 ret = String8::allocate(len);
	if (ret.isNull()) {
		return ret;
	}
	_String_copyMakingLower(ret.getData(), sz, len);
	return ret;
}

String16 String16::toLower(const sl_char16* sz, sl_reg _len)
{
	if (_len == 0) {
		return String16::getEmpty();
	}
	sl_size len;
	if (_len < 0) {
		len = Base::getStringLength2(sz);
	} else {
		len = _len;
	}
	String16 ret = String16::allocate(len);
	if (ret.isNull()) {
		return ret;
	}
	_String_copyMakingLower(ret.getData(), sz, len);
	return ret;
}

String8 String8::toUpper() const
{
	if (isNull()) {
		return String8::null();
	}
	return toUpper(getData(), getLength());
}

String16 String16::toUpper() const
{
	if (isNull()) {
		return String16::null();
	}
	return toUpper(getData(), getLength());
}

String8 SafeString8::toUpper() const
{
	if (isNull()) {
		return String8::null();
	}
	String8 s(*this);
	return String8::toUpper(s.getData(), s.getLength());
}

String16 SafeString16::toUpper() const
{
	if (isNull()) {
		return String16::null();
	}
	String16 s(*this);
	return String16::toUpper(s.getData(), s.getLength());
}

String8 String8::toLower() const
{
	if (isNull()) {
		return String8::null();
	}
	return toLower(getData(), getLength());
}

String16 String16::toLower() const
{
	if (isNull()) {
		return String16::null();
	}
	return toLower(getData(), getLength());
}

String8 SafeString8::toLower() const
{
	if (isNull()) {
		return String8::null();
	}
	String8 s(*this);
	return String8::toLower(s.getData(), s.getLength());
}

String16 SafeString16::toLower() const
{
	if (isNull()) {
		return String16::null();
	}
	String16 s(*this);
	return String16::toLower(s.getData(), s.getLength());
}


struct STRING_REPLACE_SUBSET
{
	sl_reg start;
	sl_reg len;
};

template <class ST, class CT>
SLIB_INLINE ST _String_replaceAll(const ST& str, const CT* pattern, sl_reg countPat, const CT* bufReplace, sl_reg countReplace)
{
	if (countPat == 0) {
		return ST::null();
	}
	sl_reg count = str.getLength();
	if (count == 0) {
		return ST::null();
	}
	Queue<STRING_REPLACE_SUBSET> queue;
	STRING_REPLACE_SUBSET subset;
	const CT* buf = str.getData();
	sl_reg size = 0;
	sl_reg start = 0;
	while (start <= count + countPat - 1) {
		sl_reg index = str.indexOf(pattern, start);
		if (index < 0) {
			index = count;
		} else {
			size += countReplace;
		}
		subset.start = start;
		subset.len = index - start;
		queue.push_NoLock(subset);
		size += subset.len;
		start = index + countPat;
	}

	ST ret = ST::allocate(size);
	if (ret.isNotNull()) {
		CT* out = ret.getData();
		while (queue.pop_NoLock(&subset)) {
			Base::copyMemory(out, buf + subset.start, subset.len*sizeof(CT));
			out += subset.len;
			if (queue.isNotEmpty()) {
				Base::copyMemory(out, bufReplace, countReplace*sizeof(CT));
				out += countReplace;
			}
		}
	}
	return ret;
}

String8 String8::replaceAll(const String8& pattern, const String8& replacement) const
{
	return _String_replaceAll<String8, sl_char8>(*this, pattern.getData(), pattern.getLength(), replacement.getData(), replacement.getLength());
}

String16 String16::replaceAll(const String16& pattern, const String16& replacement) const
{
	return _String_replaceAll<String16, sl_char16>(*this, pattern.getData(), pattern.getLength(), replacement.getData(), replacement.getLength());
}

String8 SafeString8::replaceAll(const String8& pattern, const String8& replacement) const
{
	String8 s(*this);
	return s.replaceAll(pattern, replacement);
}

String16 SafeString16::replaceAll(const String16& pattern, const String16& replacement) const
{
	String16 s(*this);
	return s.replaceAll(pattern, replacement);
}

String8 String8::replaceAll(const String8& pattern, const sl_char8* replacement) const
{
	return _String_replaceAll<String8, sl_char8>(*this, pattern.getData(), pattern.getLength(), replacement, Base::getStringLength(replacement));
}

String16 String16::replaceAll(const String16& pattern, const sl_char16* replacement) const
{
	return _String_replaceAll<String16, sl_char16>(*this, pattern.getData(), pattern.getLength(), replacement, Base::getStringLength2(replacement));
}

String8 SafeString8::replaceAll(const String8& pattern, const sl_char8* replacement) const
{
	String8 s(*this);
	return s.replaceAll(pattern, replacement);
}

String16 SafeString16::replaceAll(const String16& pattern, const sl_char16* replacement) const
{
	String16 s(*this);
	return s.replaceAll(pattern, replacement);
}

String8 String8::replaceAll(const sl_char8* pattern, const String8& replacement) const
{
	return _String_replaceAll<String8, sl_char8>(*this, pattern, Base::getStringLength(pattern), replacement.getData(), replacement.getLength());
}

String16 String16::replaceAll(const sl_char16* pattern, const String16& replacement) const
{
	return _String_replaceAll<String16, sl_char16>(*this, pattern, Base::getStringLength2(pattern), replacement.getData(), replacement.getLength());
}

String8 SafeString8::replaceAll(const sl_char8* pattern, const String8& replacement) const
{
	String8 s(*this);
	return s.replaceAll(pattern, replacement);
}

String16 SafeString16::replaceAll(const sl_char16* pattern, const String16& replacement) const
{
	String16 s(*this);
	return s.replaceAll(pattern, replacement);
}

String8 String8::replaceAll(const sl_char8* pattern, const sl_char8* replacement) const
{
	return _String_replaceAll<String8, sl_char8>(*this, pattern, Base::getStringLength(pattern), replacement, Base::getStringLength(replacement));
}

String16 String16::replaceAll(const sl_char16* pattern, const sl_char16* replacement) const
{
	return _String_replaceAll<String16, sl_char16>(*this, pattern, Base::getStringLength2(pattern), replacement, Base::getStringLength2(replacement));
}

String8 SafeString8::replaceAll(const sl_char8* pattern, const sl_char8* replacement) const
{
	String8 s(*this);
	return s.replaceAll(pattern, replacement);
}

String16 SafeString16::replaceAll(const sl_char16* pattern, const sl_char16* replacement) const
{
	String16 s(*this);
	return s.replaceAll(pattern, replacement);
}

template <class ST, class CT>
SLIB_INLINE ST _String_trim(const ST& str)
{
	const CT* sz = str.getData();
	sl_size n = str.getLength();
	sl_size i = 0;
	for (; i < n; i++) {
		CT c = sz[i];
		if (c != ' ' && c != '\t' && c != '\r' && c != '\n') {
			break;
		}
	}
	if (i >= n) {
		return ST::null();
	}
	sl_size j = n - 1;
	for (; j >= i; j--) {
		CT c = sz[j];
		if (c != ' ' && c != '\t' && c != '\r' && c != '\n') {
			break;
		}
	}
	return str.substring(i, j + 1);
}

String8 String8::trim() const
{
	return _String_trim<String8, sl_char8>(*this);
}

String16 String16::trim() const
{
	return _String_trim<String16, sl_char16>(*this);
}

String8 SafeString8::trim() const
{
	String8 s(*this);
	return s.trim();
}

String16 SafeString16::trim() const
{
	String16 s(*this);
	return s.trim();
}


template <class ST, class CT>
SLIB_INLINE ST _String_trimLeft(const ST& str)
{
	const CT* sz = str.getData();
	sl_size n = str.getLength();
	sl_size i = 0;
	for (; i < n; i++) {
		CT c = sz[i];
		if (c != ' ' && c != '\t' && c != '\r' && c != '\n') {
			break;
		}
	}
	if (i >= n) {
		return ST::null();
	}
	return str.substring(i);
}

String8 String8::trimLeft() const
{
	return _String_trimLeft<String8, sl_char8>(*this);
}

String16 String16::trimLeft() const
{
	return _String_trimLeft<String16, sl_char16>(*this);
}

String8 SafeString8::trimLeft() const
{
	String8 s(*this);
	return s.trimLeft();
}

String16 SafeString16::trimLeft() const
{
	String16 s(*this);
	return s.trimLeft();
}


template <class ST, class CT>
SLIB_INLINE ST _String_trimRight(const ST& str)
{
	const CT* sz = str.getData();
	sl_size n = str.getLength();
	sl_size j = n;
	for (; j > 0; j--) {
		CT c = sz[j-1];
		if (c != ' ' && c != '\t' && c != '\r' && c != '\n') {
			break;
		}
	}
	if (j == 0) {
		return ST::null();
	}
	return str.substring(0, j);
}

String8 String8::trimRight() const
{
	return _String_trimRight<String8, sl_char8>(*this);
}

String16 String16::trimRight() const
{
	return _String_trimRight<String16, sl_char16>(*this);
}

String8 SafeString8::trimRight() const
{
	String8 s(*this);
	return s.trimRight();
}

String16 SafeString16::trimRight() const
{
	String16 s(*this);
	return s.trimRight();
}


template <class ST, class CT, class TT>
List<ST> _String_split(const ST& str, const CT* pattern, sl_size countPattern)
{
	if (countPattern == 0) {
		return List<ST>::null();
	}
	CList<ST>* ret = CList<ST>::create();
	if (ret) {
		sl_reg start = 0;
		while (1) {
			sl_reg index = _String_indexOf<ST, CT, TT>(str, pattern, countPattern, start);
			if (index < 0) {
				ret->add_NoLock(str.substring(start));
				break;
			}
			ret->add_NoLock(str.substring(start, index));
			start = index + countPattern;
		}
	}
	return ret;
}

List<String8> String8::split(const String8& pattern) const
{
	return _String_split<String8, sl_char8, _TemplateFunc8>(*this, pattern.getData(), pattern.getLength());
}

List<String16> String16::split(const String16& pattern) const
{
	return _String_split<String16, sl_char16, _TemplateFunc16>(*this, pattern.getData(), pattern.getLength());
}

List<String8> SafeString8::split(const String8& pattern) const
{
	String8 s(*this);
	return s.split(pattern);
}

List<String16> SafeString16::split(const String16& pattern) const
{
	String16 s(*this);
	return s.split(pattern);
}

List<String8> String8::split(const sl_char8* pattern) const
{
	return _String_split<String8, sl_char8, _TemplateFunc8>(*this, pattern, Base::getStringLength(pattern));
}

List<String16> String16::split(const sl_char16* pattern) const
{
	return _String_split<String16, sl_char16, _TemplateFunc16>(*this, pattern, Base::getStringLength2(pattern));
}

List<String8> SafeString8::split(const sl_char8* pattern) const
{
	String8 s(*this);
	return s.split(pattern);
}

List<String16> SafeString16::split(const sl_char16* pattern) const
{
	String16 s(*this);
	return s.split(pattern);
}


template <class ST, class CT>
static sl_size _String_applyBackslashEscapes(const ST& s, sl_bool flagDoubleQuote, sl_bool flagAddQuote, sl_bool flagEscapeNonAscii, CT* buf)
{
	const CT* ch = s.getData();
	sl_size len = s.getLength();
	sl_size d;
	if (flagAddQuote) {
		d = 1;
		if (buf) {
			buf[0] = flagDoubleQuote ? '"' : '\'';
		}
	} else {
		d = 0;
	}
	for (sl_size i = 0; i < len; i++) {
		CT c = ch[i];
		CT r = 0;
		switch (c) {
			case '\\':
				r = c;
				break;
			case '"':
				if (flagDoubleQuote) {
					r = c;
				}
				break;
			case '\'':
				if (! flagDoubleQuote) {
					r = c;
				}
				break;
			case 0:
				r = '0';
				break;
			case '\n':
				r = 'n';
				break;
			case '\r':
				r = 'r';
				break;
			case '\b':
				r = 'b';
				break;
			case '\f':
				r = 'f';
				break;
			case '\a':
				r = 'a';
				break;
			case '\v':
				r = 'v';
				break;
		}
		if (r) {
			if (buf) {
				buf[d++] = '\\';
				buf[d++] = r;
			} else {
				d += 2;
			}
		} else {
			if (flagEscapeNonAscii && ((sl_uint8)c < 32 || (sl_uint8)c > 126)) {
				if (sizeof(CT) == 1) {
					if (buf) {
						sl_uint8 t = (sl_uint8)c;
						buf[d++] = '\\';
						buf[d++] = 'x';
						buf[d++] = _StringConv_radixPatternLower[(t >> 4) & 15];
						buf[d++] = _StringConv_radixPatternLower[t & 15];
					} else {
						d += 4;
					}
				} else {
					if (buf) {
						sl_uint16 t = (sl_uint16)c;
						buf[d++] = '\\';
						buf[d++] = 'x';
						buf[d++] = _StringConv_radixPatternLower[(t >> 12) & 15];
						buf[d++] = _StringConv_radixPatternLower[(t >> 8) & 15];
						buf[d++] = _StringConv_radixPatternLower[(t >> 4) & 15];
						buf[d++] = _StringConv_radixPatternLower[t & 15];
					} else {
						d += 6;
					}
				}
				
			} else {
				if (buf) {
					buf[d++] = c;
				} else {
					d++;
				}
			}
		}
	}
	if (flagAddQuote) {
		if (buf) {
			buf[d++] = flagDoubleQuote ? '"' : '\'';
		} else {
			d++;
		}
	}
	return d;
}

String8 String8::applyBackslashEscapes(sl_bool flagDoubleQuote, sl_bool flagAddQuote, sl_bool flagEscapeNonAscii)
{
	sl_size n = _String_applyBackslashEscapes<String8, sl_char8>(*this, flagDoubleQuote, flagAddQuote, flagEscapeNonAscii, sl_null);
	if (n == 0) {
		return String8::getEmpty();
	}
	String8 ret = allocate(n);
	if (ret.isEmpty()) {
		return String8::null();
	}
	_String_applyBackslashEscapes<String8, sl_char8>(*this, flagDoubleQuote, flagAddQuote, flagEscapeNonAscii, ret.getData());
	return ret;
}

String16 String16::applyBackslashEscapes(sl_bool flagDoubleQuote, sl_bool flagAddQuote, sl_bool flagEscapeNonAscii)
{
	sl_size n = _String_applyBackslashEscapes<String16, sl_char16>(*this, flagDoubleQuote, flagAddQuote, flagEscapeNonAscii, sl_null);
	String16 ret = allocate(n);
	if (ret.isEmpty()) {
		return String16::null();
	}
	_String_applyBackslashEscapes<String16, sl_char16>(*this, flagDoubleQuote, flagAddQuote, flagEscapeNonAscii, ret.getData());
	return ret;
}

String8 SafeString8::applyBackslashEscapes(sl_bool flagDoubleQuote, sl_bool flagAddQuote, sl_bool flagEscapeNonAscii)
{
	String8 s(*this);
	return s.applyBackslashEscapes(flagDoubleQuote, flagAddQuote, flagEscapeNonAscii);
}

String16 SafeString16::applyBackslashEscapes(sl_bool flagDoubleQuote, sl_bool flagAddQuote, sl_bool flagEscapeNonAscii)
{
	String16 s(*this);
	return s.applyBackslashEscapes(flagDoubleQuote, flagAddQuote, flagEscapeNonAscii);
}

template <class ST, class CT>
SLIB_INLINE ST _String_parseBackslashEscapes(const CT* sz, sl_size n, sl_size* lengthParsed, sl_bool* outFlagError)
{
	if (lengthParsed) {
		*lengthParsed = 0;
	}
	if (outFlagError) {
		*outFlagError = sl_true;
	}
	if (n <= 0) {
		return ST::null();
	}
	sl_bool flagDoubleQuote = sl_false;
	CT chEnd = 0;
	if (sz[0] == '"') {
		flagDoubleQuote = sl_true;
		chEnd = '"';
	} else if (sz[0] == '\'') {
		flagDoubleQuote = sl_false;
		chEnd = '\'';
	} else {
		return ST::null();
	}
	SLIB_SCOPED_BUFFER(CT, 2048, buf, n);
	if (buf == sl_null) {
		return ST::null();
	}
	sl_size len = 0;
	sl_bool flagSuccess = sl_false;
	sl_size i = 1;
	for (; i < n; i++) {
		CT ch = sz[i];
		sl_bool flagError = sl_false;
		sl_bool flagBackslash = sl_false;
		switch (ch) {
			case '\\':
				flagBackslash = sl_true;
				i++;
				if (i < n) {
					ch = sz[i];
					switch (ch) {
						case '\\':
						case '"':
						case '\'':
							break;
						case 'n':
							ch = '\n';
							break;
						case 'r':
							ch = '\r';
							break;
						case 't':
							ch = '\t';
							break;
						case 'b':
							ch = '\b';
							break;
						case 'f':
							ch = '\f';
							break;
						case 'a':
							ch = '\a';
							break;
						case '0': case '1': case '2': case '3':
						case '4': case '5': case '6': case '7':
						{
							i++;
							sl_size nh = 2;
							sl_uint32 t = ch - '0';
							while (i < n && nh > 0) {
								ch = sz[i];
								if (ch >= '0' && ch < '8') {
									t = (t << 3) | (ch - '0');
									i++;
								} else {
									break;
								}
							}
							i--;
							ch = (CT)t;
							break;
						}
						case 'x':
						{
							i++;
							sl_uint32 h = SLIB_CHAR_HEX_TO_INT(sz[i]);
							if (h < 16) {
								i++;
								sl_uint32 t = h;
								sl_size nh;
								if (sizeof(CT) == 1) {
									nh = 1;
								} else {
									nh = 3;
								}
								while (i < n && nh > 0) {
									ch = sz[i];
									h = SLIB_CHAR_HEX_TO_INT(ch);
									if (h < 16) {
										t = (t << 4) | h;
										i++;
									} else {
										break;
									}
								}
							} else {
								flagError = sl_true;
							}
							i--;
							break;
						}
						case 'u':
						{
							if (i + 4 < n) {
								i++;
								sl_uint16 t = 0;
								for (int k = 0; k < 4; k++) {
									sl_uint16 h = SLIB_CHAR_HEX_TO_INT(ch);
									if (h < 16) {
										t = (t << 4) | h;
										i++;
									} else {
										flagError = sl_true;
										break;
									}
								}
								if (!flagError) {
									if (sizeof(CT) == 1) {
										sl_char8 u[3];
										sl_size nu = Charsets::utf16ToUtf8((sl_char16*)&t, 1, u, 3);
										if (nu > 0) {
											for (sl_size iu = 0; iu < nu - 1; iu++) {
												buf[len++] = (CT)(u[iu]);
											}
											ch = (CT)(u[nu - 1]);
										} else {
											flagError = sl_true;
										}
									} else {
										ch = (CT)t;
									}
								}
								i--;
							} else {
								flagError = sl_true;
							}
							break;
						}
						case 'U':
						{
							if (i + 8 < n) {
								i++;
								sl_uint32 t = 0;
								for (int k = 0; k < 4; k++) {
									sl_uint32 h = SLIB_CHAR_HEX_TO_INT(ch);
									if (h < 16) {
										t = (t << 4) | h;
										i++;
									} else {
										flagError = sl_true;
										break;
									}
								}
								if (!flagError) {
									if (sizeof(CT) == 1) {
										sl_char8 u[6];
										sl_size nu = Charsets::utf32ToUtf8(&t, 1, u, 6);
										if (nu > 0) {
											for (sl_size iu = 0; iu < nu - 1; iu++) {
												buf[len++] = (CT)(u[iu]);
											}
											ch = (CT)(u[nu - 1]);
										} else {
											flagError = sl_true;
										}
									} else {
										sl_char16 u[2];
										sl_size nu = Charsets::utf32ToUtf16(&t, 1, u, 2);
										if (nu > 0) {
											for (sl_size iu = 0; iu < nu - 1; iu++) {
												buf[len++] = (CT)(u[iu]);
											}
											ch = (CT)(u[nu - 1]);
										} else {
											flagError = sl_true;
										}
									}
								}
								i--;
							} else {
								flagError = sl_true;
							}
							break;
						}
						default:
							flagError = sl_true;
							break;
					}
				} else {
					flagError = sl_true;
				}
				break;
			case 0:
			case '\r':
			case '\n':
			case '\v':
				flagError = sl_true;
				break;
		}
		if (flagError) {
			break;
		} else {
			if (ch == chEnd && ! flagBackslash) {
				flagSuccess = sl_true;
				i++;
				break;
			} else {
				buf[len++] = ch;
			}
		}
	}
	if (lengthParsed) {
		*lengthParsed = i;
	}
	ST ret;
	if (flagSuccess) {
		ret = ST(buf, len);
		if (outFlagError) {
			*outFlagError = sl_false;
		}
	}
	return ret;
}

String8 String8::parseBackslashEscapes(const sl_char8* sz, sl_size n, sl_size* lengthParsed, sl_bool* outFlagError)
{
	return _String_parseBackslashEscapes<String8, sl_char8>(sz, n, lengthParsed, outFlagError);
}

String16 String16::parseBackslashEscapes(const sl_char16* sz, sl_size n, sl_size* lengthParsed, sl_bool* outFlagError)
{
	return _String_parseBackslashEscapes<String16, sl_char16>(sz, n, lengthParsed, outFlagError);
}

String8 String8::parseBackslashEscapes(sl_size* lengthParsed, sl_bool* flagError) const
{
    return parseBackslashEscapes(getData(), getLength(), lengthParsed, flagError);
}

String16 String16::parseBackslashEscapes(sl_size* lengthParsed, sl_bool* flagError) const
{
	return parseBackslashEscapes(getData(), getLength(), lengthParsed, flagError);
}

String8 SafeString8::parseBackslashEscapes(sl_size* lengthParsed, sl_bool* flagError) const
{
	String8 s(*this);
	return s.parseBackslashEscapes(lengthParsed, flagError);
}

String16 SafeString16::parseBackslashEscapes(sl_size* lengthParsed, sl_bool* flagError) const
{
	String16 s(*this);
	return s.parseBackslashEscapes(lengthParsed, flagError);
}

template <class CT>
SLIB_INLINE sl_size _String_countLineNumber(const CT* input, sl_size len, sl_size* columnLast)
{
	sl_size line = 1;
	sl_size col = 1;
	for (sl_size i = 0; i < len; i++) {
		CT ch = input[i];
		if (ch == '\r') {
			line++;
			col = 0;
			if (i + 1 < len && input[i+1] == '\n') {
				i++;
			}
		} else if (ch == '\n') {
			line++;
			col = 0;
		}
		col++;
	}
	if (columnLast) {
		*columnLast = col;
	}
	return line;
}

sl_size String8::countLineNumber(const sl_char8* input, sl_size len, sl_size* columnLast)
{
	return _String_countLineNumber(input, len, columnLast);
}

sl_size String16::countLineNumber(const sl_char16* input, sl_size len, sl_size* columnLast)
{
	return _String_countLineNumber(input, len, columnLast);
}

sl_size String8::countLineNumber(sl_size pos, sl_size* column) const
{
	return countLineNumber(getData(), Math::min(getLength(), pos), column);
}

sl_size String16::countLineNumber(sl_size pos, sl_size* column) const
{
	return countLineNumber(getData(), Math::min(getLength(), pos), column);
}

sl_size SafeString8::countLineNumber(sl_size pos, sl_size* column) const
{
	String8 s(*this);
	return s.countLineNumber(pos, column);
}

sl_size SafeString16::countLineNumber(sl_size pos, sl_size* column) const
{
	String16 s(*this);
	return s.countLineNumber(pos, column);
}

/**********************************************************
					String Conversion
**********************************************************/

#define MAX_NUMBER_STR_LEN 256
#define MAX_PRECISION 50

const char _string_conv_radix_pattern_lower[65] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ@_";
const char* _StringConv_radixPatternLower = _string_conv_radix_pattern_lower;
const char _string_conv_radix_pattern_upper[65] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz@_";
const char* _StringConv_radixPatternUpper = _string_conv_radix_pattern_upper;

const sl_uint8 _string_conv_radix_inverse_pattern_big[128] = {
	/*00*/ 255, 255, 255, 255, 255, 255, 255, 255,
	/*08*/ 255, 255, 255, 255, 255, 255, 255, 255,
	/*10*/ 255, 255, 255, 255, 255, 255, 255, 255,
	/*18*/ 255, 255, 255, 255, 255, 255, 255, 255,
	/*20*/ 255, 255, 255, 255, 255, 255, 255, 255,
	/*28*/ 255, 255, 255, 255, 255, 255, 255, 255,
	/*30*/ 0,   1,   2,   3,   4,   5,   6,   7,
	/*38*/ 8,   9,   255, 255, 255, 255, 255, 255,
	/*40*/ 62,  36,  37,  38,  39,  40,  41,  42,
	/*48*/ 43,  44,  45,  46,  47,  48,  49,  50,
	/*50*/ 51,  52,  53,  54,  55,  56,  57,  58,
	/*58*/ 59,  60,  61,  255, 255, 255, 255, 63,
	/*60*/ 255, 10,  11,  12,  13,  14,  15,  16,
	/*68*/ 17,  18,  19,  20,  21,  22,  23,  24,
	/*70*/ 25,  26,  27,  28,  29,  30,  31,  32,
	/*78*/ 33,  34,  35,  255, 255, 255, 255, 255
};
const sl_uint8* _StringConv_radixInversePatternBig = _string_conv_radix_inverse_pattern_big;

const sl_uint8 _string_conv_radix_inverse_pattern_small[128] = {
	/*00*/ 255, 255, 255, 255, 255, 255, 255, 255,
	/*08*/ 255, 255, 255, 255, 255, 255, 255, 255,
	/*10*/ 255, 255, 255, 255, 255, 255, 255, 255,
	/*18*/ 255, 255, 255, 255, 255, 255, 255, 255,
	/*20*/ 255, 255, 255, 255, 255, 255, 255, 255,
	/*28*/ 255, 255, 255, 255, 255, 255, 255, 255,
	/*30*/ 0,   1,   2,   3,   4,   5,   6,   7,
	/*38*/ 8,   9,   255, 255, 255, 255, 255, 255,
	/*40*/ 255, 10,  11,  12,  13,  14,  15,  16,
	/*48*/ 17,  18,  19,  20,  21,  22,  23,  24,
	/*50*/ 25,  26,  27,  28,  29,  30,  31,  32,
	/*58*/ 33,  34,  35,  255, 255, 255, 255, 255,
	/*60*/ 255, 10,  11,  12,  13,  14,  15,  16,
	/*68*/ 17,  18,  19,  20,  21,  22,  23,  24,
	/*70*/ 25,  26,  27,  28,  29,  30,  31,  32,
	/*78*/ 33,  34,  35,  255, 255, 255, 255, 255
};
const sl_uint8* _StringConv_radixInversePatternSmall = _string_conv_radix_inverse_pattern_small;

template <class IT, class CT>
SLIB_INLINE sl_reg _String_parseInt(sl_int32 radix, const CT* sz, sl_size i, sl_size n, IT* _out)
{
	if (i >= n) {
		return SLIB_PARSE_ERROR;
	}
	sl_int32 r = radix;
	
	const sl_uint8* pattern = r <= 36 ? _string_conv_radix_inverse_pattern_small : _string_conv_radix_inverse_pattern_big;
	IT v = 0;
	sl_bool bMinus = sl_false;
	sl_bool bEmpty = sl_true;
	if (sz[i] == '-') {
		i++;
		bMinus = sl_true;
	}
	for (; i < n; i++) {
		sl_uint32 c = (sl_uint32)(sz[i]);
		if (c != '\t' && c != ' ') {
			break;
		}
	}
	for (; i < n; i++) {
		sl_uint32 c = (sl_uint32)(sz[i]);
		sl_uint32 m = c < 128 ? pattern[c] : 255;
		if (m < (sl_uint32)r) {
			v = v * r + m;
			bEmpty = sl_false;
		} else {
			break;
		}
	}
	if (bEmpty) {
		return SLIB_PARSE_ERROR;
	}
	if (bMinus) {
		v = -v;
	}
	if (_out) {
		*_out = v;
	}
	return i;
}

template <class IT, class CT>
SLIB_INLINE sl_reg _String_parseUint(sl_int32 radix, const CT* sz, sl_size i, sl_size n, IT* _out)
{
	if (i >= n) {
		return SLIB_PARSE_ERROR;
	}
	sl_uint32 r = radix;
	sl_bool bEmpty = sl_true;
	const sl_uint8* pattern = r <= 36 ? _string_conv_radix_inverse_pattern_small : _string_conv_radix_inverse_pattern_big;
	IT v = 0;
	for (; i < n; i++) {
		sl_uint32 c = (sl_uint32)(sz[i]);
		sl_uint32 m = c < 128 ? pattern[c] : 255;
		if (m < r) {
			v = v * r + m;
			bEmpty = sl_false;
		} else {
			break;
		}
	}
	if (bEmpty) {
		return SLIB_PARSE_ERROR;
	}
	if (_out) {
		*_out = v;
	}
	return i;
}

sl_reg String8::parseInt32(sl_int32 radix, sl_int32* _out, const sl_char8* sz, sl_size posBegin, sl_size len)
{
	return _String_parseInt(radix, sz, posBegin, len, _out);
}

sl_reg String16::parseInt32(sl_int32 radix, sl_int32* _out, const sl_char16* sz, sl_size posBegin, sl_size len)
{
	return _String_parseInt(radix, sz, posBegin, len, _out);
}

sl_bool String8::parseInt32(sl_int32 radix, sl_int32* _out) const
{
	sl_size n = getLength();
	if (n == 0) {
		return sl_false;
	}
	return _String_parseInt(radix, getData(), 0, n, _out) == n;
}

sl_bool String16::parseInt32(sl_int32 radix, sl_int32* _out) const
{
	sl_size n = getLength();
	if (n == 0) {
		return sl_false;
	}
	return _String_parseInt(radix, getData(), 0, n, _out) == n;
}

sl_bool SafeString8::parseInt32(sl_int32 radix, sl_int32* _out) const
{
	String8 s(*this);
	return s.parseInt32(radix, _out);
}

sl_bool SafeString16::parseInt32(sl_int32 radix, sl_int32* _out) const
{
	String16 s(*this);
	return s.parseInt32(radix, _out);
}

sl_int32 String8::parseInt32(sl_int32 radix, sl_int32 def) const
{
	sl_int32 _out = def;
	parseInt32(radix, &_out);
	return _out;
}

sl_int32 String16::parseInt32(sl_int32 radix, sl_int32 def) const
{
	sl_int32 _out = def;
	parseInt32(radix, &_out);
	return _out;
}

sl_int32 SafeString8::parseInt32(sl_int32 radix, sl_int32 def) const
{
	String8 s(*this);
	return s.parseInt32(radix, def);
}

sl_int32 SafeString16::parseInt32(sl_int32 radix, sl_int32 def) const
{
	String16 s(*this);
	return s.parseInt32(radix, def);
}


sl_reg String8::parseUint32(sl_int32 radix, sl_uint32* _out, const sl_char8* sz, sl_size posBegin, sl_size len)
{
	return _String_parseUint(radix, sz, posBegin, len, _out);
}

sl_reg String16::parseUint32(sl_int32 radix, sl_uint32* _out, const sl_char16* sz, sl_size posBegin, sl_size len)
{
	return _String_parseUint(radix, sz, posBegin, len, _out);
}

sl_bool String8::parseUint32(sl_int32 radix, sl_uint32* _out) const
{
	sl_size n = getLength();
	if (n == 0) {
		return sl_false;
	}
	return _String_parseUint(radix, getData(), 0, n, _out) == n;
}

sl_bool String16::parseUint32(sl_int32 radix, sl_uint32* _out) const
{
	sl_size n = getLength();
	if (n == 0) {
		return sl_false;
	}
	return _String_parseUint(radix, getData(), 0, n, _out) == n;
}

sl_bool SafeString8::parseUint32(sl_int32 radix, sl_uint32* _out) const
{
	String8 s(*this);
	return s.parseUint32(radix, _out);
}

sl_bool SafeString16::parseUint32(sl_int32 radix, sl_uint32* _out) const
{
	String16 s(*this);
	return s.parseUint32(radix, _out);
}

sl_uint32 String8::parseUint32(sl_int32 radix, sl_uint32 def) const
{
	sl_uint32 _out = def;
	parseUint32(radix, &_out);
	return _out;
}

sl_uint32 String16::parseUint32(sl_int32 radix, sl_uint32 def) const
{
	sl_uint32 _out = def;
	parseUint32(radix, &_out);
	return _out;
}

sl_uint32 SafeString8::parseUint32(sl_int32 radix, sl_uint32 def) const
{
	String8 s(*this);
	return s.parseUint32(radix, def);
}

sl_uint32 SafeString16::parseUint32(sl_int32 radix, sl_uint32 def) const
{
	String16 s(*this);
	return s.parseUint32(radix, def);
}


sl_reg String8::parseInt64(sl_int32 radix, sl_int64* _out, const sl_char8* sz, sl_size posBegin, sl_size len)
{
	return _String_parseInt(radix, sz, posBegin, len, _out);
}

sl_reg String16::parseInt64(sl_int32 radix, sl_int64* _out, const sl_char16* sz, sl_size posBegin, sl_size len)
{
	return _String_parseInt(radix, sz, posBegin, len, _out);
}

sl_bool String8::parseInt64(sl_int32 radix, sl_int64* _out) const
{
	sl_size n = getLength();
	if (n == 0) {
		return sl_false;
	}
	return _String_parseInt(radix, getData(), 0, n, _out) == n;
}

sl_bool String16::parseInt64(sl_int32 radix, sl_int64* _out) const
{
	sl_size n = getLength();
	if (n == 0) {
		return sl_false;
	}
	return _String_parseInt(radix, getData(), 0, n, _out) == n;
}

sl_bool SafeString8::parseInt64(sl_int32 radix, sl_int64* _out) const
{
	String8 s(*this);
	return s.parseInt64(radix, _out);
}

sl_bool SafeString16::parseInt64(sl_int32 radix, sl_int64* _out) const
{
	String16 s(*this);
	return s.parseInt64(radix, _out);
}

sl_int64 String8::parseInt64(sl_int32 radix, sl_int64 def) const
{
	sl_int64 _out = def;
	parseInt64(radix, &_out);
	return _out;
}

sl_int64 String16::parseInt64(sl_int32 radix, sl_int64 def) const
{
	sl_int64 _out = def;
	parseInt64(radix, &_out);
	return _out;
}

sl_int64 SafeString8::parseInt64(sl_int32 radix, sl_int64 def) const
{
	String8 s(*this);
	return s.parseInt64(radix, def);
}

sl_int64 SafeString16::parseInt64(sl_int32 radix, sl_int64 def) const
{
	String16 s(*this);
	return s.parseInt64(radix, def);
}


sl_reg String8::parseUint64(sl_int32 radix, sl_uint64* _out, const char* sz, sl_size posBegin, sl_size len)
{
	return _String_parseUint(radix, sz, posBegin, len, _out);
}

sl_reg String16::parseUint64(sl_int32 radix, sl_uint64* _out, const sl_char16* sz, sl_size posBegin, sl_size len)
{
	return _String_parseUint(radix, sz, posBegin, len, _out);
}

sl_bool String8::parseUint64(sl_int32 radix, sl_uint64* _out) const
{
	sl_size n = getLength();
	if (n == 0) {
		return sl_false;
	}
	return _String_parseUint(radix, getData(), 0, n, _out) == n;
}

sl_bool String16::parseUint64(sl_int32 radix, sl_uint64* _out) const
{
	sl_size n = getLength();
	if (n == 0) {
		return sl_false;
	}
	return _String_parseUint(radix, getData(), 0, n, _out) == n;
}

sl_bool SafeString8::parseUint64(sl_int32 radix, sl_uint64* _out) const
{
	String8 s(*this);
	return s.parseUint64(radix, _out);
}

sl_bool SafeString16::parseUint64(sl_int32 radix, sl_uint64* _out) const
{
	String16 s(*this);
	return s.parseUint64(radix, _out);
}

sl_uint64 String8::parseUint64(sl_int32 radix, sl_uint64 def) const
{
	sl_uint64 _out = def;
	parseUint64(radix, &_out);
	return _out;
}

sl_uint64 String16::parseUint64(sl_int32 radix, sl_uint64 def) const
{
	sl_uint64 _out = def;
	parseUint64(radix, &_out);
	return _out;
}

sl_uint64 SafeString8::parseUint64(sl_int32 radix, sl_uint64 def) const
{
	String8 s(*this);
	return s.parseUint64(radix, def);
}

sl_uint64 SafeString16::parseUint64(sl_int32 radix, sl_uint64 def) const
{
	String16 s(*this);
	return s.parseUint64(radix, def);
}


sl_bool String8::parseInt(sl_int32 radix, sl_reg* _out) const
{
#ifdef SLIB_ARCH_IS_64BIT
	return parseInt64(radix, _out);
#else
	return parseInt32(radix, _out);
#endif
}

sl_bool String16::parseInt(sl_int32 radix, sl_reg* _out) const
{
#ifdef SLIB_ARCH_IS_64BIT
	return parseInt64(radix, _out);
#else
	return parseInt32(radix, _out);
#endif
}

sl_bool SafeString8::parseInt(sl_int32 radix, sl_reg* _out) const
{
	String8 s(*this);
	return s.parseInt(radix, _out);
}

sl_bool SafeString16::parseInt(sl_int32 radix, sl_reg* _out) const
{
	String16 s(*this);
	return s.parseInt(radix, _out);
}

sl_reg String8::parseInt(sl_int32 radix, sl_reg def) const
{
	sl_reg _out = def;
	parseInt(radix, &_out);
	return _out;
}

sl_reg String16::parseInt(sl_int32 radix, sl_reg def) const
{
	sl_reg _out = def;
	parseInt(radix, &_out);
	return _out;
}

sl_reg SafeString8::parseInt(sl_int32 radix, sl_reg def) const
{
	String8 s(*this);
	return s.parseInt(radix, def);
}

sl_reg SafeString16::parseInt(sl_int32 radix, sl_reg def) const
{
	String16 s(*this);
	return s.parseInt(radix, def);
}


sl_bool String8::parseSize(sl_int32 radix, sl_size* _out) const
{
#ifdef SLIB_ARCH_IS_64BIT
	return parseUint64(radix, _out);
#else
	return parseUint32(radix, _out);
#endif
}

sl_bool String16::parseSize(sl_int32 radix, sl_size* _out) const
{
#ifdef SLIB_ARCH_IS_64BIT
	return parseUint64(radix, _out);
#else
	return parseUint32(radix, _out);
#endif
}

sl_bool SafeString8::parseSize(sl_int32 radix, sl_size* _out) const
{
	String8 s(*this);
	return s.parseSize(radix, _out);
}

sl_bool SafeString16::parseSize(sl_int32 radix, sl_size* _out) const
{
	String16 s(*this);
	return s.parseSize(radix, _out);
}

sl_size String8::parseSize(sl_int32 radix, sl_size def) const
{
	sl_size _out = def;
	parseSize(radix, &_out);
	return _out;
}

sl_size String16::parseSize(sl_int32 radix, sl_size def) const
{
	sl_size _out = def;
	parseSize(radix, &_out);
	return _out;
}

sl_size SafeString8::parseSize(sl_int32 radix, sl_size def) const
{
	String8 s(*this);
	return s.parseSize(radix, def);
}

sl_size SafeString16::parseSize(sl_int32 radix, sl_size def) const
{
	String16 s(*this);
	return s.parseSize(radix, def);
}


template <class FT, class CT>
SLIB_INLINE sl_reg _String_parseFloat(const CT* sz, sl_size i, sl_size n, FT* _out)
{
	if (i >= n) {
		return SLIB_PARSE_ERROR; // input string is empty
	}
	
	sl_bool bMinus = sl_false;
	sl_bool bEmpty = sl_true;
	
	if (sz[i] == '-') {
		i++;
		bMinus = sl_true;
	}
	for (; i < n; i++) {
		sl_uint32 c = (sl_uint32)(sz[i]);
		if (!(SLIB_CHAR_IS_SPACE_TAB(c))) {
			break;
		}
	}
	
	FT v = 0;
	sl_uint32 vi = 0;
	sl_bool flagMulInt = sl_true;

	for (; i < n; i++) {
		sl_uint32 c = (sl_uint32)sz[i];
		if (SLIB_CHAR_IS_DIGIT(c)) {
			if (flagMulInt) {
				vi = vi * 10 + (c - '0');
				if (vi >= 214748364) {
					v = (FT)vi;
					flagMulInt = sl_false;
				}
			} else {
				v = v * 10 + (c - '0');
			}
			bEmpty = sl_false;
		} else {
			break;
		}
	}
	if (bEmpty) {
		return SLIB_PARSE_ERROR; // integral number is required
	}
	if (flagMulInt) {
		v = (FT)vi;
	}
	
	if (i < n) {
		if (sz[i] == '.') {
			i++;
			bEmpty = sl_true;
			FT weight = (FT)(0.1);
			for (; i < n; i++) {
				sl_uint32 c = (sl_uint32)sz[i];
				if (SLIB_CHAR_IS_DIGIT(c)) {
					v = v + (c - '0') * weight;
					weight /= 10;
					bEmpty = sl_false;
				} else {
					break;
				}
			}
			if (bEmpty) {
				return SLIB_PARSE_ERROR; // fraction number is required
			}
		}
		if (i < n) {
			if (sz[i] == 'e' || sz[i] == 'E') {
				i++;
				bEmpty = sl_true;
				sl_bool bMinuxExp = sl_false;
				FT exp = 0;
				if (i < n && (sz[i] == '+' || sz[i] == '-')) {
					if (sz[i] == '-') {
						bMinuxExp = sl_true;
					}
					i++;
				}
				for (; i < n; i++) {
					sl_uint32 c = (sl_uint32)sz[i];
					if (SLIB_CHAR_IS_DIGIT(c)) {
						exp = exp * 10 + (c - '0');
						bEmpty = sl_false;
					} else {
						break; // invalid character
					}
				}
				if (bEmpty) {
					return SLIB_PARSE_ERROR; // exponent number is required
				}
				if (bMinuxExp) {
					exp = -exp;
				}
				v *= Math::pow((FT)(10.0), exp);
			}
		}
	}
	if (bMinus) {
		v = -v;
	}
	if (_out) {
		*_out = v;
	}
	return i;
}

sl_reg String8::parseFloat(float* _out, const char* sz, sl_size posBegin, sl_size len)
{
	return _String_parseFloat(sz, posBegin, len, _out);
}

sl_reg String16::parseFloat(float* _out, const sl_char16* sz, sl_size posBegin, sl_size len)
{
	return _String_parseFloat(sz, posBegin, len, _out);
}

sl_bool String8::parseFloat(float* _out) const
{
	sl_size n = getLength();
	if (n == 0) {
		return sl_false;
	}
	return _String_parseFloat(getData(), 0, n, _out) == n;
}

sl_bool String16::parseFloat(float* _out) const
{
	sl_size n = getLength();
	if (n == 0) {
		return sl_false;
	}
	return _String_parseFloat(getData(), 0, n, _out) == n;
}

sl_bool SafeString8::parseFloat(float* _out) const
{
	String8 s(*this);
	return s.parseFloat(_out);
}

sl_bool SafeString16::parseFloat(float* _out) const
{
	String16 s(*this);
	return s.parseFloat(_out);
}

float String8::parseFloat(float def) const
{
	float _out = def;
	parseFloat(&_out);
	return _out;
}

float String16::parseFloat(float def) const
{
	float _out = def;
	parseFloat(&_out);
	return _out;
}

float SafeString8::parseFloat(float def) const
{
	String8 s(*this);
	return s.parseFloat(def);
}

float SafeString16::parseFloat(float def) const
{
	String16 s(*this);
	return s.parseFloat(def);
}


sl_reg String8::parseDouble(double* _out, const char* sz, sl_size posBegin, sl_size n)
{
	return _String_parseFloat(sz, posBegin, n, _out);
}

sl_reg String16::parseDouble(double* _out, const sl_char16* sz, sl_size posBegin, sl_size n)
{
	return _String_parseFloat(sz, posBegin, n, _out);
}

sl_bool String8::parseDouble(double* _out) const
{
	sl_size n = getLength();
	if (n == 0) {
		return sl_false;
	}
	return _String_parseFloat(getData(), 0, n, _out) == n;
}

sl_bool String16::parseDouble(double* _out) const
{
	sl_size n = getLength();
	if (n == 0) {
		return sl_false;
	}
	return _String_parseFloat(getData(), 0, n, _out) == n;
}

sl_bool SafeString8::parseDouble(double* _out) const
{
	String8 s(*this);
	return s.parseDouble(_out);
}

sl_bool SafeString16::parseDouble(double* _out) const
{
	String16 s(*this);
	return s.parseDouble(_out);
}

double String8::parseDouble(double def) const
{
	double _out = def;
	parseDouble(&_out);
	return _out;
}

double String16::parseDouble(double def) const
{
	double _out = def;
	parseDouble(&_out);
	return _out;
}

double SafeString8::parseDouble(double def) const
{
	String8 s(*this);
	return s.parseDouble(def);
}

double SafeString16::parseDouble(double def) const
{
	String16 s(*this);
	return s.parseDouble(def);
}

template <class CT>
SLIB_INLINE sl_reg _String_parseBoolean(const CT* sz, sl_size i, sl_size n, sl_bool* _out)
{
	if (i >= n) {
		return SLIB_PARSE_ERROR;
	}
	sl_bool f = sl_false;
	switch(sz[i]) {
		case '1':
			i++;
			f = sl_true;
			break;
		case '0':
			i++;
			f = sl_false;
			break;
		case 'T':
		case 't':
			if (i + 4 <= n) {
				i++;
				if (sz[i] == 'R' || sz[i] == 'r') {
					i++;
					if (sz[i] == 'U' || sz[i] == 'u') {
						i++;
						if (sz[i] == 'E' || sz[i] == 'e') {
							i++;
							f = sl_true;
							break;
						}
					}
				}
			}
			return SLIB_PARSE_ERROR;
		case 'F':
		case 'f':
			if (i + 5 <= n) {
				i++;
				if (sz[i] == 'A' || sz[i] == 'a') {
					i++;
					if (sz[i] == 'L' || sz[i] == 'l') {
						i++;
						if (sz[i] == 'S' || sz[i] == 's') {
							i++;
							if (sz[i] == 'E' || sz[i] == 'e') {
								i++;
								f = sl_false;
								break;
							}
						}
					}
				}
			}
			return SLIB_PARSE_ERROR;
		case 'Y':
		case 'y':
			i++;
			if (i + 2 <= n && (sz[i] == 'E' || sz[i] == 'e')) {
				i++;
				if (sz[i] == 'S' || sz[i] == 's') {
					i++;
					f = sl_true;
					break;
				}
			} else {
				f = sl_true;
				break;
			}
			return SLIB_PARSE_ERROR;
		case 'N':
		case 'n':
			i++;
			if (i + 1 <= n && (sz[i] == 'O' || sz[i] == 'o')) {
				i++;
			}
			f = sl_false;
			break;
	}
	if (i < n && sz[i] && SLIB_CHAR_IS_C_NAME(sz[i])) {
		return SLIB_PARSE_ERROR;
	}
	if (_out) {
		*_out = f;
	}
	return i;
}

sl_reg String8::parseBoolean(sl_bool* _out, const char* sz, sl_size posBegin, sl_size len)
{
	return _String_parseBoolean(sz, posBegin, len, _out);
}

sl_reg String16::parseBoolean(sl_bool* _out, const sl_char16* sz, sl_size posBegin, sl_size len)
{
	return _String_parseBoolean(sz, posBegin, len, _out);
}

sl_bool String8::parseBoolean(sl_bool* _out) const
{
	sl_size n = getLength();
	if (n == 0) {
		return sl_false;
	}
	return _String_parseBoolean(getData(), 0, n, _out) == n;
}

sl_bool String16::parseBoolean(sl_bool* _out) const
{
	sl_size n = getLength();
	if (n == 0) {
		return sl_false;
	}
	return _String_parseBoolean(getData(), 0, n, _out) == n;
}

sl_bool SafeString8::parseBoolean(sl_bool* _out) const
{
	String8 s(*this);
	return s.parseBoolean(_out);
}

sl_bool SafeString16::parseBoolean(sl_bool* _out) const
{
	String16 s(*this);
	return s.parseBoolean(_out);
}

sl_bool String8::parseBoolean(sl_bool def) const
{
	sl_bool _out = def;
	parseBoolean(&_out);
	return _out;
}

sl_bool String16::parseBoolean(sl_bool def) const
{
	sl_bool _out = def;
	parseBoolean(&_out);
	return _out;
}

sl_bool SafeString8::parseBoolean(sl_bool def) const
{
	String8 s(*this);
	return s.parseBoolean(def);
}

sl_bool SafeString16::parseBoolean(sl_bool def) const
{
	String16 s(*this);
	return s.parseBoolean(def);
}


template <class CT>
SLIB_INLINE sl_reg _String_parseHexString(const CT* sz, sl_size i, sl_size n, void* _out)
{
	if (i >= n) {
		return SLIB_PARSE_ERROR;
	}
	sl_uint8* buf = (sl_uint8*)(_out);
	sl_size k = 0;
	for (; i < n; i += 2) {
		sl_uint32 v1, v2;
		{
			sl_uint32 ch = (sl_uint32)sz[i];
			if (ch >= '0' && ch <= '9') {
				v1 = ch - '0';
			} else if (ch >= 'A' && ch <= 'F') {
				v1 = ch - 'A' + 10;
			} else if (ch >= 'a' && ch <= 'f') {
				v1 = ch - 'a' + 10;
			} else {
				break;
			}
		}
		{
			sl_uint32 ch = (sl_uint32)sz[i + 1];
			if (ch >= '0' && ch <= '9') {
				v2 = ch - '0';
			} else if (ch >= 'A' && ch <= 'F') {
				v2 = ch - 'A' + 10;
			} else if (ch >= 'a' && ch <= 'f') {
				v2 = ch - 'a' + 10;
			} else {
				break;
			}
		}
		buf[k++] = (sl_uint8)((v1 << 4) | v2);
	}
	return i;
}

sl_reg String8::parseHexString(void* _out, const char* sz, sl_size posBegin, sl_size len)
{
	return _String_parseHexString(sz, posBegin, len, _out);
}

sl_reg String16::parseHexString(void* _out, const sl_char16* sz, sl_size posBegin, sl_size len)
{
	return _String_parseHexString(sz, posBegin, len, _out);
}

sl_bool String8::parseHexString(void* _out) const
{
	sl_size n = getLength();
	if (n == 0) {
		return sl_false;
	}
	return _String_parseHexString(getData(), 0, n, _out) == n;
}

sl_bool String16::parseHexString(void* _out) const
{
	sl_size n = getLength();
	if (n == 0) {
		return sl_false;
	}
	return _String_parseHexString(getData(), 0, n, _out) == n;
}

sl_bool SafeString8::parseHexString(void* _out) const
{
	String8 s(*this);
	return s.parseHexString(_out);
}

sl_bool SafeString16::parseHexString(void* _out) const
{
	String16 s(*this);
	return s.parseHexString(_out);
}



template <class IT, class UT, class ST, class CT>
SLIB_INLINE ST _String_fromInt(IT _value, sl_uint32 radix, sl_uint32 minWidth, sl_bool flagUpperCase, CT chGroup = sl_false, sl_bool flagSignPositive = sl_false, sl_bool flagLeadingSpacePositive = sl_false, sl_bool flagEncloseNagtive = sl_false)
{
	if (radix < 2 || radix > 64) {
		return ST::null();
	}
	
	const char* pattern = flagUpperCase && radix <= 36 ? _string_conv_radix_pattern_upper : _string_conv_radix_pattern_lower;
	
	CT buf[MAX_NUMBER_STR_LEN];
	
	sl_uint32 pos = MAX_NUMBER_STR_LEN;
	
	if (minWidth < 1) {
		minWidth = 1;
	}
	
	sl_bool flagMinus = sl_false;
	UT value;
	if (_value < 0) {
		value = -_value;
		flagMinus = sl_true;
		if (flagEncloseNagtive) {
			pos--;
			buf[pos] = ')';
		}
	} else {
		value = _value;
	}
	
	sl_uint32 nDigits = 0;
	while (value || minWidth > 0) {
		if (chGroup) {
			if (nDigits > 0) {
				if (pos > 0) {
					if (nDigits % 3 == 0) {
						pos--;
						buf[pos] = chGroup;
					}
				} else {
					break;
				}
			}
		}
		if (pos > 0) {
			pos--;
			buf[pos] = pattern[value % radix];
			value = value / radix;
			if (minWidth > 0) {
				minWidth--;
			}
			nDigits++;
		} else {
			break;
		}
	}
	
	if (flagMinus) {
		if (pos > 0) {
			pos--;
			buf[pos] = '-';
			if (flagEncloseNagtive) {
				if (pos > 0) {
					pos--;
					buf[pos] = '(';
				}
			}
		}
	} else {
		if (flagSignPositive) {
			if (pos > 0) {
				pos--;
				buf[pos] = '+';
			}
		}
		if (flagLeadingSpacePositive) {
			if (pos > 0) {
				pos--;
				buf[pos] = ' ';
			}
		}
	}
	return ST(buf + pos, MAX_NUMBER_STR_LEN - pos);
}

template <class IT, class ST, class CT>
SLIB_INLINE ST _String_fromUint(IT value, sl_uint32 radix, sl_uint32 minWidth, sl_bool flagUpperCase, CT chGroup = 0, sl_bool flagSignPositive = sl_false, sl_bool flagLeadingSpacePositive = sl_false)
{
	if (radix < 2 || radix > 64) {
		return ST::null();
	}
	
	const char* pattern = flagUpperCase && radix <= 36 ? _string_conv_radix_pattern_upper : _string_conv_radix_pattern_lower;
	
	CT buf[MAX_NUMBER_STR_LEN];
	
	sl_uint32 pos = MAX_NUMBER_STR_LEN;
	
	if (minWidth < 1) {
		minWidth = 1;
	}
	
	sl_uint32 nDigits = 0;
	while (value || minWidth > 0) {
		if (chGroup) {
			if (nDigits > 0) {
				if (pos > 0) {
					if (nDigits % 3 == 0) {
						pos--;
						buf[pos] = chGroup;
					}
				} else {
					break;
				}
			}
		}
		if (pos > 0) {
			pos--;
			buf[pos] = pattern[value % radix];
			value = value / radix;
			if (minWidth > 0) {
				minWidth--;
			}
			nDigits++;
		} else {
			break;
		}
	}
	
	if (flagSignPositive) {
		if (pos > 0) {
			pos--;
			buf[pos] = '+';
		}
	}
	if (flagLeadingSpacePositive) {
		if (pos > 0) {
			pos--;
			buf[pos] = ' ';
		}
	}
	
	return ST(buf + pos, MAX_NUMBER_STR_LEN - pos);
}

String8 String8::fromInt32(sl_int32 value, sl_uint32 radix, sl_uint32 minWidth, sl_bool flagUpperCase)
{
	return _String_fromInt<sl_int32, sl_uint32, String8, sl_char8>(value, radix, minWidth, flagUpperCase);
}

String16 String16::fromInt32(sl_int32 value, sl_uint32 radix, sl_uint32 minWidth, sl_bool flagUpperCase)
{
	return _String_fromInt<sl_int32, sl_uint32, String16, sl_char16>(value, radix, minWidth, flagUpperCase);
}

String8 String8::fromUint32(sl_uint32 value, sl_uint32 radix, sl_uint32 minWidth, sl_bool flagUpperCase)
{
	return _String_fromUint<sl_uint32, String8, sl_char8>(value, radix, minWidth, flagUpperCase);
}

String16 String16::fromUint32(sl_uint32 value, sl_uint32 radix, sl_uint32 minWidth, sl_bool flagUpperCase)
{
	return _String_fromUint<sl_uint32, String16, sl_char16>(value, radix, minWidth, flagUpperCase);
}

String8 String8::fromInt64(sl_int64 value, sl_uint32 radix, sl_uint32 minWidth, sl_bool flagUpperCase)
{
	return _String_fromInt<sl_int64, sl_uint64, String8, sl_char8>(value, radix, minWidth, flagUpperCase);
}

String16 String16::fromInt64(sl_int64 value, sl_uint32 radix, sl_uint32 minWidth, sl_bool flagUpperCase)
{
	return _String_fromInt<sl_int64, sl_uint64, String16, sl_char16>(value, radix, minWidth, flagUpperCase);
}

String8 String8::fromUint64(sl_uint64 value, sl_uint32 radix, sl_uint32 minWidth, sl_bool flagUpperCase)
{
	return _String_fromUint<sl_uint64, String8, sl_char8>(value, radix, minWidth, flagUpperCase);
}

String16 String16::fromUint64(sl_uint64 value, sl_uint32 radix, sl_uint32 minWidth, sl_bool flagUpperCase)
{
	return _String_fromUint<sl_uint64, String16, sl_char16>(value, radix, minWidth, flagUpperCase);
}

String8 String8::fromInt(sl_reg value, sl_uint32 radix, sl_uint32 minWidth, sl_bool flagUpperCase)
{
#ifdef SLIB_ARCH_IS_64BIT
	return fromInt64(value, radix, minWidth, flagUpperCase);
#else
	return fromInt32(value, radix, minWidth, flagUpperCase);
#endif
}

String16 String16::fromInt(sl_reg value, sl_uint32 radix, sl_uint32 minWidth, sl_bool flagUpperCase)
{
#ifdef SLIB_ARCH_IS_64BIT
	return fromInt64(value, radix, minWidth, flagUpperCase);
#else
	return fromInt32(value, radix, minWidth, flagUpperCase);
#endif
}

String8 String8::fromSize(sl_size value, sl_uint32 radix, sl_uint32 minWidth, sl_bool flagUpperCase)
{
#ifdef SLIB_ARCH_IS_64BIT
	return fromUint64(value, radix, minWidth, flagUpperCase);
#else
	return fromUint32(value, radix, minWidth, flagUpperCase);
#endif
}

String16 String16::fromSize(sl_size value, sl_uint32 radix, sl_uint32 minWidth, sl_bool flagUpperCase)
{
#ifdef SLIB_ARCH_IS_64BIT
	return fromUint64(value, radix, minWidth, flagUpperCase);
#else
	return fromUint32(value, radix, minWidth, flagUpperCase);
#endif
}

template <class FT, class ST, class CT>
SLIB_INLINE ST _String_fromFloat(FT value, sl_int32 precision, sl_bool flagZeroPadding, sl_int32 minWidthIntegral, CT chConv = 'g', CT chGroup = 0, sl_bool flagSignPositive = sl_false, sl_bool flagLeadingSpacePositive = sl_false, sl_bool flagEncloseNagtive = sl_false)
{
	
	CT buf[MAX_NUMBER_STR_LEN];
	
	if (Math::isNaN(value)) {
		return "NaN";
	}
	if (Math::isInfinite(value)) {
		return "Infinity";
	}

	if (minWidthIntegral > MAX_PRECISION) {
		minWidthIntegral = MAX_PRECISION;
	}
	if (precision > MAX_PRECISION) {
		precision = MAX_PRECISION;
	}
	if (precision <= 0) {
		flagZeroPadding = sl_false;
	}

	if (value == 0) {
		sl_uint32 pos = 0;
		if (flagLeadingSpacePositive) {
			buf[pos++] = ' ';
		}
		if (flagSignPositive) {
			buf[pos++] = '+';
		}
		for (sl_int32 i = 0; i < minWidthIntegral; i++) {
			if (chGroup) {
				if (i > 0) {
					if ((minWidthIntegral - i) % 3 == 0) {
						buf[pos++] = chGroup;
					}
				}
			}
			buf[pos++] = '0';
		}
		if (precision != 0) {
			buf[pos++] = '.';
			if (flagZeroPadding) {
				for (sl_int32 k = 0; k < precision; k++) {
					buf[pos++] = '0';
				}
			} else {
				buf[pos++] = '0';
			}
		}
		return ST(buf, pos);
	}
	
	CT* str = buf;
	CT* str_last = buf + MAX_NUMBER_STR_LEN - MAX_PRECISION;
	
	sl_int32 flagMinus;
	if (value < 0) {
		flagMinus = 1;
		value = -value;
		if (flagEncloseNagtive) {
			*(str++) = '(';
		}
		*(str++) = '-';
	} else {
		flagMinus = 0;
		if (flagLeadingSpacePositive) {
			*(str++) = ' ';
		}
		if (flagSignPositive) {
			*(str++) = '+';
		}
	}
	
	sl_int32 nExp;
	sl_int32 nInt;
	if (chConv == 'f') {
		nInt = (sl_int32)(Math::log10(value));
		nExp = 0;
	} else if (chConv == 'e' || chConv == 'E') {
		nInt = minWidthIntegral - 1;
		nExp = (sl_int32)(Math::log10(value));
	} else {
		nInt = (sl_int32)(Math::log10(value));
		nExp = 0;
		if (nInt >= 15) { // use exp
			nExp = nInt;
			nInt = 0;
		}
		if (nInt < -15) { // use exp
			nExp = nInt - 1;
			nInt = 0;
		}
	}
	
	if (nExp != 0) {
		value = value / Math::pow((FT)10, (FT)(nExp));
	}
	
	FT min_value;
	if (precision < 0) {
		if (sizeof(FT) == 4) {
			precision = 5 - nInt;
		} else {
			precision = 15 - nInt;
		}
		if (precision < 0) {
			precision = 0;
		}
		if (precision > 50) {
			precision = 50;
		}
	}
	min_value = Math::pow((FT)10, (FT)(-precision));
	value += min_value / 3;
	if (flagZeroPadding) {
		min_value = 0;
	}

	if (nInt < minWidthIntegral - 1) {
		nInt = minWidthIntegral - 1;
	}
	FT weight = 1;
	if (nInt != 0) {
		weight = Math::pow((FT)10, (FT)nInt);
	}
	while (str < str_last && nInt >= -precision && (nInt >= 0 || value >= min_value)) {
		if (nInt == -1) {
			if (value >= min_value) {
				*(str++) = '.';
			} else {
				break;
			}
		}
		if (weight > 0 && !(Math::isInfinite(weight))) {
			sl_int32 digit = (sl_int32)(value / weight);
			value -= (digit * weight);
			if (digit >= 0 && digit <= 9) {
				*(str++) = (CT)('0' + digit);
			} else {
				*(str++) = '#';
			}
		}
		if (chGroup) {
			if (nInt > 0 && nInt % 3 == 0) {
				*(str++) = chGroup;
			}
		}
		nInt--;
		weight /= 10;
	}
	if (precision > 0) {
		if (nInt >= -1) {
			*(str++) = '.';
			*(str++) = '0';
		}
	}
	
	if (nExp) {
		if (chConv == 'E' || chConv == 'G') {
			*(str++) = 'E';
		} else {
			*(str++) = 'e';
		}
		if (nExp > 0) {
			*(str++) = '+';
		} else {
			*(str++) = '-';
			nExp = -nExp;
		}
		CT* t1 = str;
		while (nExp > 0 && str < str_last) {
			*(str++) = (CT)('0' + (nExp % 10));
			nExp /= 10;
		}
		CT* t2 = str - 1;
		while (t1 < t2) {
			CT chTemp = *t1;
			*t1 = *t2;
			*t2 = chTemp;
			t1++;
			t2--;
		}
	}

	if (flagMinus) {
		if (flagEncloseNagtive) {
			*(str++) = ')';
		}
	}
	
	return ST(buf, str - buf);
}

String8 String8::fromDouble(double value, sl_int32 precision, sl_bool flagZeroPadding, sl_uint32 minWidthIntegral) {
	return _String_fromFloat<double, String8, sl_char8>(value, precision, flagZeroPadding, minWidthIntegral);
}

String8 String8::fromFloat(float value, sl_int32 precision, sl_bool flagZeroPadding, sl_uint32 minWidthIntegral)
{
	return _String_fromFloat<float, String8, sl_char8>(value, precision, flagZeroPadding, minWidthIntegral);
}

String16 String16::fromDouble(double value, sl_int32 precision, sl_bool flagZeroPadding, sl_uint32 minWidthIntegral) {
	return _String_fromFloat<double, String16, sl_char16>(value, precision, flagZeroPadding, minWidthIntegral);
}

String16 String16::fromFloat(float value, sl_int32 precision, sl_bool flagZeroPadding, sl_uint32 minWidthIntegral)
{
	return _String_fromFloat<float, String16, sl_char16>(value, precision, flagZeroPadding, minWidthIntegral);
}

String8 String8::fromPointerValue(const void* pointer)
{
#ifdef SLIB_ARCH_IS_64BIT
	return fromUint64((sl_uint64)(pointer), 16, 16, sl_true);
#else
	return fromUint32((sl_uint32)(pointer), 16, 8, sl_true);
#endif
}

String16 String16::fromPointerValue(const void* pointer)
{
#ifdef SLIB_ARCH_IS_64BIT
	return fromUint64((sl_uint64)(pointer), 16, 16, sl_true);
#else
	return fromUint32((sl_uint32)(pointer), 16, 8, sl_true);
#endif
}

String8 String8::fromBoolean(sl_bool value)
{
	if (value) {
		SLIB_STATIC_STRING8(s, "true");
		return s;
	} else {
		SLIB_STATIC_STRING8(s, "false");
		return s;
	}
}

String16 String16::fromBoolean(sl_bool value)
{
	if (value) {
		SLIB_STATIC_STRING16_BY_ARRAY(s, 't', 'r', 'u', 'e');
		return s;
	} else {
		SLIB_STATIC_STRING16_BY_ARRAY(s, 'f', 'a', 'l', 's', 'e');
		return s;
	}
}

template <class ST, class CT>
SLIB_INLINE ST _String_makeHexString(const void* buf, sl_size size)
{
	if (!buf || size <= 0) {
		return ST::null();
	}
	ST str = ST::allocate(size * 2);
	if (str.isEmpty()) {
		return str;
	}
	CT* sz = (CT*)(str.getData());
	for (sl_size i = 0; i < size; i++) {
		sl_uint8 v = ((sl_uint8*)(buf))[i];
		sz[i << 1] = _string_conv_radix_pattern_lower[v >> 4];
		sz[(i << 1) + 1] = _string_conv_radix_pattern_lower[v & 15];
	}
	return str;
}

String8 String8::makeHexString(const void* buf, sl_size size)
{
	return _String_makeHexString<String8, sl_char8>(buf, size);
}

String16 String16::makeHexString(const void* buf, sl_size size)
{
	return _String_makeHexString<String16, sl_char16>(buf, size);
}

String8 String8::makeHexString(const Memory& mem)
{
	return makeHexString(mem.getData(), mem.getSize());
}

String16 String16::makeHexString(const Memory& mem)
{
	return makeHexString(mem.getData(), mem.getSize());
}

/*
 
	String Formatting is similar with Java Formatter
 
 https://docs.oracle.com/javase/7/docs/api/java/util/Formatter.html
 
	%[argument_index$][flags][width][.precision]conversion
 
 */

template <class ST, class CT, class BT>
SLIB_INLINE ST _String_format(const CT* format, sl_size len, const Variant* params, sl_size _nParams)
{
	if (len == 0) {
		return ST::getEmpty();
	}
	sl_uint32 nParams = (sl_uint32)_nParams;
	if (nParams == 0) {
		return format;
	}
	BT sb;
	sl_size pos = 0;
	sl_size posText = 0;
	sl_uint32 indexArgLast = 0;
	sl_uint32 indexArgAuto = 0;
	while (pos <= len) {
		CT ch;
		if (pos < len) {
			ch = format[pos];
		} else {
			ch = 0;
		}
		if (ch == '%' || ch == 0) {
			sb.addStatic(format + posText, pos - posText);
			posText = pos;
			pos++;
			if (pos >= len) {
				break;
			}
			if (ch == '%') {
				do {
					ch = format[pos];
					if (ch == '%') {
						CT t = '%';
						sb.addStatic(&t, 1);
						pos++;
						posText = pos;
						break;
					} else if (ch == 'n') {
						CT t[2] = {'\r', '\n'};
						sb.addStatic(t, 2);
						pos++;
						posText = pos;
						break;
					}
					// Argument Index
					sl_uint32 indexArg;
					if (ch == '<') {
						indexArg = indexArgLast;
						pos++;
					} else {
						sl_uint32 iv;
						sl_reg iRet = ST::parseUint32(10, &iv, format, pos, len);
						if (iRet == SLIB_PARSE_ERROR) {
							indexArg = indexArgAuto;
							indexArgAuto++;
						} else {
							if ((sl_uint32)iRet >= len) {
								break;
							}
							if (format[iRet] == '$') {
								if (iv > 0) {
									iv--;
								}
								indexArg = iv;
								pos = iRet + 1;
							} else {
								indexArg = indexArgAuto;
								indexArgAuto++;
							}
						}
					}
					if (indexArg >= nParams) {
						indexArg = nParams - 1;
					}
					indexArgLast = indexArg;
					if (pos >= len) {
						break;
					}
					
					// Flags
					sl_bool flagAlignLeft = sl_false; // '-'
					sl_bool flagSignPositive = sl_false; // '+'
					sl_bool flagLeadingSpacePositive = sl_false; // ' '
					sl_bool flagZeroPadded = sl_false; // '0'
					sl_bool flagGroupingDigits = sl_false; // ','
					sl_bool flagEncloseNegative = sl_false; // '('
					do {
						ch = format[pos];
						if (ch == '-') {
							flagAlignLeft = sl_true;
						} else if (ch == '+') {
							flagSignPositive = sl_true;
						} else if (ch == ' ') {
							flagLeadingSpacePositive = sl_true;
						} else if (ch == '0') {
							flagZeroPadded = sl_true;
						} else if (ch == ',') {
							flagGroupingDigits = sl_true;
						} else if (ch == '(') {
							flagEncloseNegative = sl_true;
						} else {
							break;
						}
						pos++;
					} while (pos < len);
					if (pos >= len) {
						break;
					}
					
					// Min-Width
					sl_uint32 minWidth = 0;
					sl_reg iRet = ST::parseUint32(10, &minWidth, format, pos, len);
					if (iRet != SLIB_PARSE_ERROR) {
						pos = iRet;
						if (pos >= len) {
							break;
						}
					}
					
					// Precision
					sl_uint32 precision = 0;
					sl_bool flagUsePrecision = sl_false;
					if (format[pos] == '.') {
						pos++;
						if (pos >= len) {
							break;
						}
						flagUsePrecision = sl_true;
						iRet = ST::parseUint32(10, &precision, format, pos, len);
						if (iRet != SLIB_PARSE_ERROR) {
							pos = iRet;
							if (pos >= len) {
								break;
							}
						}
					}
					
					// Conversion
					ch = format[pos];
					pos++;
					
					const Variant& arg = params[indexArg];
					
					sl_size lenContent = 0;
					sl_bool flagError = sl_false;
					
					if (arg.isTime()) {
						ST content;
						Time time = arg.getTime();
						switch (ch) {
							case 'y':
								if (flagZeroPadded) {
									if (flagZeroPadded) {
										if (minWidth < 4) {
											minWidth = 4;
										}
									}
									content = ST::fromInt32(time.getYear(), 10, minWidth);
								} else {
									content = ST::fromInt32(time.getYear());
								}
								break;
							case 'm':
								if (flagZeroPadded) {
									if (flagZeroPadded) {
										if (minWidth < 2) {
											minWidth = 2;
										}
									}
									content = ST::fromInt32(time.getMonth(), 10, minWidth);
								} else {
									content = ST::fromInt32(time.getMonth());
								}
								break;
							case 'd':
								if (flagZeroPadded) {
									if (flagZeroPadded) {
										if (minWidth < 2) {
											minWidth = 2;
										}
									}
									content = ST::fromInt32(time.getDay(), 10, minWidth);
								} else {
									content = ST::fromInt32(time.getDay());
								}
								break;
							case 'w':
								content = time.getWeekday(sl_true);
								break;
							case 'W':
								content = time.getWeekday(sl_false);
								break;
							case 'H':
								if (flagZeroPadded) {
									if (flagZeroPadded) {
										if (minWidth < 2) {
											minWidth = 2;
										}
									}
									content = ST::fromInt32(time.getHour(), 10, minWidth);
								} else {
									content = ST::fromInt32(time.getHour());
								}
								break;
							case 'M':
								if (flagZeroPadded) {
									if (flagZeroPadded) {
										if (minWidth < 2) {
											minWidth = 2;
										}
									}
									content = ST::fromInt32(time.getMinute(), 10, minWidth);
								} else {
									content = ST::fromInt32(time.getMinute());
								}
								break;
							case 'S':
								if (flagZeroPadded) {
									if (flagZeroPadded) {
										if (minWidth < 2) {
											minWidth = 2;
										}
									}
									content = ST::fromInt32(time.getSecond(), 10, minWidth);
								} else {
									content = ST::fromInt32(time.getSecond());
								}
								break;
							case 'l':
								if (flagZeroPadded) {
									content = ST::fromInt32(time.getSecond(), 10, minWidth);
								} else {
									content = ST::fromInt32(time.getMillisecond());
								}
								break;
							case 'D':
								if (sizeof(CT) == 1) {
									content = time.getDateString();
								} else {
									content = time.getDateString16();
								}
								break;
							case 'T':
								if (sizeof(CT) == 1) {
									content = time.getTimeString();
								} else {
									content = time.getTimeString16();
								}
								break;
							case 's':
								if (sizeof(CT) == 1) {
									content = time.toString();
								} else {
									content = time.toString16();
								}
								break;
							default:
								flagError = sl_true;
								break;
						}
						if (flagError) {
							break;
						}
						lenContent = content.getLength();
						if (lenContent < minWidth) {
							if (flagAlignLeft) {
								sb.add(content);
								sb.add(ST(' ', minWidth - lenContent));
							} else {
								sb.add(ST(' ', minWidth - lenContent));
								sb.add(content);
							}
						} else {
							sb.add(content);
						}
					} else {
						switch (ch) {
							case 's':
							{
								if (sizeof(CT) == 1) {
									if (arg.isSz8() ) {
										const sl_char8* sz = arg.getSz8();
										lenContent = Base::getStringLength(sz);
										if (lenContent < minWidth) {
											if (flagAlignLeft) {
												sb.addStatic((const CT*)sz, lenContent);
												sb.add(ST(' ', minWidth - lenContent));
											} else {
												sb.add(ST(' ', minWidth - lenContent));
												sb.addStatic((const CT*)sz, lenContent);
											}
										} else {
											sb.addStatic((const CT*)sz, lenContent);
										}
									} else {
										String8 str = arg.getString8();
										lenContent = str.getLength();
										if (lenContent < minWidth) {
											if (flagAlignLeft) {
												sb.add(str);
												sb.add(ST(' ', minWidth - lenContent));
											} else {
												sb.add(ST(' ', minWidth - lenContent));
												sb.add(str);
											}
										} else {
											sb.add(str);
										}
									}
								} else {
									if (arg.isSz16() ) {
										const sl_char16* sz = arg.getSz16();
										lenContent = Base::getStringLength2(sz);
										if (lenContent < minWidth) {
											if (flagAlignLeft) {
												sb.addStatic((const CT*)sz, lenContent);
												sb.add(ST(' ', minWidth - lenContent));
											} else {
												sb.add(ST(' ', minWidth - lenContent));
												sb.addStatic((const CT*)sz, lenContent);
											}
										} else {
											sb.addStatic((const CT*)sz, lenContent);
										}
									} else {
										String16 str = arg.getString16();
										lenContent = str.getLength();
										if (lenContent < minWidth) {
											if (flagAlignLeft) {
												sb.add(str);
												sb.add(ST(' ', minWidth - lenContent));
											} else {
												sb.add(ST(' ', minWidth - lenContent));
												sb.add(str);
											}
										} else {
											sb.add(str);
										}
									}
								}
								break;
							}
							case 'd':
							case 'x':
							case 'X':
							case 'o':
							{
								CT chGroup = 0;
								if (flagGroupingDigits) {
									chGroup = ',';
								}
								sl_uint32 radix = 10;
								sl_bool flagUpperCase = sl_false;
								if (ch == 'x') {
									radix = 16;
								} else if (ch == 'X') {
									radix = 16;
									flagUpperCase = sl_true;
								} else if (ch == 'o') {
									radix = 8;
								}
								sl_uint32 _minWidth = 0;
								if (flagZeroPadded) {
									_minWidth = minWidth;
								}
								ST content;
								if (arg.isUint32()) {
									content = _String_fromUint<sl_uint32, ST, CT>(arg.getUint32(), radix, _minWidth, flagUpperCase, chGroup, flagSignPositive, flagLeadingSpacePositive);
								} else if (arg.isInt32()) {
									content = _String_fromInt<sl_int32, sl_uint32, ST, CT>(arg.getInt32(), radix, _minWidth, flagUpperCase, chGroup, flagSignPositive, flagLeadingSpacePositive, flagEncloseNegative);
								} else if (arg.isUint64()) {
									content = _String_fromUint<sl_uint64, ST, CT>(arg.getUint64(), radix, _minWidth, flagUpperCase, chGroup, flagSignPositive, flagLeadingSpacePositive);
								} else {
									content = _String_fromInt<sl_int64, sl_uint64, ST, CT>(arg.getInt64(), radix, _minWidth, flagUpperCase, chGroup, flagSignPositive, flagLeadingSpacePositive, flagEncloseNegative);
								}
								lenContent = content.getLength();
								if (lenContent < minWidth) {
									if (flagAlignLeft) {
										sb.add(content);
										sb.add(ST(' ', minWidth - lenContent));
									} else {
										sb.add(ST(' ', minWidth - lenContent));
										sb.add(content);
									}
								} else {
									sb.add(content);
								}
								break;
							}
							case 'f':
							case 'e':
							case 'E':
							case 'g':
							case 'G':
							{
								CT chGroup = 0;
								if (flagGroupingDigits) {
									chGroup = ',';
								}
								sl_int32 _precision = -1;
								if (flagUsePrecision) {
									_precision = precision;
								}
								ST content;
								if (arg.isFloat()) {
									content = _String_fromFloat<float, ST, CT>(arg.getFloat(), _precision, flagZeroPadded, 1, ch, chGroup, flagSignPositive, flagLeadingSpacePositive, flagEncloseNegative);
								} else {
									content = _String_fromFloat<double, ST, CT>(arg.getDouble(), _precision, flagZeroPadded, 1, ch, chGroup, flagSignPositive, flagLeadingSpacePositive, flagEncloseNegative);
								}
								lenContent = content.getLength();
								if (lenContent < minWidth) {
									if (flagAlignLeft) {
										sb.add(content);
										sb.add(ST(' ', minWidth - lenContent));
									} else {
										sb.add(ST(' ', minWidth - lenContent));
										sb.add(content);
									}
								} else {
									sb.add(content);
								}
								break;
							}
							case 'c':
							{
								sl_char16 unicode = (sl_char16)(arg.getUint32());
								ST str(&unicode, 1);
								lenContent = str.getLength();
								if (lenContent < minWidth) {
									if (flagAlignLeft) {
										sb.add(str);
										sb.add(ST(' ', minWidth - lenContent));
									} else {
										sb.add(ST(' ', minWidth - lenContent));
										sb.add(str);
									}
								} else {
									sb.add(str);
								}
								break;
							}
							default:
								flagError = sl_true;
								break;
						}
						if (flagError) {
							break;
						}
					}
					posText = pos;
				} while (0);
			} else {
				break;
			}
		} else {
			pos++;
		}
	}
	return sb.merge();
}

String8 String8::formatv(const String8& format, const Variant *params, sl_size nParams)
{
	return _String_format<String8, sl_char8, StringBuffer8>(format.getData(), format.getLength(), params, nParams);
}

String16 String16::formatv(const String16& format, const Variant *params, sl_size nParams)
{
	return _String_format<String16, sl_char16, StringBuffer16>(format.getData(), format.getLength(), params, nParams);
}

String8 String8::formatv(const sl_char8* format, const Variant *params, sl_size nParams)
{
	return _String_format<String8, sl_char8, StringBuffer8>(format, Base::getStringLength(format), params, nParams);
}

String16 String16::formatv(const sl_char16* format, const Variant *params, sl_size nParams)
{
	return _String_format<String16, sl_char16, StringBuffer16>(format, Base::getStringLength2(format), params, nParams);
}

String8 String8::argv(const Variant* params, sl_size nParams)
{
	return formatv(*this, params, nParams);
}

String16 String16::argv(const Variant* params, sl_size nParams)
{
	return formatv(*this, params, nParams);
}

String8 SafeString8::argv(const Variant* params, sl_size nParams)
{
	return String8::formatv(*this, params, nParams);
}

String16 SafeString16::argv(const Variant* params, sl_size nParams)
{
	return String16::formatv(*this, params, nParams);
}

String8 String8::format(const String8& szFormat, const Variant& param)
{
	return formatv(szFormat, &param, 1);
}

String16 String16::format(const String16& szFormat, const Variant& param)
{
	return formatv(szFormat, &param, 1);
}

String8 String8::format(const sl_char8* szFormat, const Variant& param)
{
	return formatv(szFormat, &param, 1);
}

String16 String16::format(const sl_char16* szFormat, const Variant& param)
{
	return formatv(szFormat, &param, 1);
}

String8 String8::arg(const Variant& param)
{
	return formatv(*this, &param, 1);
}

String16 String16::arg(const Variant& param)
{
	return formatv(*this, &param, 1);
}

String8 SafeString8::arg(const Variant& param)
{
	return String8::formatv(*this, &param, 1);
}

String16 SafeString16::arg(const Variant& param)
{
	return String16::formatv(*this, &param, 1);
}

#define FORMAT_BEGIN_VARS_LIST(n) char _vars[sizeof(Variant)*n]; Variant* vars=(Variant*)((void*)_vars); char* pvar = _vars;
#define FORMAT_ADD_VAR_TO_LIST(s) { char* sv = (char*)((void*)&s); for (int iv = 0; iv < sizeof(Variant); iv++) { *(pvar++) = *(sv++); } }

#define FORMAT_DEFINE_FUNCTIONS(nParams) \
String8 String8::format(const String8& szFormat, _SLIB_STRING_DECLARE_FORMAT_FUNCTIONS_PARAMS##nParams) \
{ \
	FORMAT_BEGIN_VARS_LIST(nParams) \
	FORMAT_PARAMS_LIST_##nParams \
	return formatv(szFormat, vars, nParams); \
} \
String16 String16::format(const String16& szFormat, _SLIB_STRING_DECLARE_FORMAT_FUNCTIONS_PARAMS##nParams) \
{ \
	FORMAT_BEGIN_VARS_LIST(nParams) \
	FORMAT_PARAMS_LIST_##nParams \
	return formatv(szFormat, vars, nParams); \
} \
String8 String8::format(const sl_char8* szFormat, _SLIB_STRING_DECLARE_FORMAT_FUNCTIONS_PARAMS##nParams) \
{ \
	FORMAT_BEGIN_VARS_LIST(nParams) \
	FORMAT_PARAMS_LIST_##nParams \
	return formatv(szFormat, vars, nParams); \
} \
String16 String16::format(const sl_char16* szFormat, _SLIB_STRING_DECLARE_FORMAT_FUNCTIONS_PARAMS##nParams) \
{ \
	FORMAT_BEGIN_VARS_LIST(nParams) \
	FORMAT_PARAMS_LIST_##nParams \
	return formatv(szFormat, vars, nParams); \
} \
String8 String8::arg(_SLIB_STRING_DECLARE_FORMAT_FUNCTIONS_PARAMS##nParams) \
{ \
	FORMAT_BEGIN_VARS_LIST(nParams) \
	FORMAT_PARAMS_LIST_##nParams \
	return formatv(*this, vars, nParams); \
} \
String16 String16::arg(_SLIB_STRING_DECLARE_FORMAT_FUNCTIONS_PARAMS##nParams) \
{ \
	FORMAT_BEGIN_VARS_LIST(nParams) \
	FORMAT_PARAMS_LIST_##nParams \
	return formatv(*this, vars, nParams); \
} \
String8 SafeString8::arg(_SLIB_STRING_DECLARE_FORMAT_FUNCTIONS_PARAMS##nParams) \
{ \
	FORMAT_BEGIN_VARS_LIST(nParams) \
	FORMAT_PARAMS_LIST_##nParams \
	return String8::formatv(*this, vars, nParams); \
} \
String16 SafeString16::arg(_SLIB_STRING_DECLARE_FORMAT_FUNCTIONS_PARAMS##nParams) \
{ \
	FORMAT_BEGIN_VARS_LIST(nParams) \
	FORMAT_PARAMS_LIST_##nParams \
	return String16::formatv(*this, vars, nParams); \
}


#define FORMAT_PARAMS_LIST_2 FORMAT_ADD_VAR_TO_LIST(param1) FORMAT_ADD_VAR_TO_LIST(param2)
#define FORMAT_PARAMS_LIST_3 FORMAT_PARAMS_LIST_2 FORMAT_ADD_VAR_TO_LIST(param3)
#define FORMAT_PARAMS_LIST_4 FORMAT_PARAMS_LIST_3 FORMAT_ADD_VAR_TO_LIST(param4)
#define FORMAT_PARAMS_LIST_5 FORMAT_PARAMS_LIST_4 FORMAT_ADD_VAR_TO_LIST(param5)
#define FORMAT_PARAMS_LIST_6 FORMAT_PARAMS_LIST_5 FORMAT_ADD_VAR_TO_LIST(param6)
#define FORMAT_PARAMS_LIST_7 FORMAT_PARAMS_LIST_6 FORMAT_ADD_VAR_TO_LIST(param7)
#define FORMAT_PARAMS_LIST_8 FORMAT_PARAMS_LIST_7 FORMAT_ADD_VAR_TO_LIST(param8)
#define FORMAT_PARAMS_LIST_9 FORMAT_PARAMS_LIST_8 FORMAT_ADD_VAR_TO_LIST(param9)
#define FORMAT_PARAMS_LIST_10 FORMAT_PARAMS_LIST_9 FORMAT_ADD_VAR_TO_LIST(param10)
#define FORMAT_PARAMS_LIST_11 FORMAT_PARAMS_LIST_10 FORMAT_ADD_VAR_TO_LIST(param11)
#define FORMAT_PARAMS_LIST_12 FORMAT_PARAMS_LIST_11 FORMAT_ADD_VAR_TO_LIST(param12)
#define FORMAT_PARAMS_LIST_13 FORMAT_PARAMS_LIST_12 FORMAT_ADD_VAR_TO_LIST(param13)
#define FORMAT_PARAMS_LIST_14 FORMAT_PARAMS_LIST_13 FORMAT_ADD_VAR_TO_LIST(param14)
#define FORMAT_PARAMS_LIST_15 FORMAT_PARAMS_LIST_14 FORMAT_ADD_VAR_TO_LIST(param15)
#define FORMAT_PARAMS_LIST_16 FORMAT_PARAMS_LIST_15 FORMAT_ADD_VAR_TO_LIST(param16)
#define FORMAT_PARAMS_LIST_17 FORMAT_PARAMS_LIST_16 FORMAT_ADD_VAR_TO_LIST(param17)
#define FORMAT_PARAMS_LIST_18 FORMAT_PARAMS_LIST_17 FORMAT_ADD_VAR_TO_LIST(param18)
#define FORMAT_PARAMS_LIST_19 FORMAT_PARAMS_LIST_18 FORMAT_ADD_VAR_TO_LIST(param19)
#define FORMAT_PARAMS_LIST_20 FORMAT_PARAMS_LIST_19 FORMAT_ADD_VAR_TO_LIST(param20)
#define FORMAT_PARAMS_LIST_21 FORMAT_PARAMS_LIST_20 FORMAT_ADD_VAR_TO_LIST(param21)
#define FORMAT_PARAMS_LIST_22 FORMAT_PARAMS_LIST_21 FORMAT_ADD_VAR_TO_LIST(param22)
#define FORMAT_PARAMS_LIST_23 FORMAT_PARAMS_LIST_22 FORMAT_ADD_VAR_TO_LIST(param23)
#define FORMAT_PARAMS_LIST_24 FORMAT_PARAMS_LIST_23 FORMAT_ADD_VAR_TO_LIST(param24)
#define FORMAT_PARAMS_LIST_25 FORMAT_PARAMS_LIST_24 FORMAT_ADD_VAR_TO_LIST(param25)
#define FORMAT_PARAMS_LIST_26 FORMAT_PARAMS_LIST_25 FORMAT_ADD_VAR_TO_LIST(param26)
#define FORMAT_PARAMS_LIST_27 FORMAT_PARAMS_LIST_26 FORMAT_ADD_VAR_TO_LIST(param27)
#define FORMAT_PARAMS_LIST_28 FORMAT_PARAMS_LIST_27 FORMAT_ADD_VAR_TO_LIST(param28)
#define FORMAT_PARAMS_LIST_29 FORMAT_PARAMS_LIST_28 FORMAT_ADD_VAR_TO_LIST(param29)
#define FORMAT_PARAMS_LIST_30 FORMAT_PARAMS_LIST_29 FORMAT_ADD_VAR_TO_LIST(param30)

FORMAT_DEFINE_FUNCTIONS(2)
FORMAT_DEFINE_FUNCTIONS(3)
FORMAT_DEFINE_FUNCTIONS(4)
FORMAT_DEFINE_FUNCTIONS(5)
FORMAT_DEFINE_FUNCTIONS(6)
FORMAT_DEFINE_FUNCTIONS(7)
FORMAT_DEFINE_FUNCTIONS(8)
FORMAT_DEFINE_FUNCTIONS(9)
FORMAT_DEFINE_FUNCTIONS(10)
FORMAT_DEFINE_FUNCTIONS(11)
FORMAT_DEFINE_FUNCTIONS(12)
FORMAT_DEFINE_FUNCTIONS(13)
FORMAT_DEFINE_FUNCTIONS(14)
FORMAT_DEFINE_FUNCTIONS(15)
FORMAT_DEFINE_FUNCTIONS(16)
FORMAT_DEFINE_FUNCTIONS(17)
FORMAT_DEFINE_FUNCTIONS(18)
FORMAT_DEFINE_FUNCTIONS(19)
FORMAT_DEFINE_FUNCTIONS(20)
FORMAT_DEFINE_FUNCTIONS(21)
FORMAT_DEFINE_FUNCTIONS(22)
FORMAT_DEFINE_FUNCTIONS(23)
FORMAT_DEFINE_FUNCTIONS(24)
FORMAT_DEFINE_FUNCTIONS(25)
FORMAT_DEFINE_FUNCTIONS(26)
FORMAT_DEFINE_FUNCTIONS(27)
FORMAT_DEFINE_FUNCTIONS(28)
FORMAT_DEFINE_FUNCTIONS(29)
FORMAT_DEFINE_FUNCTIONS(30)

template <>
int Compare<String8>::compare(const String8& a, const String8& b)
{
	return a.compare(b);
}

template <>
int Compare<String16>::compare(const String16& a, const String16& b)
{
	return a.compare(b);
}

template <>
int Compare<SafeString8>::compare(const SafeString8& a, const SafeString8& b)
{
	return a.compare(b);
}

template <>
int Compare<SafeString16>::compare(const SafeString16& a, const SafeString16& b)
{
	return a.compare(b);
}


template <>
sl_bool Compare<String8>::equals(const String8& a, const String8& b)
{
	return a.equals(b);
}

template <>
sl_bool Compare<String16>::equals(const String16& a, const String16& b)
{
	return a.equals(b);
}

template <>
sl_bool Compare<SafeString8>::equals(const SafeString8& a, const SafeString8& b)
{
	return a.equals(b);
}

template <>
sl_bool Compare<SafeString16>::equals(const SafeString16& a, const SafeString16& b)
{
	return a.equals(b);
}


template <>
sl_uint32 Hash<String8>::hash(const String8& v)
{
	return v.getHashCode();
}
template <>
sl_uint32 Hash<String16>::hash(const String16& v)
{
	return v.getHashCode();
}

template <>
sl_uint32 Hash<SafeString8>::hash(const SafeString8& v)
{
	return v.getHashCode();
}

template <>
sl_uint32 Hash<SafeString16>::hash(const SafeString16& v)
{
	return v.getHashCode();
}


/**********************************************************
 String Buffer
 **********************************************************/

StringBuffer8::StringBuffer8()
{
	m_len = 0;
}

StringBuffer16::StringBuffer16()
{
	m_len = 0;
}

sl_size StringBuffer8::getLength() const
{
	return m_len;
}

sl_size StringBuffer16::getLength() const
{
	return m_len;
}

sl_bool StringBuffer8::add(const String8& str)
{
	sl_size len = str.getLength();
	if (len == 0) {
		return sl_true;
	}
	ObjectLocker lock(this);
	StringData data;
	data.sz8 = str.getData();
	data.len = len;
	data.str8 = str;
	if (m_queue.push(data)) {
		m_len += len;
		return sl_true;
	}
	return sl_false;
}

sl_bool StringBuffer16::add(const String16& str)
{
	sl_size len = str.getLength();
	if (len == 0) {
		return sl_true;
	}
	ObjectLocker lock(this);
	StringData data;
	data.sz16 = str.getData();
	data.len = len;
	data.str16 = str;
	if (m_queue.push(data)) {
		m_len += len;
		return sl_true;
	}
	return sl_false;
}

sl_bool StringBuffer8::add(const StringData& data)
{
	sl_size len = data.len;
	if (len == 0) {
		return sl_true;
	}
	if (data.sz8) {
		ObjectLocker lock(this);
		if (m_queue.push(data)) {
			m_len += len;
			return sl_true;
		}
	}
	return sl_false;
}

sl_bool StringBuffer16::add(const StringData& data)
{
	sl_size len = data.len;
	if (len == 0) {
		return sl_true;
	}
	if (data.sz16) {
		ObjectLocker lock(this);
		if (m_queue.push(data)) {
			m_len += len;
			return sl_true;
		}
	}
	return sl_false;
}

sl_bool StringBuffer8::addStatic(const sl_char8* buf, sl_size length)
{
	StringData data;
	data.sz8 = buf;
	data.len = length;
	return add(data);
}

sl_bool StringBuffer16::addStatic(const sl_char16* buf, sl_size length)
{
	StringData data;
	data.sz16 = buf;
	data.len = length;
	return add(data);
}

void StringBuffer8::link(StringBuffer8& buf)
{
	ObjectLocker lock(this, &buf);
	m_len += buf.m_len;
	buf.m_len = 0;
	m_queue.merge(&(buf.m_queue));
}

void StringBuffer16::link(StringBuffer16& buf)
{
	ObjectLocker lock(this, &buf);
	m_len += buf.m_len;
	buf.m_len = 0;
	m_queue.merge(&(buf.m_queue));
}

void StringBuffer8::clear()
{
	ObjectLocker lock(this);
	m_queue.removeAll();
	m_len = 0;
}

void StringBuffer16::clear()
{
	ObjectLocker lock(this);
	m_queue.removeAll();
	m_len = 0;
}

String8 StringBuffer8::merge() const
{
	ObjectLocker lock(this);
	if (m_queue.getCount() == 0) {
		return String8::getEmpty();
	}
	Link<StringData>* begin = m_queue.getBegin();
	sl_size total = m_len;
	String8 ret = String8::allocate(total);
	if (ret.isNotEmpty()) {
		sl_char8* buf = (sl_char8*)(ret.getData());
		sl_size offset = 0;
		Link<StringData>* item = begin;
		while (item) {
			StringData& s = item->value;
			sl_size t = s.len;
			if (offset + t > total) {
				Base::copyMemory(buf + offset, s.sz8, total - offset);
				return ret;
			}
			Base::copyMemory(buf + offset, s.sz8, t);
			offset += t;
			item = item->next;
		}
	}
	return ret;
}

String16 StringBuffer16::merge() const
{
	ObjectLocker lock(this);
	if (m_queue.getCount() == 0) {
		return String16::getEmpty();
	}
	Link<StringData>* begin = m_queue.getBegin();
	sl_size total = m_len;
	String16 ret = String16::allocate(total);
	if (ret.isNotEmpty()) {
		sl_char16* buf = (sl_char16*)(ret.getData());
		sl_size offset = 0;
		Link<StringData>* item = begin;
		while (item) {
			StringData& s = item->value;
			sl_size t = s.len;
			if (offset + t > total) {
				Base::copyMemory(buf + offset, s.sz16, (total - offset) * 2);
				return ret;
			}
			Base::copyMemory(buf + offset, s.sz16, t * 2);
			offset += t;
			item = item->next;
		}
	}
	return ret;
}

Memory StringBuffer8::mergeToMemory() const
{
	ObjectLocker lock(this);
	if (m_queue.getCount() == 0) {
		return Memory::null();
	}
	Link<StringData>* begin = m_queue.getBegin();
	sl_size total = m_len;
	Memory ret = Memory::create(total);
	if (ret.isNotEmpty()) {
		sl_char8* buf = (sl_char8*)(ret.getData());
		sl_size offset = 0;
		Link<StringData>* item = begin;
		while (item) {
			StringData& s = item->value;
			sl_size t = s.len;
			Base::copyMemory(buf + offset, s.sz8, t);
			offset += t;
			item = item->next;
		}
	}
	return ret;
}

Memory StringBuffer16::mergeToMemory() const
{
	ObjectLocker lock(this);
	if (m_queue.getCount() == 0) {
		return Memory::null();
	}
	Link<StringData>* begin = m_queue.getBegin();
	sl_size total = m_len;
	Memory ret = Memory::create(total * 2);
	if (ret.isNotEmpty()) {
		sl_char16* buf = (sl_char16*)(ret.getData());
		sl_size offset = 0;
		Link<StringData>* item = begin;
		while (item) {
			StringData& s = item->value;
			sl_size t = s.len;
			Base::copyMemory(buf + offset, s.sz16, t * 2);
			offset += t;
			item = item->next;
		}
	}
	return ret;
}

SLIB_NAMESPACE_END