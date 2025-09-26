export module CoreGlobal;

import CorePch.stdx;
import Types;
import ThreadManager;
import DeadLockProfiler;

export
{
	class CoreGlobal
	{
	public:
		CoreGlobal();
		~CoreGlobal();
	};

	extern class ThreadManager* GThreadManager;
	extern class DeadLockProfiler* GDeadLockProfiler;
	extern class CoreGlobal GCoreGlobal;
}