export module CoreGlobal;

import CorePch.stdx;
import Types;
import ThreadManager;
import DeadLockProfiler;

export
{
	extern class ThreadManager* GThreadManager = nullptr;
	extern class DeadLockProfiler* GDeadLockProfiler = nullptr;

	class CoreGlobal
	{
	public:
		CoreGlobal()
		{
			GThreadManager = new ThreadManager();
			GDeadLockProfiler = new DeadLockProfiler();
		}

		~CoreGlobal()
		{
			delete GThreadManager;
			delete GDeadLockProfiler;
		}
	} GCoreGlobal;
}