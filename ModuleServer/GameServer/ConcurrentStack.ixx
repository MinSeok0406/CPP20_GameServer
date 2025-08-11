export module ConcurrentStack;

import Pch;

export
{
	template<typename T>
	class LockStack
	{
	public:
		LockStack() { }

		LockStack(const LockStack&) = delete;
		LockStack& operator=(const LockStack&) = delete;

		void Push(T value)
		{
			lock_guard<mutex> lock(_mutex);
			_stack.push(std::move(value));
			_condVar.notify_one();
		}

		bool TryPop(T& value)
		{
			lock_guard<mutex> lock(_mutex);
			if (_stack.empty())
				return false;

			value = std::move(_stack.top());
			_stack.pop();
			return true;
		}

		void WaitPop(T& value)
		{
			unique_lock<mutex> lock(_mutex);
			_condVar.wait(lock, [this] { return _stack.empty() == false; });
			value = std::move(_stack.top());
			_stack.pop();
		}

	private:
		stack<T> _stack;
		mutex _mutex;
		condition_variable _condVar;
	};

	// C++14 버전
	/*template<typename T>
	class LockFreeStack
	{
		struct Node;

		struct CountedNodePtr
		{
			int32 externalCount = 0;
			Node* ptr = nullptr;
		};

		struct Node
		{
			Node(const T& value) : data(make_shared<T>(value))
			{

			}

			shared_ptr<T> data;					// payload는 shared_ptr로 별도 수명 보장
			atomic<int32> internalCount = 0;	// 내부 참조 카운트(음수도 잠깐 가능)
			CountedNodePtr next;				// 다음 노드(포인터+외부카운트 세트)
		};

	public:


		// 1. 새 노드를 만들고 외부카운트=1(헤드가 가리키는 참조 1개).
		// 2. 현재 헤드를 읽어 node->next로 연결.
		// 3. 헤드를 node로 CAS. 실패하면 기대값(node->next)이 최신 헤드로 갱신되니 반복.
		void Push(const T& value)
		{
			CountedNodePtr node;
			node.ptr = new Node(value);
			node.externalCount = 1;

			node.ptr->next = _head.load();
			while (_head.compare_exchange_weak(node.ptr->next, node) == false)
			{
				
			}
		}

		shared_ptr<T> TryPop()
		{
			CountedNodePtr oldHead = _head.load();
			while (true)
			{
				IncreaseHeadCount(oldHead);

				Node* ptr = oldHead.ptr;

				if (ptr == nullptr)
					return shared_ptr<T>();

				if (_head.compare_exchange_strong(oldHead, ptr->next))
				{
					shared_ptr<T> res;
					res.swap(ptr->data);

					// 외부카운트 중, head 참조(1) + 내 로컬 참조(1) = 2개는 소진됨
					const int32 countIncrease = oldHead.externalCount - 2;

					// 남은 외부카운트를 내부카운트로 ‘이관’
					if (ptr->internalCount.fetch_add(countIncrease) == -countIncrease)
						delete ptr;

					return res;
				}
				else if (ptr->internalCount.fetch_sub(1) == 1)
				{
					delete ptr;
				}
			}
		}

	private:
		void IncreaseHeadCount(CountedNodePtr& oldCounter)
		{
			while (true)
			{
				CountedNodePtr newCounter = oldCounter;
				newCounter.externalCount++;

				if (_head.compare_exchange_strong(oldCounter, newCounter))
				{
					oldCounter.externalCount = newCounter.externalCount;
					break;
				}
			}
		}

	private:
		atomic<CountedNodePtr> _head;	// 헤드 자체도 (ptr, externalCount)를 함께 가진다
	};*/

	// C++20 버전
	template<typename T>
	class LockFreeStack
	{
		struct Node
		{
			Node(const T& value) : data(value)
			{

			}

			T data;
			std::shared_ptr<Node> next;
		};

	public:


		// 1. 새 노드를 만들고 외부카운트=1(헤드가 가리키는 참조 1개).
		// 2. 현재 헤드를 읽어 node->next로 연결.
		// 3. 헤드를 node로 CAS. 실패하면 기대값(node->next)이 최신 헤드로 갱신되니 반복.
		void Push(const T& value)
		{
			auto n = std::make_shared<Node>(std::move(value));
			auto old = _head.load();
			n->next = old;

			while (_head.compare_exchange_weak(old, n))
			{
				
			}
		}

		std::optional<T> TryPop()
		{
			auto old = _head.load();
			while (old)
			{
				auto next = old->next;
				if (_head.compare_exchange_strong(old, next))
				{
					return std::optional<T>{ std::move(old->data) };
				}
			}

			return std::nullopt;
		}

	private:
		std::atomic<std::shared_ptr<Node>> _head;
	};
}