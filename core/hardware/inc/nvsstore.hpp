#pragma once

#include "istorage.hpp"
#include "types.hpp"
#include <string_view>

namespace storage {
namespace hw {

/**
 * @class NvsStore
 * @brief Implementation Non-Volatile Storage (NVS)
 */
class NvsStore final : public IStorage {
  public:
    /**
     * @brief Constructor for NvsStore.
     *
     * @param nvsNamespace The namespace used for storing values in NVS.
     */
    NvsStore(const std::string_view nvsNamespace);

    /**
     * @brief Initializes the NVS storage system.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    static common::Error init();

    /**
     * @brief Erases all stored data in NVS.
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    static common::Error erease();

    /**
     * @brief Stores an 8-bit integer value in NVS.
     *
     * @param key The key associated with the value.
     * @param item The 8-bit integer value to store.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error setItem(const std::string_view& key,
                          const uint8_t& item) override;

    /**
     * @brief Retrieves an 8-bit integer value from NVS.
     *
     * @param key The key associated with the value.
     * @param item Reference to a variable where the retrieved value will be
     * stored.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error getItem(const std::string_view& key, uint8_t& item) override;

    /**
     * @brief Stores a string in NVS.
     *
     * @param key The key associated with the string.
     * @param string The string value to store.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error setString(const std::string_view& key,
                            const std::string& string) override;

    /**
     * @brief Retrieves a string from NVS.
     *
     * @param key The key associated with the string.
     * @param string Reference to a variable where the retrieved string will be
     * stored.
     * @param size The maximum size of the string to retrieve.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error getString(const std::string_view& key,
                            std::string& string) override;

    /**
     * @brief Saves any pending changes to NVS.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error save() override;

  private:
    // 's' stands for "size" — shortened due to the 15-character maximum key
    // length limitation.
    static constexpr std::string_view STRING_SIZE_KEY_SUFFIX{"s"};
    static bool isNvsInitialized_;
    std::string_view namespace_;
};

} // namespace hw
} // namespace storage
