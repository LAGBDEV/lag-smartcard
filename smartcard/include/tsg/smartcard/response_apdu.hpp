#ifndef TSG_SMARTCARD_RESPONSE_APDU_HPP
#define TSG_SMARTCARD_RESPONSE_APDU_HPP

#include <algorithm>
#include <cstdint>

namespace tsg {
namespace smartcard {

struct StatusWord {
    enum SW1 {
        response_bytes_still_available = 0x61,
        wrong_length_le = 0x6C,
    };
};

constexpr uint32_t k_max_rapdu_length = 256;

class ResponseAPDU {
  public:
    constexpr ResponseAPDU() {}

    constexpr ResponseAPDU(std::initializer_list<uint8_t> l) : m_size(l.size()) {
        for (size_t index = 0; index < l.size(); index++) {
            at(index) = *(l.begin() + index);
        }
    }

    ResponseAPDU(uint8_t *bytes, size_t size) {
        memcpy(m_data, bytes, size);
        m_size = size;
    }

    ~ResponseAPDU() {}

    constexpr uint8_t &get_sw1() { return at(size() - 2); }

    constexpr uint8_t &get_sw2() { return at(size() - 1); }

    constexpr bool sw_is(uint8_t sw1, uint8_t sw2) { return (get_sw1() == sw1 && get_sw2() == sw2); }

    void swap(ResponseAPDU &other) {
        std::swap(m_data, other.m_data);
        std::swap(m_size, other.m_size);
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

    constexpr size_t capacity() const { return k_max_rapdu_length; }

    constexpr uint8_t *data() { return m_data; }

    constexpr const uint8_t *data() const { return m_data; }

  private:
    uint8_t m_data[k_max_rapdu_length] = {};
    size_t m_size = 0;
};

} // namespace smartcard
} // namespace tsg

#endif // TSG_SMARTCARD_RESPONSE_APDU_HPP