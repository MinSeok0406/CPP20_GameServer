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

	// C++14 ����
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

			shared_ptr<T> data;					// payload�� shared_ptr�� ���� ���� ����
			atomic<int32> internalCount = 0;	// ���� ���� ī��Ʈ(������ ��� ����)
			CountedNodePtr next;				// ���� ���(������+�ܺ�ī��Ʈ ��Ʈ)
		};

	public:


		// 1. �� ��带 ����� �ܺ�ī��Ʈ=1(��尡 ����Ű�� ���� 1��).
		// 2. ���� ��带 �о� node->next�� ����.
		// 3. ��带 node�� CAS. �����ϸ� ��밪(node->next)�� �ֽ� ���� ���ŵǴ� �ݺ�.
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

					// �ܺ�ī��Ʈ ��, head ����(1) + �� ���� ����(1) = 2���� ������
					const int32 countIncrease = oldHead.externalCount - 2;

					// ���� �ܺ�ī��Ʈ�� ����ī��Ʈ�� ���̰���
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
		atomic<CountedNodePtr> _head;	// ��� ��ü�� (ptr, externalCount)�� �Բ� ������
	};*/

	// C++20 ����
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


		// 1. �� ��带 ����� �ܺ�ī��Ʈ=1(��尡 ����Ű�� ���� 1��).
		// 2. ���� ��带 �о� node->next�� ����.
		// 3. ��带 node�� CAS. �����ϸ� ��밪(node->next)�� �ֽ� ���� ���ŵǴ� �ݺ�.
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