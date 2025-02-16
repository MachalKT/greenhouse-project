#pragma once

#include "types.hpp"
#include <string_view>

namespace net {

/**
 * @class Wifi
 * @brief Class for managing Wi-Fi configuration.
 */
class Wifi {
  public:
    /**
     * @brief Wi-Fi operating modes.
     */
    enum class Mode : uint8_t { NONE = 0, STA, AP, APSTA };

    /**
     * @brief Wi-Fi authentication modes.
     */
    enum class AuthenticateMode : uint8_t {
      OPEN = 0,
      WEP,
      WPA_PSK,
      WPA2_PSK,
      WPA_WPA2_PSK,
      ENTERPRISE,
      WPA2_ENTERPRISE = ENTERPRISE, // Wi-Fi EAP security
      WPA3_PSK,
      WPA2_WPA3_PSK,
      WAPI_PSK,
      OWE,
      WPA3_ENT_192,
      DPP = 13,
      WPA3_ENTERPRISE,
      WPA2_WPA3_ENTERPRISE
    };

    /**
     * @brief Wi-fi Ap mode Configuration.
     */
    struct ApConfig {
        std::string_view ssid;
        std::string_view password;
        AuthenticateMode authenticateMode;
        uint8_t maxStaConnected;
        uint8_t channel;
    };

    /**
     * @brief Wi-fi Sta mode Configuration.
     */
    struct StaConfig {
        std::string_view ssid;
        std::string_view password;
        AuthenticateMode authenticateMode;
    };

    /**
     * @brief Wi-Fi configuration.
     */
    struct Config {
        StaConfig sta;
        ApConfig ap;
        Mode mode;
    };

    /**
     * @brief Retrieves the singleton instance of the Wifi class.
     */
    static Wifi& getIstance();

    /**
     * @brief Initializes the Wi-Fi module.
     *
     * @param config Wi-Fi configuration settings.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error init(const Config config);

    /**
     * @brief Deinitializes the Wi-Fi module.
     *
     * @param config Wi-Fi configuration settings.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error deinit();

    /**
     * @brief Restores the given Wi-Fi configuration.
     *
     * @param config config Wi-Fi configuration to restore.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     *   - common::Error::INVALID_STATE: Wi-Fi is not initialized.
     */
    common::Error restore(const Config config);

    /**
     * @brief Sets the event handler for Wi-Fi events.
     *
     * @param eventHandler Function pointer to the event handler.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error
    setWifiEventHandler(common::event::HandlerFunction eventHandler);

    /**
     * @brief Sets the event handler for IP events.
     *
     * @param eventHandler Function pointer to the event handler.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error
    setIpEventHandler(common::event::HandlerFunction eventHandler);

    /**
     * @brief Starts the Wi-Fi module.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error start();

    /**
     * @brief Stops the Wi-Fi module.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error stop();

    /**
     * @brief Connects to a Wi-Fi network.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     *   - common::Error::INVALID_STATE: Wi-Fi not set to Sta mode.
     */
    common::Error connect();

    /**
     * @brief Disconnects from a Wi-Fi network.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     *   - common::Error::INVALID_STATE: Wi-Fi not set to Sta mode.
     */
    common::Error disconnect();

    /**
     * @brief Displays the list of available access points.
     */
    void showAvailableApList();

    /**
     * @brief Displays the list of connected stations.
     */
    void showConnectedStaList();

    /**
     * @brief Displays the assigned IP address.
     *
     * @param eventData Event data containing the IP address information.
     */
    void showIp(common::event::Data eventData);

    /**
     * @brief Displays information about the connected station.
     *
     * @param eventData Event data containing the station connected information.
     */
    void showConnectedStaInfo(common::event::Data eventData);

    /**
     * @brief Displays information about the disconnected station.
     *
     * @param eventData Event data containing the station disconnected
     * information.
     */
    void showDisconnectedStaInfo(common::event::Data eventData);

  private:
    Wifi() = default;
    ~Wifi() = default;

    Wifi(const Wifi&) = delete;
    Wifi& operator=(const Wifi&) = delete;
    Wifi(Wifi&&) = delete;
    Wifi& operator=(Wifi&&) = delete;

    common::Error preInit_();

    common::Error nvsInit_();

    common::Error setMode_(const Mode mode);

    common::Error setConfig_(const Config config);

    common::Error setStaConfig_(const StaConfig config);

    common::Error setApConfig_(const ApConfig config);

    bool isStaMode_() const;

    bool isApMode_() const;

    std::string_view toString_(AuthenticateMode authenticateMode);

    static constexpr size_t SCAN_LIST_SIZE{10};
    static constexpr int NO_STA_IS_CONNECTED{0};
    static constexpr int32_t EVENT_ANY_ID{-1};
    Mode mode_{Mode::NONE};
};

} // namespace net
