#ifndef TSG_BASE_ARRAY_HPP
#define TSG_BASE_ARRAY_HPP

#include "hex.hpp"
#include <algorithm>
#include <cassert>
#include <cstdint>

namespace tsg {

template <size_t N> struct ByteArray {
  public:
    using value_type = uint8_t;
    using pointer_type = value_type *;
    using reference_type = value_type &;
    using const_pointer_type = const value_type *;
    using const_reference_type = const value_type &;
    using iterator_type = pointer_type;
    using const_iterator_type = const_pointer_type;
    using size_type = size_t;

    value_type m_data[N];

  public:
    constexpr void fill(const_reference_type value) {
        for (size_type index = 0; index < size(); index++) {
            at(index) = value;
        }
    }

    constexpr void swap(ByteArray &other) { std::swap(this->data(), other.data()); }

    constexpr size_type size() const { return N; }

    constexpr bool empty() const { return N == 0 ? true : false; }

    constexpr reference_type at(size_type i) { return m_data[i]; }

    constexpr const_reference_type at(size_type i) const { return m_data[i]; }

    constexpr reference_type operator[](size_type i) { return this->at(i); }

    constexpr const_reference_type operator[](size_type i) const { return this->at(i); }

    constexpr iterator_type begin() { return &m_data[0]; }

    constexpr const_iterator_type begin() const { return &m_data[0]; }

    constexpr iterator_type end() { return &m_data[N]; }

    constexpr const_iterator_type end() const { return &m_data[N]; }

    constexpr reference_type front() { return m_data[0]; }

    constexpr const_reference_type front() const { return m_data[0]; }

    constexpr reference_type back() { return m_data[N - 1]; }

    constexpr const_reference_type back() const { return m_data[N - 1]; }

    constexpr pointer_type data() { return m_data; }

    constexpr const_pointer_type data() const { return m_data; }
};

} // namespace tsg

#endif // TSG_BASE_ARRAY_HPP