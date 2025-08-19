
#define OUT

/*
 *	Lock
 */

#define USE_MANY_LOCKS(count)	Lock _locks[count];
#define USE_LOCK				USE_MANY_LOCKS(1)
#define READ_LOCK_IDX(idx)		ReadLockGuard readLockGuard_##idx(_locks[idx]);
#define READ_LOCK				READ_LOCK_IDX(0)
#define WRITE_LOCK_IDX(idx)		WriteLockGuard writeLockGuard_##idx(_locks[idx]);
#define WRITE_LOCK				WRITE_LOCK_IDX(0)


/*
 *	Crash
 */

#ifndef CORE_MACRO_H
#define CORE_MACRO_H

#include <sal.h>

// __analysis_assume 가 정의되지 않은 경우 대비
#ifndef __analysis_assume
#define __analysis_assume(x) ((void)0)
#endif

#define CRASH(cause)						\
{											\
	__int32* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}

#define ASSERT_CRASH(expr)					\
{											\
	if (!(expr))							\
	{										\
		CRASH("ASSERT_CRASH");				\
		__analysis_assume(expr);			\
	}										\
}

#endif // CORE_MACRO_H