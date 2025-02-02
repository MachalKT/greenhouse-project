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
   Before building the project, set up the ESP-IDF environment:
   ```bash
   . $HOME/esp/esp-idf/export.sh
   ```

2. **Clone repository**
```bash
   git clone https://github.com/MachalKT/greenhouse-project.git
   cd greenhouse-project
```
3. **Building the project**
```bash
   idf.py build
```
4. **Flash device**
```bash
   idf.py -p PORT flash
```

## Project Structure

- application/ – Application logic
- common/ – Shared utilities, functions, types
- components/ – Drivers for components used on the PCB board
- core/ – Drivers for microcontroller peripherals (e.g., timers, GPIO) and FreeRTOS system functions.
- main/ – Application entry point