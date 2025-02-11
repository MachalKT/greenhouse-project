# Greenhouse Project

## Project Description

A plant monitoring system that collects environmental data such as temperature and air humidity, with future capabilities to send telemetry data to AWS.

## Features

- **Monitoring**: Tracks temperature, humidity, and light levels.
- **Telemetry Transmission**: Future capability to send telemetry data from the main controller to the hub and from the hub to AWS.

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

2. **Clone repository**
```bash
   git clone https://github.com/MachalKT/greenhouse-project.git
   cd greenhouse-project
```
3. **Building the project**
- Greenhouse controller
```bash
   idf.py build -C greenhouse-controller
```
- Hub
```bash
   idf.py build -C hub
```

4. **Flash device**
- Greenhouse controller
```bash
   idf.py -C greenhouse-controller -p PORT flash
```
-  Hub
```bash
   idf.py -C hub -p PORT flash
```

## Project Structure
```
greenhouse-project/
│-- application/             # Application logic
│-- common/                  # Shared utilities (e.g., data types, helper functions)
│-- components/              # Drivers for components used on the PCB board
│-- core/                    # Microcontroller-specific drivers (GPIO, timers, FreeRTOS)
│-- greenhouse-controller/   # Main controller firmware
│-- hub/                     # Hub firmware
```