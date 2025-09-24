#ifndef TSG_SMARTCARD_CARD_CONNECTION_HPP
#define TSG_SMARTCARD_CARD_CONNECTION_HPP

#include "atr.hpp"
#include "command_apdu.hpp"
#include "response_apdu.hpp"

namespace tsg {
namespace smartcard {

struct CardConnectionImpl;
struct CardConnectCI;

struct CardConnectionInit {};

class CardConnection {
  public:
    enum ResetType {
        reset_type_none,
        reset_type_warm,
        reset_type_cold,
    };

    enum CommunicationProtocol {
        com_protocol_t_none,
        com_protocol_t_0,
        com_protocol_t_1,
    };

  public:
    CardConnection();

    ~CardConnection();

    int32_t initialize(CardConnectCI &ci);

    int32_t cleanup();

    void swap(CardConnection &o);

    int32_t connect();

    int32_t reconnect(ResetType reset_type);

    int32_t disconnect();

    ResponseAPDU transmit(CommandAPDU &capdu);

    ATR get_atr();

    CommunicationProtocol get_communication_protocol();

    std::string get_terminal_name();

    bool is_connected() const;

    bool is_valid() const;

  private:
    CardConnectionImpl *m_impl;
};

} // namespace smartcard
} // namespace tsg

#endif // TSG_SMARTCARD_CARD_CONNECTION_HPP