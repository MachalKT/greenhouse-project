#include "awspacket.hpp"
#include "cJSON.h"
#include <cstring>

namespace packet {
namespace aws {

Telemetry::Telemetry(common::Telemetry telemetry) : telemetry_{telemetry} {}

common::Error Telemetry::serializeToJson(char* buffer,
                                         const size_t bufferLength) {
  if (buffer == nullptr || bufferLength == 0) {
    return common::Error::INVALID_ARG;
  }

  cJSON* root = cJSON_CreateObject();
  if (root == nullptr) {
    return common::Error::FAIL;
  }

  cJSON* telemetry = cJSON_CreateObject();
  if (root == nullptr) {
    return common::Error::FAIL;
  }

  cJSON_AddNumberToObject(telemetry, "temperature", telemetry_.temperatureC);
  cJSON_AddNumberToObject(telemetry, "humidity", telemetry_.humidityRh);
  cJSON_AddItemToObject(root, "telemetry", telemetry);

  char* jsonStr = cJSON_PrintUnformatted(root);
  if (jsonStr == nullptr) {
    cJSON_Delete(root);
    return common::Error::FAIL;
  }

  size_t jsonLength = std::strlen(jsonStr);
  if (jsonLength + 1 > bufferLength) {
    delete[] jsonStr;
    cJSON_Delete(root);
    return common::Error::FAIL;
  }

  std::strncpy(buffer, jsonStr, bufferLength);

  delete[] jsonStr;
  cJSON_Delete(root);

  return common::Error::OK;
}

} // namespace aws
} // namespace packet
