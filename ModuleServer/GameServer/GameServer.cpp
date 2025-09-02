import Pch;

// 타이밍 이슈인지 모르겠지만 제대로 작동 X
class TestLock
{
public:
	int32 TestRead()
	{
		auto wr = READ_LOCK(_locks);

		if (_queue.empty())
			return -1;

		return _queue.front();
	}

	void TestPush()
	{
		auto wr = WRITE_LOCK(_locks);

		_queue.push(rand() % 100);
	}

	void TestPop()
	{
		auto wr = WRITE_LOCK(_locks);

		if (_queue.empty() == false)
			_queue.pop();
	}

private:
	queue<int32> _queue;
	LockBox<1> _locks{};
};


TestLock testLock;

void ThreadWrite()
{
	while (true)
	{
		testLock.TestPush();
		this_thread::sleep_for(100ms);
		testLock.TestPop();
	}
}

void ThreadRead()
{
	while (true)
	{
		int32 value = testLock.TestRead();

		osyncstream scout(cout);
		scout << value << "\n";
		this_thread::sleep_for(100ms);
	}
}

int main()
{
	for (int32 i = 0; i < 2; i++)
	{
		GThreadManager->Launch(ThreadWrite);
	}

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch(ThreadRead);
	}

	GThreadManager->Join();
}