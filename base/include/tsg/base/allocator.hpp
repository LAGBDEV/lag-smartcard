#ifndef TSG_BASE_ALLOCATOR_HPP
#define TSG_BASE_ALLOCATOR_HPP

#include "memory.hpp"

namespace tsg {

class Allocator {
  public:
    constexpr Allocator(const char *name = "") {}

    Allocator(const Allocator &, const char *) {}

    ~Allocator() {}

    Allocator &operator=(const Allocator &x) { return *this; }

    void *allocate(size_t n, int flags = 0) { return TSG_ALLOC(n); }

    void *allocate(size_t n, const char *source_file, unsigned int line, const char *function_id) {
        return TSG_ALLOC2(n, source_file, line, function_id);
    }

    void *allocate(size_t n, size_t alignment, size_t offset, int flags = 0) { return TSG_ALLOC(n); }

    void deallocate(void *p, size_t n) { TSG_FREE(p,n); }

    void deallocate(void *p, size_t n, const char *source_file, unsigned int line, const char *function_id) { TSG_FREE2(p,n, source_file, line, function_id); }

    const char *get_name() const { return "Default Allocator"; }

    void set_name(const char *pName) {}
};

inline bool operator==(const Allocator &a, const Allocator &b) { return true; }

inline bool operator!=(const Allocator &a, const Allocator &b) { return false; }

template <typename Alloc> void *allocate_memory(Alloc &alloc, size_t count) { return alloc.allocate(count); }

template <typename Alloc> void deallocate_memory(Alloc &alloc, void *target, size_t count) {
    alloc.deallocate(target, count);
}

} // namespace tsg

#endif // TSG_BASE_ALLOCATOR_HPP