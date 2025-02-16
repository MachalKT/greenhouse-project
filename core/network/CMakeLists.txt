set(SRC src/wifi.cpp)

idf_component_register(
    SRCS ${SRC}
    INCLUDE_DIRS inc
    REQUIRES common esp_event esp_netif esp_wifi nvs_flash
)
