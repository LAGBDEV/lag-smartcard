#ifndef TSG_SMARTCARD_COMMAND_APDU_HPP
#define TSG_SMARTCARD_COMMAND_APDU_HPP

#include <tsg/base/byte_heap_array.hpp>

namespace tsg {
namespace smartcard {
class CommandAPDU : public ByteHeapArray {
  public:
    using ByteHeapArray::ByteHeapArray;

    CommandAPDU(uint8_t cls, uint8_t ins, uint8_t p1, uint8_t p2) : CommandAPDU(4) {
        at(0) = cls;
        at(1) = ins;
        at(2) = p1;
        at(3) = p2;
    }

    CommandAPDU(uint8_t cls, uint8_t ins, uint8_t p1, uint8_t p2, uint8_t *data, size_t data_size) : CommandAPDU(4) {
        at(0) = cls;
        at(1) = ins;
        at(2) = p1;
        at(3) = p2;
        push_back((uint8_t)data_size);
        append(data, data_size);
    }

    CommandAPDU(uint8_t cls, uint8_t ins, uint8_t p1, uint8_t p2, uint8_t *data, size_t data_size, uint8_t le)
        : CommandAPDU(4) {
        at(0) = cls;
        at(1) = ins;
        at(2) = p1;
        at(3) = p2;
        push_back((uint8_t)data_size);
        append(data, data_size);
        push_back(le);
    }

    ~CommandAPDU() { ByteHeapArray::~ByteHeapArray(); }
};

} // namespace smartcard

} // namespace tsg

#endif // TSG_SMARTCARD_COMMAND_APDU_HPP