#ifndef TSG_SMARTCARD_ATR_HPP
#define TSG_SMARTCARD_ATR_HPP

#include <algorithm>
#include <cstdint>

namespace tsg {
namespace smartcard {

class ATR {
  public:
    constexpr ATR() {}

    constexpr ATR(std::initializer_list<uint8_t> l) : m_size(l.size()) {
        for (size_t index = 0; index < l.size(); index++) {
            at(index) = *(l.begin() + index);
        }
    }

    ATR(uint8_t *bytes, size_t size) {
        memcpy(m_data, bytes, size);
        m_size = size;
    }

    ~ATR() {}

    void swap(ATR &other) {
        std::swap(m_data, other.m_data);
        std::swap(m_size, other.m_size);
    }

    void reset(uint8_t *bytes, size_t size) {
        memcpy(m_data, bytes, size);
        m_size = size;
    }

    constexpr size_t size() const { return m_size; }

    constexpr bool empty() const { return m_size == 0; }

    constexpr uint8_t &at(size_t i) { return m_data[i]; }

    constexpr const uint8_t &at(size_t i) const { return m_data[i]; }

    constexpr uint8_t &operator[](size_t i) { return this->at(i); }

    constexpr const uint8_t &operator[](size_t i) const { return this->at(i); }

    constexpr uint8_t *begin() { return &m_data[0]; }

    constexpr const uint8_t *begin() const { return &m_data[0]; }

    constexpr uint8_t *end() { return &m_data[size()]; }

    constexpr const uint8_t *end() const { return &m_data[size()]; }

    constexpr uint8_t &front() { return m_data[0]; }

    constexpr const uint8_t &front() const { return m_data[0]; }

    constexpr uint8_t &back() { return m_data[size() - 1]; }

    constexpr const uint8_t &back() const { return m_data[size() - 1]; }

    constexpr size_t capacity() const { return m_max_size; }

    constexpr uint8_t *data() { return m_data; }

    constexpr const uint8_t *data() const { return m_data; }

  private:
    static const size_t m_max_size = 42;
    uint8_t m_data[m_max_size] = {};
    size_t m_size = 0;
};

} // namespace smartcard
} // namespace tsg

#endif // TSG_SMARTCARD_ATR_HPP
