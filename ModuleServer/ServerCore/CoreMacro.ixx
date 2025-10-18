export module CoreMacro;

import CorePch.stdx;
import Lock;
import Allocator;
import <source_location>;

// CRASH
export
{
	constexpr void analysis_assume(bool) noexcept {}

    [[noreturn]] inline void crash_impl() noexcept
	{
        volatile int* p = nullptr;
        *p = 0xDEADBEEF;
        for (;;) {} // [[noreturn]] 보장
    }

    [[noreturn]] inline void CRASH(const char*  = "CRASH") noexcept
	{
        crash_impl();
    }

    inline void ASSERT_CRASH(bool expr, const char* expr_text = "ASSERT_CRASH") noexcept
	{
        if (!expr) 
        {
            (void)expr_text;      // 원하면 로깅에 사용
            analysis_assume(false);
            CRASH("ASSERT_CRASH");
        }
        else 
        {
            analysis_assume(true);
        }
    }
}

// LOCK
export
{
    // --- 객체 멤버용 Lock 컨테이너 ---
    template <std::size_t N> 
    struct LockBox 
    {
        std::array<Lock, N> locks; // 이동/복사 불필요
        Lock& operator[](std::size_t i) noexcept                { return locks[i]; }
        const Lock& operator[](std::size_t i) const noexcept  { return locks[i]; }
    };

    template <std::size_t N>
    [[nodiscard("READ_LOCK_IDX 반환 실패")]] inline ReadLockGuard READ_LOCK_IDX(LockBox<N>& box, std::size_t idx,
            std::source_location loc = std::source_location::current())
	{
        return ReadLockGuard(box[idx], loc.function_name());
    }

    template <std::size_t N>
    [[nodiscard("WRITE_LOCK_IDX 반환 실패")]] inline WriteLockGuard WRITE_LOCK_IDX(LockBox<N>& box, std::size_t idx,
            std::source_location loc = std::source_location::current())
	{
        return WriteLockGuard(box[idx], loc.function_name());
    }

    template <std::size_t N>
    [[nodiscard("READ_LOCK 반환 실패")]] inline ReadLockGuard READ_LOCK(LockBox<N>& box,
        std::source_location loc = std::source_location::current())
	{
        return READ_LOCK_IDX(box, 0, loc);
    }

    template <std::size_t N>
    [[nodiscard("WRITE_LOCK 반환 실패")]] inline WriteLockGuard WRITE_LOCK(LockBox<N>& box,
        std::source_location loc = std::source_location::current())
	{
        return WRITE_LOCK_IDX(box,0, loc);
    }
}

// Memory
export
{
    // memory 바이트 단위 할당/해제 (매크로 대체)
    [[nodiscard("메모리 할당 실패")]] inline void* xalloc(std::size_t size,
        std::size_t align = alignof(std::max_align_t))
    {
        return BaseAllocator::Alloc(size, align);
    }

    inline void xfree(void* ptr,
        std::size_t align = alignof(std::max_align_t)) noexcept
    {
        BaseAllocator::Release(ptr, align);
    }
}