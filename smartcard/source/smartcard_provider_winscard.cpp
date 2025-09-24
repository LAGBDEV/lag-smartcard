#include <iostream>
#include <string>
#include <tsg/base/memory.hpp>
#include <tsg/smartcard/smartcard_provider.hpp>
#include <vector>

#include "internal_winscard.hpp"

namespace tsg {
namespace smartcard {

namespace priv {

struct ProviderImpl {
    SCARDCONTEXT context;
    std::vector<TerminalData> terminals;
};

} // namespace priv

SmartCardProvider::SmartCardProvider() {
    m_impl = (priv::ProviderImpl *)TSG_ALLOC(sizeof(priv::ProviderImpl));
    tsg::Memory::construct_at(m_impl);
}

SmartCardProvider::~SmartCardProvider() {
    tsg::Memory::destroy_at(m_impl);
    TSG_FREE(m_impl, sizeof(priv::ProviderImpl));
}

int32_t SmartCardProvider::initialize() {
    LONG rv = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &m_impl->context);
    if (rv != SCARD_S_SUCCESS) {
        if (rv == SCARD_E_NO_SERVICE) {
            std::cerr << "ERROR - winscard: The smart card resource manager is not running." << std::endl;
        } else {
            std::cerr << "ERROR - winscard: " << rv << std::endl;
        }
        return -1;
    }
    std::cout << "WinSCard Context established" << std::endl;

    return 0;
}

int32_t SmartCardProvider::cleanup() {
    LONG rv = SCardReleaseContext(m_impl->context);
    if (rv != SCARD_S_SUCCESS) {
        std::cerr << "ERROR - winscard: " << rv << std::endl;
        return -1;
    }
    std::cout << "WinSCard Context released" << std::endl;
    return 0;
}

CardConnection SmartCardProvider::create_card_connection() {
    bool card_found;
    for (auto &t : m_impl->terminals) {
        SCARD_READERSTATE reader_state = {};
        reader_state.szReader = t.name.c_str();
        LONG rv = SCardGetStatusChange(m_impl->context, 2000, &reader_state, 1);
        if (rv != SCARD_S_SUCCESS) {
            std::cerr << "ERROR - winscard: " << rv << std::endl;
            card_found = false;
        }

        card_found = (reader_state.cbAtr > 0) ? true : false;
        if (card_found) {
            CardConnectCI ccci;
            ccci.context = m_impl->context;
            ccci.terminal = t;
            CardConnection cc;
            cc.initialize(ccci);
            return std::move(cc);
        }
    }

    return CardConnection();
}

void SmartCardProvider::destroy_card_connection(CardConnection &cc) {
    if (cc.is_connected()) {
        cc.disconnect();
    }

    if (cc.is_valid()) {
        cc.cleanup();
    }
}

void SmartCardProvider::refresh() {
    if (!m_impl->terminals.empty()) {
        m_impl->terminals.clear();
    }

    TCHAR *reader_list_ptr = nullptr;
    DWORD len = SCARD_AUTOALLOCATE;

    LONG rv = SCardListReaders(m_impl->context, nullptr, (LPTSTR)&reader_list_ptr, &len);
    if (rv != SCARD_S_SUCCESS) {
        if (rv == SCARD_E_NO_READERS_AVAILABLE) {
            std::cerr << "No card reader found" << std::endl;
        } else {
            std::cerr << "ERROR - winscard: " << rv << std::endl;
        }
        return;
    }

    TCHAR *pszReader = reader_list_ptr;
    uint32_t index = 0;
    while (*pszReader) {
        TerminalData terminal{index, pszReader};
        m_impl->terminals.emplace_back(terminal);
        pszReader += strlen(pszReader) + 1;
        index++;
    }

    rv = SCardFreeMemory(m_impl->context, reader_list_ptr);
    if (rv != SCARD_S_SUCCESS) {
        std::cerr << "ERROR - winscard: " << rv << std::endl;
        return;
    }

    int i = 0;
    for (auto &terminal : m_impl->terminals) {
        std::cerr << "[" << i << "] " << terminal.name << std::endl;
        i++;
    }
}

} // namespace smartcard
} // namespace tsg
