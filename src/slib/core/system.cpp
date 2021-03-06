/*
 *  Copyright (c) 2008-2017 SLIBIO. All Rights Reserved.
 *
 *  This file is part of the SLib.io project.
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "../../../inc/slib/core/system.h"

#include "../../../inc/slib/core/file.h"
#include "../../../inc/slib/core/log.h"
#include "../../../inc/slib/core/list.h"
#include "../../../inc/slib/core/safe_static.h"

namespace slib
{

	String System::getApplicationDirectory()
	{
		String path = getApplicationPath();
#if !defined(SLIB_PLATFORM_IS_ANDROID)
		path = File::getParentDirectoryPath(path);
#endif
		return path;
	}

#if !defined(SLIB_PLATFORM_IS_APPLE)
	String System::getApplicationHomeDirectory()
	{
		return getApplicationDirectory();
	}

	String System::getCachesDirectory()
	{
		String dir = getApplicationDirectory() + "/caches";
		File::createDirectory(dir);
		return dir;
	}
#endif

#if defined(SLIB_PLATFORM_IS_MOBILE)
	sl_bool System::createProcess(const String& pathExecutable, const String* cmds, sl_uint32 nCmds)
	{
		LogError("System::createProcess", "Not supported");
		return sl_false;
	}

	void System::exec(const String& pathExecutable, const String* cmds, sl_uint32 nCmds)
	{
		LogError("System::exec", "Not supported");
	}

	void System::exit(int code)
	{
		LogError("System::exit", "Not supported");
	}

	void System::setCrashHandler(SIGNAL_HANDLER handler)
	{
		LogError("System::setCrashHandler", "Not supported");
	}
#endif

	void System::yield(sl_uint32 elapsed)
	{
		if (elapsed < 16) {
			return;
		}
		if (elapsed < 32) {
			System::yield();
		} else {
			System::sleep(1);
		}
	}


	SLIB_DEFINE_OBJECT(GlobalUniqueInstance, Object)
	
	GlobalUniqueInstance::GlobalUniqueInstance()
	{	
	}

	GlobalUniqueInstance::~GlobalUniqueInstance()
	{
	}

#if defined(SLIB_PLATFORM_IS_MOBILE)

	typedef CList<String> _GlobalUniqueInstanceList;

	SLIB_SAFE_STATIC_GETTER(_GlobalUniqueInstanceList, _getGlobalUniqueInstanceList)

	class _GlobalUniqueInstance : public GlobalUniqueInstance
	{
	public:
		String m_name;
		
	public:
		_GlobalUniqueInstance()
		{
		}
		
		~_GlobalUniqueInstance()
		{
			_GlobalUniqueInstanceList* list = _getGlobalUniqueInstanceList();
			if (list) {
				list->removeValue(m_name);
			}
		}
		
	};

	Ref<GlobalUniqueInstance> GlobalUniqueInstance::create(const String& _name)
	{
		String name = _name;
		if (name.isEmpty()) {
			return sl_null;
		}
		name = File::makeSafeFileName(name);
		_GlobalUniqueInstanceList* list = _getGlobalUniqueInstanceList();
		if (!list) {
			return sl_null;
		}
		if (list->indexOf(name) >= 0) {
			return sl_null;
		}
		Ref<_GlobalUniqueInstance> instance = new _GlobalUniqueInstance();
		if (instance.isNotNull()) {
			instance->m_name = name;
			list->add(name);
			return instance;
		}
		return sl_null;
	}

	sl_bool GlobalUniqueInstance::exists(const String& _name)
	{
		String name = _name;
		if (name.isEmpty()) {
			return sl_false;
		}
		name = File::makeSafeFileName(name);
		_GlobalUniqueInstanceList* list = _getGlobalUniqueInstanceList();
		if (!list) {
			return sl_false;
		}
		if (list->indexOf(name) >= 0) {
			return sl_true;
		}
		return sl_true;
	}

#endif

}
