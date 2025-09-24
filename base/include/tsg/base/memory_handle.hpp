#ifndef TSG_MEMORY_HANDLE_HPP
#define TSG_MEMORY_HANDLE_HPP

#include <type_traits>

namespace tsg {

struct MemoryHandle {
    using PointerT = void *;
    using SizeT = size_t;
    
    PointerT pointer;
    SizeT size;

    MemoryHandle() {}
    MemoryHandle(PointerT ptr, SizeT s) : pointer(ptr), size(s) {}
};

} // namespace tsg

#endif // TSG_MEMORY_HANDLE_HPP
