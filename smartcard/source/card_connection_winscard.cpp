#include "internal_winscard.hpp"
#include <cstdint>
#include <iostream>
#include <tsg/base/memory.hpp>
#include <tsg/smartcard/card_connection.hpp>

namespace tsg {
namespace smartcard {

/* Temporal Start */
void log_hexstring_of(uint8_t *bytes, size_t size, const char *prefix, const char *separator) {
    std::cout << prefix;
    for (size_t i = 0; i < size; i++) {
        char f, s;
        hex::hex_string_of(bytes[i], f, s, true);
        std::cout << separator << f << s << " ";
    }
    std::cout << std::endl;
}

void log_hexstring_of(CommandAPDU &capdu) { log_hexstring_of(capdu.data(), capdu.size(), "C-APDU - ", ""); }

void log_hexstring_of(ResponseAPDU &rapdu) { log_hexstring_of(rapdu.data(), rapdu.size(), "R-APDU - ", ""); }

void log_hexstring_of(ATR &atr) { log_hexstring_of(atr.data(), atr.size(), "ATR - ", ""); }
/* Temporal End */

struct CardConnectionImpl {
    SCARDCONTEXT context;
    TerminalData terminal;

    SCARDHANDLE card_handle;
    SCARD_IO_REQUEST send_pci;
    CardConnection::CommunicationProtocol protocol;

    bool is_connected{false};

