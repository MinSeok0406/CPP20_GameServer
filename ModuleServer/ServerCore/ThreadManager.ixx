export module ThreadManager;

import CorePch.stdx;
import Types;

// ThreadManager
export
{
	class ThreadManager
	{
	public:
		ThreadManager();
		~ThreadManager();

		void	Launch(function<void(void)> callback);
		void	Join();

		static void InitTLS();
		static void DestoryTLS();

	private:
		Mutex			_lock;
		vector<jthread>	_threads;
	};
}