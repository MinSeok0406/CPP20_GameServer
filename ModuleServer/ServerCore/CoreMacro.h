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