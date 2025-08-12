export module ConcurrentBlockingQueue;

import ConcurrentQueue;

// 빌드 과정 문제 해결
import <atomic>;
import <memory>;
import <stop_token>;
import <utility>;
using namespace std;

export
{
	template<typename T>
	class BlockingLFQueue
	{
	public:
		void Push(T v)
		{
			_q.Push(std::move(v));
			_count.fetch_add(1);
			_count.notify_one();
		}

		shared_ptr<T> PopBlocking(std::stop_token st)
		{
			// stop 요청 시 대기 중인 스레드를 깨우기 위한 콜백
			std::stop_callback on_stop(st, [this] { _count.notify_all(); });

			while (true)
			{
				// 1) 아이템이 없으면 값 기반 대기
				size_t expected = _count.load();
				while (expected == 0)
				{
					if (st.stop_requested())
						return {};

					_count.wait(expected);
					expected = _count.load();
				}

				if (auto sp = _q.TryPop())
				{
					_count.fetch_sub(1);
					return sp;
				}

				if (st.stop_requested())
					return {};
			}
		}

		void WakeAll() { _count.notify_all(); }

		bool empty() const noexcept { return _count.load() == 0; }


	private:
		LockFreeQueue<T> _q;
		atomic<size_t> _count{ 0 };
	};
}