#ifndef CHECKHEADER_SLIB_CORE_SETTING
#define CHECKHEADER_SLIB_CORE_SETTING

#include "definition.h"

#include "map.h"
#include "variant.h"

SLIB_NAMESPACE_BEGIN

class SLIB_EXPORT IniSetting
{
private:
	HashMap<String, String> m_mapValues;

public:
	void initialize();
	
	sl_bool parseFromUtf8TextFile(const String& filePath);
	
	sl_bool parseFromText(const String& text);
	
	Variant getValue(const String& name);

};

class SLIB_EXPORT SettingUtil
{
public:
	static sl_bool parseUint32Range(const String& str, sl_uint32* from = sl_null, sl_uint32* to = sl_null);

};

SLIB_NAMESPACE_END

#endif