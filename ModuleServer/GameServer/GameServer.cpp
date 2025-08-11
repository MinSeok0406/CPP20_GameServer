#include <windows.h>
import Pch;
import ConcurrentQueue;
import ConcurrentStack;

LockQueue<int32> q;
LockFreeStack<int32> s;

void Push()
{
	while (true)
	{
		int32 value = rand() % 100;
		s.Push(value);

		this_thread::sleep_for(10ms);
	}
}

void Pop()
{
	while (true)
	{
		if (auto data = s.TryPop())
		{
			cout << *data << "\n";
		}
	}
}

int main()
{
	{
		jthread t1(Push);
		jthread t2(Pop);
	}


}