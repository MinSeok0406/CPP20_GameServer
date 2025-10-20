export module Memory;

import Types;
import Allocator;
import CoreMacro;

using std::construct_at;
using std::destroy_at;

export
{
	template<typename Type, typename... Args>
		requires std::constructible_from<Type, Args...>
	[[nodiscard("xnew exclusive")]] Type* xnew(Args&&... args)
	{
		void* memory = xalloc(sizeof(Type), alignof(Type));

		try 
		{
			return construct_at(static_cast<Type*>(memory), std::forward<Args>(args)...);
		}
		catch (...) 
		{
			BaseAllocator::Release(memory, alignof(Type));
			throw;
		}
	}

	template<typename Type>
		requires std::destructible<Type>
	void xdelete(Type* obj) noexcept
	{
		if (!obj)
		{
			return;
		}

		destroy_at(obj);
		xfree(static_cast<void*>(obj), alignof(Type));
	}
}