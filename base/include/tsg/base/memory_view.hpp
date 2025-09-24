#ifndef TSG_MEMORY_VIEW_HPP
#define TSG_MEMORY_VIEW_HPP

#include <cassert>

namespace tsg {

template <typename T> class MemoryView {
  public:
    using pointer_type = T *;
    using reference_type = T &;
    using const_pointer_type = const T *;
    using const_reference_type = const T &;
    using iterator_type = pointer_type;
    using const_iterator_type = const_pointer_type;
    using size_type = size_t;

  public:
    constexpr MemoryView(pointer_type ptr, size_type size) : m_data(ptr), m_size(size) {}

    constexpr size_type size() const { return m_size; }

    constexpr bool empty() const { return N == 0 ? true : false; }

    constexpr reference_type at(size_type i) {
        assert(i < m_size);
        return m_data[i];
    }

    constexpr const_reference_type at(size_type i) const {
        assert(i < m_size);
        return m_data[i];
    }

    constexpr reference_type operator[](size_type i) {
        assert(i < m_size);
        return this->at(i);
    }

    constexpr const_reference_type operator[](size_type i) const {
        assert(i < m_size);
        return this->at(i);
    }

    constexpr iterator_type begin() { return &m_data[0]; }

    constexpr const_iterator_type begin() const { return &m_data[0]; }

    constexpr iterator_type end() { return &m_data[m_size]; }

    constexpr const_iterator_type end() const { return &m_data[m_size]; }

    constexpr reference_type front() { return m_data[0]; }

    constexpr const_reference_type front() const { return m_data[0]; }

    constexpr reference_type back() { return m_data[m_size - 1]; }

    constexpr const_reference_type back() const { return m_data[m_size - 1]; }

    constexpr pointer_type data() { return m_data; }

    constexpr const_pointer_type data() const { return m_data; }

  private:
    pointer_type m_data;
    size_type m_size;
};

} // namespace tsg

#endif // TSG_MEMORY_VIEW_HPP