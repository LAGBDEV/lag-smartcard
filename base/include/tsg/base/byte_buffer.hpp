#ifndef TSG_BASE_BYTE_BUFFER_HPP
#define TSG_BASE_BYTE_BUFFER_HPP

#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <tsg/base/allocator.hpp>

namespace tsg {
class ByteBuffer {
  public:
    using ValueT = uint8_t;
    using PointerT = ValueT *;
    using ReferenceT = ValueT &;
    using SizeT = size_t;
    using IteratorT = PointerT;
    using InitializerListT = std::initializer_list<ValueT>;
    using AllocT = Allocator;

    constexpr ByteBuffer() : m_data(nullptr), m_capacity(0), m_size(0) {}

    constexpr ByteBuffer(SizeT count) : ByteBuffer() { resize(count); }

    constexpr ByteBuffer(InitializerListT l) : ByteBuffer() { reset((PointerT)l.begin(), l.size()); }

    constexpr ByteBuffer(const ByteBuffer &bb) : ByteBuffer() { reset(bb); }

    constexpr ByteBuffer(ByteBuffer &&v) : ByteBuffer() { swap(v); }

    ~ByteBuffer() { deallocate(); }

    constexpr int32_t reset(SizeT count = 0) {
        if (count == 0) {
            deallocate();
        } else {
            return allocate(count);
        }
        return 0;
    }

    constexpr int32_t reset(const PointerT ptr, const SizeT count) {
        if (count == 0 || ptr == nullptr) {
            return -1;
        }
        resize(count);
        memcpy(data(), ptr, count);
        return 0;
    }

    constexpr int32_t reset(const ByteBuffer &bb) { return reset(bb.data(), bb.size()); }

    constexpr void reserve(size_t count) { reallocate(count); }

    constexpr void resize(size_t count) {
        reallocate(count);
        m_size = count;
    }

    constexpr void append(const PointerT ptr, const SizeT count) {
        SizeT old_size = size();
        resize(old_size + count);
        memcpy(data() + old_size, ptr, count);
    }

    constexpr void append(const ByteBuffer &bb) { append(bb.data(), bb.size()); }

    constexpr void append(ByteBuffer &&bb) { append(bb.data(), bb.size()); }

    constexpr void push_back(const ReferenceT v) {
        resize(size() + 1);
        at(size() - 1) = v;
    }

    constexpr void push_back(ReferenceT &v) {
        resize(size() + 1);
        at(size() - 1) = v;
    }

    constexpr IteratorT emplace(IteratorT target) {
        if ((data() == nullptr) || (target >= end())) {
            return nullptr;
        }
        *target = 0;
        return target;
    }

    constexpr IteratorT emplace_back() {
        resize(size() + 1);
        return &at(size() - 1);
    }


    constexpr void fill(const ValueT value) {
        for (SizeT index = 0; index < size(); index++) {
            at(index) = value;
        }
    }

    constexpr void swap(ByteBuffer &o) {
        auto t_data = m_data;
        auto t_capacity = m_capacity;
        auto t_size = m_size;

        m_data = o.m_data;
        m_capacity = o.m_capacity;
        m_size = o.m_size;

        o.m_data = t_data;
        o.m_capacity = t_capacity;
        o.m_size = t_size;
    }

    constexpr PointerT data() { return m_data; }

    constexpr const PointerT data() const { return m_data; }

    constexpr ReferenceT at(SizeT index) { return data()[index]; }

    constexpr const ReferenceT at(SizeT index) const { return data()[index]; }

    constexpr ReferenceT operator[](SizeT index) { return at(index); }

    constexpr const ReferenceT operator[](SizeT index) const { return at(index); }

    constexpr ReferenceT front() { return at(0); }

    constexpr const ReferenceT front() const { return at(0); }

    constexpr ReferenceT back() { return at(size() - 1); }

    constexpr const ReferenceT back() const { return at(size() - 1); }

    constexpr IteratorT begin() { return data(); }

    constexpr IteratorT end() { return data() + size(); }

    constexpr const IteratorT begin() const { return data(); }

    constexpr const IteratorT end() const { return data() + size(); }

    constexpr SizeT size() { return m_size; }

    constexpr const SizeT size() const { return m_size; }

    constexpr SizeT capacity() { return m_capacity; }

    constexpr const SizeT capacity() const { return m_capacity; }

    constexpr bool empty() const { return size() == 0 ? true : false; }

  public:
    constexpr int32_t allocate(SizeT count) {
        if ((data() != nullptr) && capacity() == count) { // Use the same allocation
            m_size = 0;
            return 0;
        }

        deallocate();

        PointerT new_data = (PointerT)allocator().allocate(count, TSG_FL_LN_FN);
        if (new_data == nullptr) {
            return -1;
        }

        m_data = new_data;
        m_capacity = count;
        m_size = 0;

        return 0;
    }

    constexpr int32_t reallocate(SizeT count) {
        if ((data() != nullptr) && capacity() == count) { // Use the same allocation
            return 0;
        }

        PointerT new_data = (PointerT)allocator().allocate(count, TSG_FL_LN_FN);
        if (new_data == nullptr) {
            return -1;
        }

        SizeT new_size = size() < count ? size() : count;

        if (data() != nullptr) {
            memcpy(new_data, data(), new_size);
            allocator().deallocate(data(), capacity(), TSG_FL_LN_FN);
        }

        m_data = new_data;
        m_capacity = count;
        m_size = new_size;

        return 0;
    }

    constexpr void deallocate() {
        if (data() != nullptr) {
            allocator().deallocate(data(), capacity(), TSG_FL_LN_FN);
        }
        m_data = nullptr;
        m_capacity = 0;
        m_size = 0;
    }

  protected:
    inline PointerT _memory_alloc(SizeT count) { return (PointerT)allocator().allocate(count, TSG_FL_LN_FN); }

    inline void _memory_dealloc(PointerT ptr, SizeT count) { allocator().deallocate(ptr, count, TSG_FL_LN_FN); }

    constexpr AllocT &allocator() { return m_allocator; }

  private:
    PointerT m_data;
    SizeT m_capacity;
    SizeT m_size;
    AllocT m_allocator;
};
} // namespace tsg

#endif // TSG_BASE_BYTE_BUFFER_HPP