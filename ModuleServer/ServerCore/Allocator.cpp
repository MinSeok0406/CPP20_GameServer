module Allocator;

using namespace std;
/*----------------
   BaseAllocator
----------------*/

void* BaseAllocator::Alloc(size_t size, std::size_t align)
{
    if (align <= alignof(std::max_align_t))
    {
        if (void* p = ::malloc(size))
        {
            return p;
        }

        throw ::bad_alloc{};
    }
    return ::operator new(size, std::align_val_t(align));
}

void BaseAllocator::Release(void* ptr, std::size_t align) noexcept
{
    if (!ptr)
    {
        return;
    }

    if (align <= alignof(std::max_align_t))
    {
        ::free(ptr);
    }
    else
    {
        ::operator delete(ptr, std::align_val_t(align));
    }
}
