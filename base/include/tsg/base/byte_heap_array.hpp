
//  Note/Recomendation: C++11 and above - (P0136R1) - Inherit default constructors
//
//  - Use constructor inheritance to make ByteHeapArray constructor
//    accessible with derived class names
//
//    ------
//      class DerivedClass : public ByteHeapArray {
//        public:
//          using ByteHeapArray::ByteHeapArray;
//      ...
//    ------

#ifndef TSG_BASE_HEAP_ARRAY_HPP
#define TSG_BASE_HEAP_ARRAY_HPP

#include "allocator.hpp"
#include "hex.hpp"
#include <algorithm>
#include <cassert>
#include <cstdint>

namespace tsg {

class ByteHeapArray {
  public:
    using allocator_type = Allocator;
    using value_type = uint8_t;
    using size_type = size_t;
    using const_size_type = const size_type;
    using pointer_type = uint8_t *;
    using const_pointer_type = const pointer_type;
    using reference_type = uint8_t &;
    using const_reference_type = const uint8_t &;
    using boolean_type = bool;
    using initializer_list_type = std::initializer_list<uint8_t>;
    using iterator_type = pointer_type;
    using const_iterator_type = const_pointer_type;

    constexpr ByteHeapArray() : m_data(nullptr), m_size(0) {}

    constexpr ByteHeapArray(size_type count) : ByteHeapArray() { reallocate(count); }

    constexpr ByteHeapArray(initializer_list_type l) : ByteHeapArray(l.size()) {
        for (size_t index = 0; index < l.size(); index++) {
            at(index) = *(l.begin() + index);
        }
    }

    constexpr ByteHeapArray(const ByteHeapArray &v) : ByteHeapArray(v.size()) {
        for (size_type index = 0; index < v.size(); index++) {
            at(index) = v.at(index);
        }
    }

    ByteHeapArray(ByteHeapArray &&v) : ByteHeapArray() { swap(v); }

    ByteHeapArray(uint8_t *data, size_t size) : ByteHeapArray() { append(data, size); }

    ByteHeapArray(const char *str) : ByteHeapArray() {
        resize(strlen(str) / 2);
        hex::byte_array_of(str, data(), size());
    }

    ~ByteHeapArray() { this->cleanup(); }

    ByteHeapArray &operator=(const ByteHeapArray &other) {
        cleanup();

        pointer_type new_data = (pointer_type)allocator().allocate(other.size(), TSG_FL_LN_FN);
        assert(new_data != nullptr);

        for (size_type index = 0; index < other.size(); index++) {
            new_data[index] = other.at(index);
        }

        data_set(new_data);
        size_set(other.size());

        return *this;
    }

    ByteHeapArray &operator=(ByteHeapArray &&other) {
        swap(other);
        return *this;
    }

    constexpr void append(const_pointer_type add_data, const_size_type count) {
        if ((add_data == nullptr) || (count == 0)) {
            return; // nothing to append
        }

        size_type new_size = size() + count;
        pointer_type new_data = (pointer_type)allocator().allocate(new_size, TSG_FL_LN_FN);
        assert(new_data != nullptr);

        if (data() != nullptr) { // Preserve current data elements
            size_type total_elements_to_move = size() < new_size ? size() : new_size;

            for (size_type index = 0; index < total_elements_to_move; index++) {
                new_data[index] = data()[index];
            }
            allocator().deallocate(data(), size(), TSG_FL_LN_FN);
        }

        for (size_type index = 0; index < count; index++) {
            new_data[size() + index] = add_data[index];
        }

        data_set(new_data);
        size_set(new_size);
    }

    constexpr void append(const ByteHeapArray &v) { this->append(v.data(), v.size()); }

    constexpr void append(ByteHeapArray &&v) { this->append(v.data(), v.size()); }