    ATR atr_bytes;
};

bool impl_update_atr_bytes(CardConnectionImpl *impl) {
    SCARD_READERSTATE reader_state;
    reader_state.szReader = impl->terminal.name.c_str();
    LONG rv = SCardGetStatusChange(impl->context, 2000, &reader_state, 1);
    if (rv != SCARD_S_SUCCESS) {
        std::cerr << "ERROR - winscard: " << rv << std::endl;
        return false;
    }

    impl->atr_bytes = ATR(reader_state.rgbAtr, reader_state.cbAtr);

    return true;
}

bool impl_update_send_pci(CardConnectionImpl *impl, int32_t active_protocol) {
    switch (active_protocol) {
    case SCARD_PROTOCOL_T0: {
        impl->send_pci = *SCARD_PCI_T0;
        impl->protocol = CardConnection::com_protocol_t_0;
    } break;

    case SCARD_PROTOCOL_T1: {
        impl->send_pci = *SCARD_PCI_T1;
        impl->protocol = CardConnection::com_protocol_t_1;
    } break;

    default: {
        return false;
    }
    }

    return true;
}

void impl_swap(CardConnectionImpl &a, CardConnectionImpl &b) {

    auto aux_context = a.context;
    auto aux_terminal = a.terminal;
    auto aux_card_handle = a.card_handle;
    auto aux_send_pci = a.send_pci;
    auto aux_protocol = a.protocol;
    auto aux_is_connected = a.is_connected;
    auto aux_atr_bytes = a.atr_bytes;

    a.context = b.context;
    a.terminal = b.terminal;
    a.card_handle = b.card_handle;
    a.send_pci = b.send_pci;
    a.protocol = b.protocol;
    a.is_connected = b.is_connected;
    a.atr_bytes = b.atr_bytes;

    b.context = aux_context;
    b.terminal = aux_terminal;
    b.card_handle = aux_card_handle;
    b.send_pci = aux_send_pci;
    b.protocol = aux_protocol;
    b.is_connected = aux_is_connected;
    b.atr_bytes = aux_atr_bytes;
}

static char *pcsc_stringify_error(LONG rv)
{
 static char out[20];
 sprintf_s(out, sizeof(out), "0x%08X", rv);

 return out;
}

#define CHECK(f, rv) \
 if (SCARD_S_SUCCESS != rv) \
 { \
  printf(f ": %s\n", pcsc_stringify_error(rv)); \
 }

ResponseAPDU impl_transmit(CardConnectionImpl *impl, CommandAPDU &capdu) {
    SCARD_IO_REQUEST send_pci = impl->send_pci;

    uint8_t buffer[2048];
    unsigned long length = sizeof(buffer);

    LONG rv = SCardTransmit(impl->card_handle, &send_pci, capdu.data(), capdu.size(), NULL, buffer, &length);
    CHECK("SCardTransmit", rv);

    if (rv != SCARD_S_SUCCESS) {
        return ResponseAPDU();
    }

    return ResponseAPDU(buffer, length);
}

CardConnection::CardConnection() { m_impl = nullptr; }

CardConnection::~CardConnection() {}

int32_t CardConnection::initialize(CardConnectCI &ci) {
    if (m_impl == nullptr) {
        auto new_impl = (CardConnectionImpl *)TSG_ALLOC(sizeof(CardConnectionImpl));
        if (new_impl == nullptr) {
            return -1;
        }
        tsg::Memory::construct_at(new_impl);
        m_impl = new_impl;
    }

    m_impl->context = ci.context;
    m_impl->terminal = ci.terminal;
    m_impl->is_connected = false;

    return 0;
}

int32_t CardConnection::cleanup() {
    if (m_impl != nullptr) {
        tsg::Memory::destroy_at(m_impl);
        TSG_FREE(m_impl, sizeof(CardConnectionImpl));
        m_impl = nullptr;
    }
    return 0;
}

void CardConnection::swap(CardConnection &o) {
    if (m_impl != nullptr && o.m_impl != nullptr) {
        std::swap(m_impl, o.m_impl);
    }
}

int32_t CardConnection::connect() {
    if (m_impl->is_connected) {
        return 0;
    }

    SCARDHANDLE card_handle = 0;
    DWORD active_protocol;
    LONG rv = SCardConnect(m_impl->context, m_impl->terminal.name.c_str(), SCARD_SHARE_SHARED,
                           SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &card_handle, &active_protocol);
    if (rv != SCARD_S_SUCCESS) {
        if (rv == SCARD_W_REMOVED_CARD) {
            std::cerr << "ERROR - winscard: Card removed" << std::endl;
        } else {
            std::cerr << "ERROR - winscard: " << rv << std::endl;
        }
        return -1;
    }
    m_impl->card_handle = card_handle;
    m_impl->is_connected = true;

    impl_update_atr_bytes(m_impl);
    impl_update_send_pci(m_impl, active_protocol);

    return 0;
}

int32_t CardConnection::reconnect(ResetType reset_type) {

    DWORD active_protocol;
    LONG rv;

    DWORD initialization = SCARD_RESET_CARD;
    if (reset_type == ResetType::reset_type_cold) {
        initialization = SCARD_UNPOWER_CARD;
    }

    rv = SCardReconnect(m_impl->card_handle, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, initialization,
                        &active_protocol);
    if (rv != SCARD_S_SUCCESS) {
        if (rv == SCARD_W_REMOVED_CARD) {
            std::cerr << "ERROR - winscard: Card removed" << std::endl;
        } else {
            std::cerr << "ERROR - winscard: " << rv << std::endl;
        }
        return -1;
    }

    impl_update_atr_bytes(m_impl);
    impl_update_send_pci(m_impl, active_protocol);

    return 0;
}

int32_t CardConnection::disconnect() {
    LONG rv = SCardDisconnect(m_impl->card_handle, SCARD_LEAVE_CARD);
    if (rv != SCARD_S_SUCCESS) {
        std::cerr << "ERROR - winscard: " << rv << std::endl;
        return -1;
    }

    m_impl->is_connected = false;
    return 0;
}

ResponseAPDU CardConnection::transmit(CommandAPDU &capdu) {
    std::cout << "[TRACE] - ";
    log_hexstring_of(capdu);

    ResponseAPDU rapdu = impl_transmit(m_impl, capdu);
    std::cout << "[TRACE] - ";
    log_hexstring_of(rapdu);

    if (rapdu.get_sw1() == StatusWord::wrong_length_le) {
        capdu.back() = rapdu.get_sw2();
        std::cout << "[TRACE] - ";
        log_hexstring_of(capdu);
        rapdu = impl_transmit(m_impl, capdu);
        std::cout << "[TRACE] - ";
        log_hexstring_of(rapdu);
    } else if (rapdu.get_sw1() == StatusWord::response_bytes_still_available) {
        CommandAPDU get_response_bytes_capdu("80C00000");
        get_response_bytes_capdu.push_back(rapdu.get_sw2());
        std::cout << "[TRACE] - ";
        log_hexstring_of(get_response_bytes_capdu);
        rapdu = impl_transmit(m_impl, get_response_bytes_capdu);
        std::cout << "[TRACE] - ";
        log_hexstring_of(rapdu);
    }

    return rapdu;
}

ATR CardConnection::get_atr() { return m_impl->atr_bytes; }

CardConnection::CommunicationProtocol CardConnection::get_communication_protocol() { return m_impl->protocol; }

std::string CardConnection::get_terminal_name() { return (m_impl == nullptr ? "" : m_impl->terminal.name); }

bool CardConnection::is_connected() const { return (m_impl == nullptr ? false : m_impl->is_connected); }

bool CardConnection::is_valid() const { return m_impl == nullptr ? false : true; }

} // namespace smartcard
} // namespace tsg
