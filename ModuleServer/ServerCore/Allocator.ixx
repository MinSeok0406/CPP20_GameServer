export module Allocator;

import CorePch.stdx;
import Types;


export
{
	/*----------------
	  BaseAllocator
	 ----------------*/

	class BaseAllocator
	{
	public:
		// alignof(T)는 “타입 T가 최소로 요구하는 정렬 바이트 수”를 size_t로 돌려준다
		static void* Alloc(std::size_t size, std::size_t align = alignof(std::max_align_t));
		static void	 Release(void* ptr, std::size_t align = alignof(std::max_align_t)) noexcept;
	};
}