    constexpr int32_t allocate(size_type count) {
        if ((data() != nullptr) && (size() == count)) { // just use the same allocation
            return 0;
        }

        cleanup();

        pointer_type new_data = (uint8_t *)allocator().allocate(count, TSG_FL_LN_FN);
        if (new_data == nullptr) {
            return -10;
        }

        data_set(new_data);
        size_set(count);

        return 0;
    }

    constexpr int32_t reallocate(size_type new_size) {
        if ((size() == new_size) && (data() != nullptr)) { // We can reuse the same allocation
            return 0;
        }

        pointer_type new_data = (uint8_t *)allocator().allocate(new_size, TSG_FL_LN_FN);
        if (new_data == nullptr) {
            return -10;
        }

        if (data() != nullptr) { // Preserve current data elements
            size_type elements_to_move_count = size() < new_size ? size() : new_size;
            memcpy(new_data, data(), size() < new_size ? size() : new_size);
            allocator().deallocate(data(), size(), TSG_FL_LN_FN);
        }

        data_set(new_data);
        size_set(new_size);

        return 0;
    }

    constexpr void deallocate() { cleanup(); }

    constexpr void push_back(const_reference_type elem) {
        reallocate(size() + 1);
        at(size() - 1) = elem;
    }

    constexpr void push_back(reference_type &elem) {
        reallocate(size() + 1);
        at(size() - 1) = elem;
    }

    constexpr void resize(size_type new_size) { reallocate(new_size); }

    constexpr void reserve(size_type required_size) {
        if (required_size > size()) {
            reallocate(required_size);
        }
    }

    constexpr iterator_type emplace(iterator_type target) {
        if ((data() == nullptr) || (target >= end())) {
            return nullptr;
        }

        *target = 0;

        return target;
    }

    constexpr iterator_type emplace_back() {
        reallocate(size() + 1);

        pointer_type target = &at(size() - 1);
        *target = 0;

        return target;
    }

    void swap(ByteHeapArray &other) {
        std::swap(m_data, other.m_data);
        std::swap(m_size, other.m_size);
        std::swap(m_allocator, other.m_allocator);
    }

    constexpr void fill(const_reference_type value) {
        for (size_type index = 0; index < size(); index++) {
            at(index) = value;
        }
    }

    inline void set_range(size_type dest_index, pointer_type src, size_type count){
        // check if the incoming data fits
        memcpy(&data()[dest_index], src, count);
    }

    constexpr void clear() { cleanup(); }

    constexpr pointer_type data() { return m_data; }

    constexpr const_pointer_type data() const { return m_data; }

    constexpr reference_type at(size_type index) { return data()[index]; }

    constexpr const_reference_type at(size_type index) const { return data()[index]; }

    constexpr reference_type operator[](size_type index) { return at(index); }

    constexpr const_reference_type operator[](size_type index) const { return at(index); }

    constexpr reference_type front() { return at(0); }

    constexpr const_reference_type front() const { return at(0); }

    constexpr reference_type back() { return at(size() - 1); }

    constexpr const_reference_type back() const { return at(size() - 1); }

    constexpr iterator_type begin() { return data(); }

    constexpr iterator_type end() { return data() + size(); }

    constexpr const_iterator_type begin() const { return data(); }

    constexpr const_iterator_type end() const { return data() + size(); }

    constexpr size_type size() { return m_size; }

    constexpr const_size_type size() const { return m_size; }

    constexpr bool empty() const { return size() == 0 ? true : false; }

  protected:
    constexpr void data_set(pointer_type new_pointer) { m_data = new_pointer; }

    constexpr void size_set(size_type new_size) { m_size = new_size; }

    constexpr void size_increase(size_type count) { m_size += count; }

    constexpr void cleanup() {
        if (data() != nullptr) {
            allocator().deallocate(data(), size(), TSG_FL_LN_FN);
        }
        data_set(nullptr);
        size_set(0);
    }

    constexpr allocator_type &allocator() { return m_allocator; }

  private:
    pointer_type m_data;
    size_type m_size;
    allocator_type m_allocator;
};

} // namespace tsg

#endif // TSG_BASE_HEAP_ARRAY_HPP