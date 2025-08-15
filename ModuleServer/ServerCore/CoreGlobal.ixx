export module CoreGlobal;

import CorePch.stdx;
import Types;
import ThreadManager;

export
{
	extern class ThreadManager* GThreadManager = nullptr;

	class CoreGlobal
	{
	public:
		CoreGlobal();
		~CoreGlobal();
	};
}