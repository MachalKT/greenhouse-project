idf_component_register(SRCS "main.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES application
                            hardware
                            software
                            network
                            esp-aws-iot)

target_add_binary_data(${COMPONENT_LIB} "../../certs/amazonRootCA1_pem" TEXT)
target_add_binary_data(${COMPONENT_LIB} "../../certs/certificate_pem_crt" TEXT)
target_add_binary_data(${COMPONENT_LIB} "../../certs/private_pem_key" TEXT)