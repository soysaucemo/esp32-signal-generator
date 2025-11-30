# Refactored ESP32 Signal Generator

This repository contains a refactored version of the open-source signal generator built for the ESP32 family of microcontrollers. The refactored code improves maintainability, error handling, and code organization.

## Features

- Support for sine and square waves
- Web-based UI served directly from the device via WiFi
- WiFi connectivity with static IP configuration
- Configuration persistence using EEPROM
- Improved error handling and validation

## Prerequisites

### Hardware

The [ESP32 DevKitC V4](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/modules-and-boards.html#esp32-devkitc-v4) was used for development and testing.

### Software

The [Arduino](https://www.arduino.cc/en/Main/Software) software version 1.8.15 with the [esp32 board extension](https://github.com/espressif/arduino-esp32) version 1.0.6 was used for building and uploading. To upload the static web files to the device, [ESP32 Filesystem Uploader](https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/) was used.

## Code Structure

The refactored code follows a modular design with the following components:

- **SigGen_refactored.ino**: Main application file with improved structure and error handling
- **DAC_Module**: Handles sine wave generation using ESP32's built-in DAC
- **PWM_Module**: Handles square wave generation using ESP32's LEDC peripheral
- **Settings_Module**: Manages configuration storage in EEPROM
- **WiFi_Module**: Handles WiFi connectivity and access point mode

## Improvements in Refactored Code

1. **Better Error Handling**: Added validation for all input parameters
2. **Improved Documentation**: Added comprehensive Doxygen-style comments
3. **Enhanced Code Organization**: Clearer function structure and separation of concerns
4. **Safer Memory Management**: Added bounds checking and null pointer validation
5. **Better Debugging**: More detailed serial output for debugging purposes

## Building

Build and upload using Arduino. 
Since the static files of the website are being served from a simple on-device filesystem, it is necessary to format the flash storage once prior to first use:

1. Uncomment `#define FORMAT_FILESYSTEM` in *SigGen_refactored.ino*
2. Build and upload
3. Comment `#define FORMAT_FILESYSTEM` in *SigGen_refactored.ino*
4. Build and upload again
5. [Optional] If you changed the front-end code, run parcel
6. In Arduino: **Tools > ESP32 Sketch Data Upload** (this copies the contents of */data* to the µC's flash memory)

## First run

The software is configured to connect to a WiFi network after booting, for which the configuration is stored in the µC's EEPROM. At first boot, that configuration is uninitialized and thus, connecting to WiFi will fail.

### AP Mode

Whenever a WiFi connection could not be established, the device enters the access-point (AP) mode, acting as a WiFi AP with `SigGen` as its SSID. The AP mode can be forced by uncommenting `#define FORCE_AP` in *SigGen_refactored.ino*.

### WiFi Configuration

In AP mode, the device's IP address is `192.168.4.1`. To configure the device for your WiFi, navigate to `http://<DEVICE_IP>`, select `Configuration` in the header menu and set the SSID and enter a password. You can optionally assign a static IP address to the device, as well as configure the gateway and subnet mask.

After writing the configuration to EEPROM, you need to restart the device to apply the changes. This can be achieved either via the reset button on the device itself or by navigating to `http://<DEVICE_IP>/reboot`.

## Usage

Visit `http://<DEVICE_IP>` to bring up the signal generator's UI, which is separated into two tabs (one for each waveform).

### Square Waves

* **Channel** [0-7]: Selects the PWM channel.
* **Timer** [0-3]: Selects the PWM timer (each timer can be configured independently and multiple channels can be attached to each).
* **Output GPIO Pin** [PIN#]: Selects the GPIO pin used to output the configured signal for the selected channel.
* **Resolution** [1-20]bit: Sets the resolution of the selected timer (in bits).
* **Highspeed** [True/False]: Defines whether the selected timer runs in highspeed mode.
* **Frequency** [0-40M]Hz: Sets the frequency of the selected timer (in Hz). The maximum frequency depends on the selected resolution.
* **Duty** [0-100]%: Sets the duty cycle (in %).
* **Phase** [0-360]°: Sets the phase offset (in °).

### Sine Waves

* **Channel** [1/2]: Selects the DAC channel. The ESP32 has two DAC channels (1 and 2) connected to GPIO pins 25 and 26, respectively.
* **Clock divider** [0-7]: Selects the divider for the selected channel from the DAC clock, which runs at 135Hz.
* **Invert** [Off/All/MSB/All but MSB]: Sets the inversion mode of the selected channel (MSB for non-inverted sine waves).
* **Frequency** [17-135k]Hz: Sets the frequency of the selected channel (in Hz). The minimum and maximum frequency depend on the selected clock divider (min = 135/(clk_div + 1); max ≈ 135k/(clk_div + 1)). The DAC can still produce a signal above the maximum frequency, but it will become progressively distorted.
* **Phase** [0-360]°: Sets the phase offset (in °). This cuts off the sine wave's tips.
* **Amplitude** [⅛/¼/½/1]: Sets the amplitude to a fraction of the maximum (5V).

## Technical Details

### Front-End 

The front-end is built using [Materialize](https://materializecss.com/). The number of static files has been reduced to its practical minimum due to limitations of the webserver. As a consequence, all image data is defined inline and only the CSS file is served separately from the HTML.

### Tools

[Visual Studio Code](https://code.visualstudio.com/) version 1.43.1 with the [Arduino extension](https://github.com/Microsoft/vscode-arduino.git) was used for development.

## Web API

Documentation pending (monitor HTTP requests from the UI for reference).

## Future Work

* Add more waveforms (triangle, sawtooth, arbitrary)
* Improve WiFi connectivity (deal with disconnects)
* Bluetooth connectability

## Collaboration

If you find a bug, have suggestions or are interested in extending this project, drop me a line or issue a pull request.