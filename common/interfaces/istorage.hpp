#pragma once

#include "types.hpp"
#include <any>
#include <string_view>

namespace storage {
class IStorage {
  public:
    virtual common::Error setItem(const std::string_view& key,
                                  const uint8_t& item) = 0;

    virtual common::Error getItem(const std::string_view& key,
                                  uint8_t& item) = 0;

    virtual common::Error setString(const std::string_view& key,
                                    const std::string& string) = 0;

    virtual common::Error getString(const std::string_view& key,
                                    std::string& string) = 0;

    virtual common::Error save() = 0;
};
} // namespace storage
