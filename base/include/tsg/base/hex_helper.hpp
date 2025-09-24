#ifndef TSG_BASE_HEX_HELPER_HPP
#define TSG_BASE_HEX_HELPER_HPP

// see https://github.com/zbjornson/fast-hex
// see https://stackoverflow.com/questions/3408706/hexadecimal-String-to-byte-Array-in-c
// see https://stackoverflow.com/questions/14050452/how-to-convert-byte-Array-to-hex-String-in-visual-c
#include "hex.hpp"
#include <sstream>
#include <string>

namespace tsg {
namespace hex {

std::string to_hex_string(Byte *ptr, size_t length, bool uppercase = false, const char *prefix = "",
                          const char *separator = "\0") {
    std::stringstream ss;
    for (size_t i = 0; i < length; i++) {
        char c1;
        char c2;
        hex_string_of(ptr[i], c1, c2, uppercase);
        ss << prefix << c1 << c2;
        if (i < length - 1) {
            ss << separator;
        }
    }
    return ss.str();
}

} // namespace hex
} // namespace tsg

#endif // TSG_BASE_HEX_HELPER_HPP
