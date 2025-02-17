#pragma once

#include "interfaces/itimer.hpp"
#include "types.hpp"
#include "utils.hpp"
#include "wifi.hpp"
#include <string_view>

namespace app {

/**
 * @class WifiController
 * @brief Handles Wi-Fi operations such as initialization, connection
 * management, and event handling.
 */
class WifiController {
  public:
    /**
     * @brief Constructs a WifiController instance.
     *
     * @param reconnectTimer Timer used for handling reconnection
     * attempts.
     */
    WifiController(timer::ITimer& reconnectTimer);

    /**
     * @brief Initializes the Wi-Fi controller with the provided
     *configuration.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error init();

    /**
     * @brief Starts the Wi-Fi connection process.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error start();

    /**
     * @brief Stops the Wi-Fi connection.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error stop();

  private:
    /**
     * @brief Handles Wi-Fi events.
     *
     * @param arg Generic argument.
     * @param eventbase Base event type.
     * @param eventId Event identifier.
     * @param eventData Data associated with the event.
     */
    static void wifiEventHandler_(common::Argument arg,
                                  common::event::Base eventbase,
                                  common::event::Id eventId,
                                  common::event::Data eventData);

    /**
     * @brief Handles IP events.
     *
     * @param arg Generic argument.
     * @param eventbase Base event type.
     * @param eventId Event identifier.
     * @param eventData Data associated with the event.
     */
    static void ipEventHandler_(common::Argument arg,
                                common::event::Base eventbase,
                                common::event::Id eventId,
                                common::event::Data eventData);

    /**
     * @brief Retrieves the Wi-Fi instance.
     *
     * @return Reference to the net::Wifi instance.
     */
    net::Wifi& getWifiInstance_();

    /**
     * @brief Attempts to reconnect to the Wi-Fi network.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error reconnect_();

    /**
     * @brief Displays the list of available access points.
     */
    void showAvailableApList_();

    /**
     * @brief Displays the list of connected stations.
     */
    void showConnectedStaList_();

    /**
     * @brief Display recive ip.
     *
     * @param eventData Event data containing ip.
     */
    void showIp_(common::event::Data eventData);

    /**
     * @brief Displays information about the connected station.
     *
     * @param eventData Event data containing the station connected information.
     */
    void showConnectedStaInfo_(common::event::Data eventData);

    /**
     * @brief Displays information about the disconnected station.
     *
     * @param eventData Event data containing the station disconnected
     * information.
     */
    void showDisconnectedStaInfo_(common::event::Data eventData);

    /**
     * @brief Converts authentication mode to string representation.
     *
     * @param authenticateMode Authentication mode.
     * @return String representation of the authentication mode.
     */
    std::string_view toString_(net::Wifi::AuthenticateMode authenticateMode);

    static constexpr common::Time RECONNECT_TIME_US{
        common::utils::msToUs<common::Time, common::Time>(
            common::utils::sToMs<common::Time, uint8_t>(60))};
    static constexpr size_t AP_LIST_SIZE{5};
    static constexpr int NO_STA_IS_CONNECTED{0};
    static constexpr uint16_t NO_GET_AVAILABLE_AP{0};
    static constexpr uint8_t MAX_RECONNECT_ATTEMPTS{5};
    timer::ITimer& reconnectTimer_;
    uint8_t reconnectAttempt_{0};
};

} // namespace app
