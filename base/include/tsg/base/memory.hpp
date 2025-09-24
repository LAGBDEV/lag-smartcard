#ifndef TSG_BASE_MEMORY_HPP
#define TSG_BASE_MEMORY_HPP

// clang-format off

/// Helper macros for file-line-function reporting on memory operations
#define TSG_FL_LN       __FILE__,__LINE__
#define TSG_FL_LN_FN    __FILE__,__LINE__,__FUNCTION__

#ifdef TSG_USE_MEMORY_MANAGER
#include "mmgr/mmgr.h"

#define TSG_ALLOC(count)                    mmgr_allocator(TSG_FL_LN_FN, mmgr_alloc_malloc, count)
#define TSG_ALLOC2(count, f, l, fn)         mmgr_allocator(f, l, fn, mmgr_alloc_malloc, count)
#define TSG_FREE(target, count)             mmgr_deallocator(TSG_FL_LN_FN, mmgr_alloc_free, target)
#define TSG_FREE2(target, count, f, l, fn)  mmgr_deallocator(f, l, fn, mmgr_alloc_free, target)

#else

#define TSG_ALLOC(count)                    operator new[](count)
#define TSG_ALLOC2(count, f, l, fn)         operator new[](count)
#define TSG_FREE(target, count)             operator delete[](target, count)
#define TSG_FREE2(target, count, f, l, fn)  operator delete[](target, count)

#endif
// clang-format on

namespace tsg {

struct Memory {

    template <class T> static constexpr void destroy_at(T *target) {
        if constexpr (std::is_array_v<T>)
            for (auto &elem : *target)
                (destroy_at)(std::addressof(elem));
        else
            target->~T();
    }

    template <class T, class... Args> static constexpr void construct_at(T *target, Args &&...args) {
        new (static_cast<void *>(target)) T(std::forward<Args>(args)...);
    }
};

} // namespace tsg

#endif // TSG_BASE_MEMORY_HPP
