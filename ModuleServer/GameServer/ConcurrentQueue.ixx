export module ConcurrentQueue;

import Pch;

export
{
	template<typename T>
	class LockQueue
	{
	public:
		LockQueue() {}

		LockQueue(const LockQueue&) = delete;
		LockQueue& operator=(const LockQueue&) = delete;

		void Push(T value)
		{
			lock_guard<mutex> lock(_mutex);
			_queue.push(std::move(value));
			_condVar.notify_one();
		}

		bool TryPop(T& value)
		{
			lock_guard<mutex> lock(_mutex);
			if (_queue.empty())
				return false;

			value = std::move(_queue.front());
			_queue.pop();
			return true;
		}

		void WaitPop(T& value)
		{
			unique_lock<mutex> lock(_mutex);
			_condVar.wait(lock, [this] { return _queue.empty() == false; });
			value = std::move(_queue.front());
			_queue.pop();
		}

	private:
		queue<T> _queue;
		mutex _mutex;
		condition_variable _condVar;
	};

	// C++14 버전
	/*template<typename T>
	class LockFreeQueue
	{
		struct Node
		{
			shared_ptr<T> data;
			Node* next = nullptr;
		};

	public:
		LockFreeQueue() : _head(new Node), _tail(_head)
		{
			
		}

		LockFreeQueue(const LockFreeQueue&) = delete;
		LockFreeQueue& operator=(const LockFreeQueue&) = delete;

		void Push(const T& value)
		{
			shared_ptr<T> newData = make_shared<T>(value);

			Node* dummy = new Node();

			Node* oldTail = _tail;
			oldTail->data.swap(newData);
			oldTail->next = dummy;

			_tail = dummy;
		}

		shared_ptr<T> TryPop()
		{
			Node* oldHead = PopHead();
			if (oldHead == nullptr)
				return shared_ptr<T>();

			shared_ptr<T> res(oldHead->data);
			delete oldHead;
			return res;
		}

	private:
		Node* PopHead()
		{
			Node* oldHead = _head;
			if (oldHead == _tail)
			{
				return nullptr;
			}

			_head = oldHead->next;
			return oldHead;
		}

	private:
		Node* _head = nullptr;
		Node* _tail = nullptr;
	};*/

	// C++20 버전
	template<typename T>
	class LockFreeQueue
	{
		struct Node;

		struct CountedNodePtr
		{
			int32 externalCount;
			Node* ptr = nullptr;
		};

		struct NodeCounter
		{
			uint32 internalCount : 30;
			uint32 externalCountRemaining : 2;
		};

		struct Node
		{
			Node()
			{
				NodeCounter newCount;
				newCount.internalCount = 0;
				newCount.externalCountRemaining = 2;
				count.store(newCount);
				next.store(CountedNodePtr{ 0, nullptr });
			}

			void ReleaseRef()
			{
				NodeCounter oldCounter = count.load();

				while (true)
				{
					NodeCounter newCounter = oldCounter;
					newCounter.internalCount--;

					if (count.compare_exchange_strong(oldCounter, newCounter))
					{
						if (newCounter.internalCount == 0 && newCounter.externalCountRemaining == 0)
							delete this;

						break;
					}
				}
			}

			atomic<T*> data;
			atomic<NodeCounter> count;
			atomic<CountedNodePtr> next;
		};

	public:
		LockFreeQueue()
		{
			CountedNodePtr node;
			node.ptr = new Node;
			node.externalCount = 1;

			_head.store(node);
			_tail.store(node);
		}

		~LockFreeQueue()
		{
			// 단일 스레드 파괴 가정: 남은 데이터/노드 정리
			// 1) 모든 아이템 pop하여 T 해제
			while (TryPop()) {}
			// 2) 더미 포함 잔여 체인 파괴
			auto cur = _head.load().ptr;
			while (cur) {
				auto next = cur->next.load().ptr;
				if (T* p = cur->data.exchange(nullptr))
					delete p;
				delete cur;
				cur = next;
			}
		}

		LockFreeQueue(const LockFreeQueue&) = delete;
		LockFreeQueue& operator=(const LockFreeQueue&) = delete;

		void Push(const T& value)
		{
			unique_ptr<T> newData = make_unique<T>(value);

			CountedNodePtr dummy;
			dummy.ptr = new Node;
			dummy.externalCount = 1;

			CountedNodePtr oldTail = _tail.load();

			while (true)
			{
				IncreaseExternalCount(_tail, oldTail);

				T* oldData = nullptr;
				if (oldTail.ptr->data.compare_exchange_strong(oldData, newData.get()))
				{
					oldTail.ptr->next.store(dummy);
					oldTail = _tail.exchange(dummy);
					FreeExternalCount(oldTail);
					newData.release();
					break;
				}

				oldTail.ptr->ReleaseRef();
			}
		}

		[[nodiscard]] shared_ptr<T> TryPop()
		{
			CountedNodePtr oldHead = _head.load();

			while (true)
			{
				IncreaseExternalCount(_head, oldHead);

				Node* ptr = oldHead.ptr;
				if (ptr == _tail.load().ptr)
				{
					ptr->ReleaseRef();
					return shared_ptr<T>();
				}

				if (_head.compare_exchange_strong(oldHead, ptr->next))
				{
					T* res = ptr->data.exchange(nullptr);
					FreeExternalCount(oldHead);
					return shared_ptr<T>(res);
				}

				ptr->ReleaseRef();
			}
		}

	private:
		static void IncreaseExternalCount(atomic<CountedNodePtr>& counter, CountedNodePtr& oldCounter)
		{
			while (true)
			{
				CountedNodePtr newCounter = oldCounter;
				newCounter.externalCount++;

				if (counter.compare_exchange_strong(oldCounter, newCounter))
				{
					oldCounter.externalCount = newCounter.externalCount;
					break;
				}
			}
		}

		static void FreeExternalCount(CountedNodePtr& oldNodePtr)
		{
			Node* ptr = oldNodePtr.ptr;
			const int32 countIncrease = oldNodePtr.externalCount - 2;

			NodeCounter oldCounter = ptr->count.load();

			while (true)
			{
				NodeCounter newCounter = oldCounter;
				newCounter.externalCountRemaining--;
				newCounter.internalCount += countIncrease;

				if (ptr->count.compare_exchange_strong(oldCounter, newCounter))
				{
					if (newCounter.internalCount == 0 && newCounter.externalCountRemaining == 0)
						delete ptr;

					break;
				}
			}
		}

	private:
		atomic<CountedNodePtr> _head;
		atomic<CountedNodePtr> _tail;
	};
}