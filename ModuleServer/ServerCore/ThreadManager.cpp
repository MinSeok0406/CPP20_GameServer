module ThreadManager;

import CoreTLS;

ThreadManager::ThreadManager()
{
	InitTLS();
}

ThreadManager::~ThreadManager()
{
	Join();
}

void ThreadManager::Launch(function<void(void)> callback)
{
	LockGuard guard(_lock);

	_threads.push_back(jthread([=]()
		{
			InitTLS();
			callback();
			DestoryTLS();
		}));
}

void ThreadManager::Join()
{
	// ����������Ŭ ��Ȯ�ϰ� �����Ϸ��� join ȣ��

	_threads.clear();
}

void ThreadManager::InitTLS()
{
	static Atomic<uint32> SThreadId = 1;
	LThreadId = SThreadId.fetch_add(1);
}

void ThreadManager::DestoryTLS()
{
}