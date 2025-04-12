# Greenhouse Project

## Project Description

A plant monitoring system that collects environmental data such as temperature and air humidity.
The system consists of two devices:

- **Greenhouse Controller** – measures temperature and humidity inside the greenhouse and sends the data to the Hub.

- **Hub** – connects to a Wi-Fi network and to AWS IoT Core. It receives telemetry data from the Greenhouse Controller and forwards it to AWS.

## Features

- **Monitoring:** Tracks temperature and humidity.
- **Wi-Fi Connectivity:** Connects to Wi-Fi network to enable cloud communication.
- **AWS Integration:** Connects to AWS IoT Core using preconfigured certificates and credentials.
- **Telemetry Transmission:** Sends telemetry data from the greenhouse controller to the hub, and from the hub to AWS.

## System Requirements

- C++17
- CMake
- esp-idf Framework

## Installation

1. **Install ESP-IDF**  
    Follow the [ESP-IDF installation guide](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html#installation) for your operating system.  
   
On **Linux**, before building the project, configure the ESP-IDF environment:
   ```bash
   . $HOME/esp/esp-idf/export.sh
   ```

2. **Clone the repository**
```bash
   git clone https://github.com/MachalKT/greenhouse-project.git
   cd greenhouse-project
```

3. **Update submodule**
```bash
   git submodule update --init --recursive
```

Make sure the `esp-aws-iot` submodule is set to version `release/v3.1.x`

4. **Building the project**
- Greenhouse controller
```bash
   idf.py build -C greenhouse-controller
```
- Hub
```bash
   idf.py build -C hub
```

5. **Flash device**
- Greenhouse controller
```bash
   idf.py -C greenhouse-controller -p PORT flash
```
-  Hub
```bash
   idf.py -C hub -p PORT flash
```

## First-Time Setup

### **Storing Required Values in NVS**

The system retrieves Wi-Fi credentials (SSID and password), AWS client ID, and host URL (domain or endpoint) from the ESP32's Non-Volatile Storage (NVS).  
If these values are not found, the hub will not connect to the Wi-Fi network or AWS.

```cpp
// ...

storage::hw::NvsStore storage{"storage"};

std::string ssid{"yourSSID"};
storage.setString(def::key::wifi::STA_SSID, ssid);

std::string password{"yourPassword"};
storage.setString(def::key::wifi::STA_PASSWORD, password);

std::string awsClientId{"yourClientId"};
storage.setString(def::key::aws::CLIENT_ID, awsClientId);

std::string awsHostUrl{"yourHostUrl"};
storage.setString(def::key::aws::HOST_URL, awsHostUrl);

storage.save();

// ...
```

### **AWS Certificates**

The system requires AWS certificates (Amazon Root CA 1, client certificate, and private key), which should be saved in the `certs` directory.
Rename the files as follows (without file extensions):

- `AmazonRootCA1.pem`         -> `amazonRootCA1_pem`  
- `xxxx-certificate.pem.crt`  -> `certificate_pem_crt`  
- `xxxx-private.pem.key`      -> `private_pem_key`  

```bash
   mkdir certs
   cd certs
   # Add your certificate files here
```


## Project Structure
```
greenhouse-project/
│-- application/             # Application logic
│-- common/                  # Shared utilities (e.g., data types, helper functions)
│-- components/              # Drivers for components used on the PCB
│-- core/                    # Microcontroller-specific drivers (GPIO, timers, FreeRTOS)
│-- esp-aws-iot/             # Submodule including AWS IoT library
│-- greenhouse-controller/   # Greenhouse controller firmware
│-- hub/                     # Hub firmware
│-- packet/                  # Data types and utilities for data serialization
```
