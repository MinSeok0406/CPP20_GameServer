export module DeadLockProfiler;

import CoreMacro;
import CorePch.stdx;
import Types;

using namespace std;

// 그래프 사이클을 판별(DFS 이용)
// 만약 사이클이 발생하면 데드락 상황이라 판별 (역방향 간선)
/*
 * DeadLockProfiler
 */
export
{
	class DeadLockProfiler
	{
	public:
		void PushLock(const void* name);
		void PopLock(const void* name);
		void CheckCycle();

	private:
		void Dfs(int32 here);

	private:
		unordered_map<const void*, int32>	_nameToId;
		unordered_map<int32, const void*>	_idToName;
		stack<int32>						_lockStack;
		map<int32, set<int32>>				_lockHistory;

		Mutex _lock;

	private:
		// 노드가 발견된 순서를 기록하는 배열
		vector<int32>	_discoveredOrder;

		// 노드가 발견된 순서를 카운팅
		int32			_discoveredCount = 0;

		// Dfs(i)가 종료 되었는지 여부
		vector<bool>	_finished;

		// 현재 노드의 부모를 가리킴
		vector<int32>	_parent;
	};
}