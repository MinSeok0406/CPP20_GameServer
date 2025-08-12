#include <random>
#include <windows.h>
import Pch;

import ConcurrentBlockingQueue;

int main()
{
	BlockingLFQueue<int> q;

	jthread producer([&](std::stop_token st)
		{
			std::mt19937 rng{ std::random_device{}() };
			std::uniform_int_distribution<int> dist(0, 99);

			while (!st.stop_requested())
			{
				q.Push(dist(rng));
				std::this_thread::sleep_for(10ms);
			}
		});

	jthread consumer([&](std::stop_token st)
		{
			while (true)
			{
				auto v = q.PopBlocking(st);
				if (!v)
					break;

				cout << *v << "\n";
			}
		});

	this_thread::sleep_for(3s);
	producer.request_stop();
	consumer.request_stop();

	q.WakeAll();
}