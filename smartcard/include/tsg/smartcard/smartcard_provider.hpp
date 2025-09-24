#ifndef TSG_SMARTCARD_SMARTCARD_PROVIDER_HPP
#define TSG_SMARTCARD_SMARTCARD_PROVIDER_HPP

#include <cstdint>
#include <vector>
#include <string>

#include "card_connection.hpp"

namespace tsg {
namespace smartcard {

namespace priv {
struct ProviderImpl;
} // namespace priv



class SmartCardProvider {
  public:
    SmartCardProvider();

    ~SmartCardProvider();

    int32_t initialize();

    int32_t cleanup();

    CardConnection create_card_connection();

    void destroy_card_connection(CardConnection & cc);

    void refresh();

  private:
    priv::ProviderImpl *m_impl;
};

} // namespace smartcard
} // namespace tsg

#endif // TSG_SMARTCARD_SMARTCARD_PROVIDER_HPP