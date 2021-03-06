/*
 *  Copyright (c) 2008-2017 SLIBIO. All Rights Reserved.
 *
 *  This file is part of the SLib.io project.
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "../../../inc/slib/core/definition.h"

#if defined(SLIB_PLATFORM_IS_UNIX) && !defined(SLIB_PLATFORM_IS_APPLE)

#include <pthread.h>
#include <time.h>

#include "../../../inc/slib/core/thread.h"

namespace slib
{

	SLIB_THREAD Thread* _gt_threadCurrent = sl_null;
	Thread* Thread::_nativeGetCurrentThread()
	{
		return _gt_threadCurrent;
	}

	void Thread::_nativeSetCurrentThread(Thread* thread)
	{
		_gt_threadCurrent = thread;
	}

	SLIB_THREAD sl_uint64 _gt_threadUniqueId = 0;
	sl_uint64 Thread::_nativeGetCurrentThreadUniqueId()
	{
		return _gt_threadUniqueId;
	}

	void Thread::_nativeSetCurrentThreadUniqueId(sl_uint64 n)
	{
		_gt_threadUniqueId = n;
	}

	static void* _ThreadProc(void* lpParam)
	{
		Thread* pThread = (Thread*)lpParam;
		pThread->_run();
		pThread->decreaseReference();
		pthread_exit(0);
		return 0;
	}

	void Thread::_nativeStart(sl_uint32 stackSize)
	{
		pthread_attr_t attr;
		pthread_t threadId;
		int result = pthread_attr_init(&attr);
		if (result) {
			return;
		}
		result = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		result |= pthread_attr_setstacksize(&attr, stackSize);
		if (result) {
			return;
		}
		this->increaseReference();
		result = pthread_create(&threadId, &attr, &_ThreadProc, this);
		if (result == 0) {
			m_handle = (void*)(threadId);
		} else {
			this->decreaseReference();
		}
		pthread_attr_destroy(&attr);
	}

	void Thread::_nativeClose()
	{
	}

#define _UNIX_SCHED_POLICY SCHED_FIFO
	static int _thread_getUnixPriority(ThreadPriority priority)
	{
		int min = sched_get_priority_min(_UNIX_SCHED_POLICY);
		int max = sched_get_priority_max(_UNIX_SCHED_POLICY);
		if (min < 0 || max < 0) {
			return -1;
		}
		switch (priority) {
		case ThreadPriority::Lowest:
			return min;
		case ThreadPriority::BelowNormal:
			return (min * 3 + max) / 4;
		case ThreadPriority::Normal:
			return (min + max) / 2;
		case ThreadPriority::AboveNormal:
			return (min + max * 3) / 4;
		case ThreadPriority::Highest:
			return max;
		}
		return -1;
	}

	void Thread::_nativeSetPriority()
	{
		pthread_t thread = (pthread_t)m_handle;
		if (thread) {
			int p = _thread_getUnixPriority(m_priority);
			if (p >= 0) {
				sched_param param;
				param.sched_priority = p;
				pthread_setschedparam(thread, _UNIX_SCHED_POLICY, &param);
			}
		}
	}

}

#endif
