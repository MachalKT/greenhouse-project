#pragma once

#include "types.hpp"
#include <string>

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
        std::string ssid;
        std::string password;
        AuthenticateMode authenticateMode;
        uint8_t maxStaConnected;
        uint8_t channel;
    };

    /**
     * @brief Wi-fi Sta mode Configuration.
     */
    struct StaConfig {
        std::string ssid;
        std::string password;
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
    static Wifi& getInstance();

    /**
     * @brief Initializes the Wi-Fi module.
     * @note NVS must be initialized before calling init Wi-Fi
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
     * @param arg Argument.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error
    setWifiEventHandler(common::event::HandlerFunction eventHandler,
                        common::Argument arg);

    /**
     * @brief Sets the event handler for IP events.
     *
     * @param eventHandler Function pointer to the event handler.
     * @param arg Argument.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error setIpEventHandler(common::event::HandlerFunction eventHandler,
                                    common::Argument arg);

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
     * @brief Scan all available APs.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     *   - common::Error::INVALID_STATE: Wi-Fi not set to Sta mode.
     */
    common::Error scan();

    /**
     * @brief Check if Wi-Fi is in STA mode.
     *
     * @return
     *   - true: Wi-Fi is in STA or APSTA mode.
     *   - false: Wi-Fi is in AP mode.
     */
    bool isStaMode() const;

    /**
     * @brief Check if Wi-Fi is in AP mode.
     *
     * @return
     *   - true: Wi-Fi is in AP or APSTA mode.
     *   - false: Wi-Fi is in STA mode.
     */
    bool isApMode() const;

  private:
    Wifi() = default;
    ~Wifi() = default;

    Wifi(const Wifi&) = delete;
    Wifi& operator=(const Wifi&) = delete;
    Wifi(Wifi&&) = delete;
    Wifi& operator=(Wifi&&) = delete;

    common::Error preInit_();

    common::Error setMode_(const Mode mode);

    common::Error setConfig_(const Config config);

    common::Error setStaConfig_(const StaConfig config);

    common::Error setApConfig_(const ApConfig config);

    static constexpr int32_t EVENT_ANY_ID{-1};
    Mode mode_{Mode::NONE};
};

} // namespace net
