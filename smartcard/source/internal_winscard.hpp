#ifndef TSG_SMARTCARD_INTERNAL_WINSCARD_HPP
#define TSG_SMARTCARD_INTERNAL_WINSCARD_HPP

#include <WinSCard.h>
#include <cstdint>
#include <string>

namespace tsg {
namespace smartcard {

struct TerminalData {
    uint32_t index;
    std::string name;
};

struct CardConnectCI {
    SCARDCONTEXT context;
    TerminalData terminal;
};

} // namespace smartcard
} // namespace tsg

#endif // TSG_SMARTCARD_INTERNAL_WINSCARD_HPP