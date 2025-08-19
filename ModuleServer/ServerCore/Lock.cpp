module;
#include "CoreMacro.h"

module Lock;
import CoreTLS;

// 아무도 소유 및 공유하고 있지 않을 때, 경합해서 소유권을 얻는다.
void Lock::WriteLock()
{
	// 동일한 쓰레드가 소유하고 있다면 무조건 성공
	const uint32 flag = _lockFlag.load(memory_order::relaxed);
	const uint32 lockThreadId = (flag & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId)
	{
		++_writeCount;
		return;
	}

	const auto deadline = chrono::steady_clock::now() + chrono::milliseconds(ACQUIRE_TIMEOUT_TICK);

	const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK);
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = EMPTY_FLAG;
			if (_lockFlag.compare_exchange_strong(OUT expected, desired, memory_order::acquire, memory_order::relaxed))
			{
				++_writeCount;
				return;
			}
		}

		if (chrono::steady_clock::now() >= deadline)
			CRASH("LOCK_TIMEOUT");

		// 값이 바뀔 때까지 대기
		const uint32 observed = _lockFlag.load(memory_order::relaxed);
		_lockFlag.wait(observed, memory_order::relaxed);
	}
}

void Lock::WriteUnlock()
{
	// ReadLock 다 풀기 전에는 WriteUnlock 불가능
	if ((_lockFlag.load(memory_order::relaxed) & READ_COUNT_MASK) != 0)
		CRASH("INVALID_UNLOCK_ORDER");

	const int32 lockCount = --_writeCount;
	if (lockCount == 0)
	{
		_lockFlag.store(EMPTY_FLAG, memory_order::release);
		_lockFlag.notify_all();
	}
		
}

// 아무도 소유하고 있지 않을 때 경합해서 공유 카운트를 올린다.
void Lock::ReadLock()
{
	// 동일한 쓰레드가 소유하고 있다면 무조건 성공
	const uint32 flag = _lockFlag.load(memory_order::relaxed);
	const uint32 lockThreadId = (flag & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId)
	{
		const uint32 prev = _lockFlag.fetch_add(1, memory_order::relaxed) & READ_COUNT_MASK;
		if (prev == READ_COUNT_MASK)
			CRASH("READ_COUNT_OVERFLOW");

		return;
	}

	const auto deadline = chrono::steady_clock::now() + chrono::milliseconds(ACQUIRE_TIMEOUT_TICK);

	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = _lockFlag.load(memory_order::acquire);

			if ((expected & WRITE_THREAD_MASK) != 0)
				break;

			expected = expected & READ_COUNT_MASK;
			if (expected == READ_COUNT_MASK) [[unlikely]]
				CRASH("READ_COUNT_OVERFLOW");

			if (_lockFlag.compare_exchange_strong(OUT expected, expected + 1, memory_order::acquire, memory_order::relaxed))
			{
				return;
			}
		}

		if (chrono::steady_clock::now() >= deadline)
			CRASH("LOCK_TIMEOUT");

		// writer가 풀 때까지 대기
		const uint32 observed = _lockFlag.load(memory_order::relaxed);
		_lockFlag.wait(observed, memory_order::relaxed);
	}
}

void Lock::ReadUnlock()
{
	const uint32 prev = _lockFlag.fetch_sub(1, memory_order::release);
	if ((prev & READ_COUNT_MASK) == 0) [[unlikely]]
		CRASH("MULTIPLE_UNLOCK");

	if ((prev & READ_COUNT_MASK) == 1)
		_lockFlag.notify_all();
}