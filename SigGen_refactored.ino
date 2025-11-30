/*
MIT License

Copyright (c) 2020 Benjamin von Deschwanden
Copyright (c) 2025 AI Assistant - Refactored version

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Settings_Module.h"
#include "WiFi_Module.h"

#include <WebServer.h>

#include "DAC_Module.h"
#include "PWM_Module.h"

#include "driver/dac.h"
#include "driver/ledc.h"

#define FILESYSTEM SPIFFS
#define DBG_OUTPUT_PORT Serial

#if FILESYSTEM == FFat
    #include <FFat.h>
#endif
#if FILESYSTEM == SPIFFS
    #include <SPIFFS.h>
#endif

// #define FORMAT_FILESYSTEM  // Uncomment to format the file system (only necessary once before first data upload)
// #define SHOW_PASSWORDS  // Uncomment to show passwords in debug serial output
// #define FORCE_AP  // Uncomment to force device into access-point mode at startup (don't try to connect to WiFi)

// Application constants
#define NUM_SETTINGS 5
#define SSID_SETTING 0
#define PASSWORD_SETTING 1
#define LOCAL_IP_SETTING 2
#define GATEWAY_SETTING 3
#define SUBNET_SETTING 4

// Global objects
WiFi_Module* wifi = nullptr;
WebServer *server = nullptr;
DAC_Module *dac = nullptr;
PWM_Module *pwm = nullptr;
Settings_Module* settings = nullptr;

// Configuration variables
String ssid;
String password;
IPAddress local_ip;
IPAddress gateway;
IPAddress subnet;

// Function declarations
void setupWebServer();
void loadSettings();
void resetDefaults();
void handleReboot();
void handleSetup();
void handleStop();
void handleConfig();
void handleNotFound();
bool handleFileRead(String path);
String getContentType(String filename);
bool exists(String path);

// Get MIME content type from file extension
String getContentType(String filename) {
    if (server->hasArg("download")) {
        return "application/octet-stream";
    } else if (filename.endsWith(".htm")) {
        return "text/html";
    } else if (filename.endsWith(".html")) {
        return "text/html";
    } else if (filename.endsWith(".css")) {
        return "text/css";
    } else if (filename.endsWith(".js")) {
        return "application/javascript";
    } else if (filename.endsWith(".png")) {
        return "image/png";
    } else if (filename.endsWith(".gif")) {
        return "image/gif";
    } else if (filename.endsWith(".jpg")) {
        return "image/jpeg";
    } else if (filename.endsWith(".ico")) {
        return "image/x-icon";
    } else if (filename.endsWith(".svg")) {
        return "image/svg+xml";
    } else if (filename.endsWith(".xml")) {
        return "text/xml";
    } else if (filename.endsWith(".pdf")) {
        return "application/x-pdf";
    } else if (filename.endsWith(".zip")) {
        return "application/x-zip";
    } else if (filename.endsWith(".gz")) {
        return "application/x-gzip";
    }
    return "text/plain";
}

// Check if a given file exists
bool exists(String path) {
    bool yes = false;
    File file = FILESYSTEM.open(path, "r");
    if (!file.isDirectory()) {
        yes = true;
    }
    file.close();
    return yes;
}

// This method is invoked to handle GET requests on static files
bool handleFileRead(String path) {
    if (path.endsWith("/")) {
        path += "index.html";
    }
    String contentType = getContentType(path);
    if (exists(path)) {
        File file = FILESYSTEM.open(path, "r");
        size_t sent = server->streamFile(file, contentType);
        file.close();
        return true;
    }
    return false;
}

// This method is invoked to handle a reboot command
void handleReboot() {
    server->send(200, "text/plain", "Restarting...");
    delay(1000); // Give time for response to be sent
    ESP.restart();
}

// This method is invoked to handle signal generator setup
void handleSetup() {
    // Shared parameters
    uint32_t phase = 0;
    uint32_t frequency = 1000;

    // PWM-specific parameters
    ledc_timer_t timer_num = LEDC_TIMER_0;
    ledc_channel_t pwm_channel = LEDC_CHANNEL_0;
    ledc_timer_bit_t resolution = LEDC_TIMER_10_BIT;
    bool highspeed = true;
    uint32_t duty = 512;
    uint32_t out_pin = 25;

    // DAC-specific parameters
    dac_channel_t dac_channel = DAC_CHANNEL_1;
    uint32_t clk_div = 0;
    uint32_t scale = 0;
    uint32_t invert = 2;

    String type = "square";
    
    // Parse all parameters from request
    for (uint8_t i = 0; i < server->args(); i++) {
        String argName = server->argName(i);
        String argValue = server->arg(i);
        
        // String parameters
        if (argName == "type") { 
            type = argValue; 
            continue; 
        }

        // LEDC timer parameters (converted from string)
        if (argName == "resolution") { 
            int val = atoi(argValue.c_str());
            if (val >= LEDC_TIMER_1_BIT && val <= LEDC_TIMER_14_BIT) {
                resolution = (ledc_timer_bit_t)val;
            } else {
                server->send(400, "text/plain", "Invalid resolution value: " + argValue);
                return;
            }
            continue; 
        }
        if (argName == "timer_num") { 
            int val = atoi(argValue.c_str());
            if (val >= LEDC_TIMER_0 && val <= LEDC_TIMER_3) {
                timer_num = (ledc_timer_t)val;
            } else {
                server->send(400, "text/plain", "Invalid timer_num value: " + argValue);
                return;
            }
            continue; 
        }
        if (argName == "pwm_channel") { 
            int val = atoi(argValue.c_str());
            if (val >= LEDC_CHANNEL_0 && val <= LEDC_CHANNEL_7) {
                pwm_channel = (ledc_channel_t)val;
            } else {
                server->send(400, "text/plain", "Invalid pwm_channel value: " + argValue);
                return;
            }
            continue; 
        }
        if (argName == "dac_channel") { 
            int val = atoi(argValue.c_str());
            if (val == 1 || val == 2) {
                dac_channel = (dac_channel_t)val;
            } else {
                server->send(400, "text/plain", "Invalid dac_channel value: " + argValue);
                return;
            }
            continue; 
        }

        // uint32_t parameters
        if (argName == "clk_div") { 
            clk_div = strtoul(argValue.c_str(), NULL, 10); 
            continue; 
        }
        if (argName == "frequency") { 
            frequency = strtoul(argValue.c_str(), NULL, 10); 
            continue; 
        }
        if (argName == "duty") { 
            duty = strtoul(argValue.c_str(), NULL, 10); 
            continue; 
        }
        if (argName == "phase") { 
            phase = strtoul(argValue.c_str(), NULL, 10); 
            continue; 
        }
        if (argName == "out_pin") { 
            out_pin = strtoul(argValue.c_str(), NULL, 10); 
            continue; 
        }
        if (argName == "scale") { 
            scale = strtoul(argValue.c_str(), NULL, 10); 
            continue; 
        }
        if (argName == "invert") { 
            invert = strtoul(argValue.c_str(), NULL, 10); 
            continue; 
        }

        // bool parameters
        if (argName == "highspeed") { 
            highspeed = (argValue == "true" || argValue == "1"); 
            continue; 
        }

        server->send(400, "text/plain", "Invalid parameter name: " + argName); 
        return;
    }

    // Validate and execute setup based on type
    if (type == "sine") {
        dac->Setup(dac_channel, clk_div, frequency, scale, phase, invert);
        server->send(200, "text/plain", "Sine wave setup successful.");
    } else if (type == "square") {
        pwm->Setup(timer_num, pwm_channel, highspeed, resolution, frequency, duty, phase, out_pin);
        server->send(200, "text/plain", "Square wave setup successful.");
    } else {
        server->send(400, "text/plain", "Invalid type. Supported: sine, square");
    }
}

// This method is invoked to stop a signal generator
void handleStop() {
    String type = "square";
    ledc_channel_t pwm_channel = LEDC_CHANNEL_0;
    dac_channel_t dac_channel = DAC_CHANNEL_1;
    bool highspeed = true;

    for (uint8_t i = 0; i < server->args(); i++) {
        String argName = server->argName(i);
        String argValue = server->arg(i);
        
        if (argName == "type") { 
            type = argValue; 
        } else if (argName == "pwm_channel") { 
            int val = atoi(argValue.c_str());
            if (val >= LEDC_CHANNEL_0 && val <= LEDC_CHANNEL_7) {
                pwm_channel = (ledc_channel_t)val;
            } else {
                server->send(400, "text/plain", "Invalid pwm_channel value: " + argValue);
                return;
            }
        } else if (argName == "dac_channel") { 
            int val = atoi(argValue.c_str());
            if (val == 1 || val == 2) {
                dac_channel = (dac_channel_t)val;
            } else {
                server->send(400, "text/plain", "Invalid dac_channel value: " + argValue);
                return;
            }
        } else if (argName == "highspeed") { 
            highspeed = (argValue == "true" || argValue == "1"); 
        } else {
            server->send(400, "text/plain", "Invalid parameter name: " + argName); 
            return;
        }
    }

    if (type == "sine") {
        dac->Stop(dac_channel);
        server->send(200, "text/plain", "Sine wave stopped successfully.");
    } else if (type == "square") {
        pwm->Stop(pwm_channel, highspeed);
        server->send(200, "text/plain", "Square wave stopped successfully.");
    } else {
        server->send(400, "text/plain", "Invalid type. Supported: sine, square");
    }
}

// This method is invoked to get or set the configuration
void handleConfig() {
    switch (server->method()) {
    case HTTP_GET:
        Serial.println("Getting configuration...");
        server->send(200, "application/json", 
            "{\"ssid\":\"" + ssid + "\","
            "\"local_ip\":\"" + local_ip.toString() + "\","
            "\"gateway\":\"" + gateway.toString() + "\","
            "\"subnet\":\"" + subnet.toString() + "\"}");
        break;
    case HTTP_POST:
        Serial.println("Setting configuration...");
        bool hasError = false;
        String errorMessage = "";

        for (uint8_t i = 0; i < server->args(); i++) {
            String argName = server->argName(i);
            String argValue = server->arg(i);
            
            if (argName == "ssid") {
                ssid = argValue;
                Serial.println("Setting ssid to '" + ssid + "'");
                settings->StoreString(SSID_SETTING, ssid);
            } else if (argName == "password") {
                password = argValue;
                Serial.print("Setting password to '");
                #ifdef SHOW_PASSWORDS
                    Serial.print(password);
                #else
                    Serial.print("****");
                #endif
                Serial.println("'");
                settings->StoreString(PASSWORD_SETTING, password);
            } else if (argName == "local_ip") {
                if (local_ip.fromString(argValue)) {
                    Serial.println("Setting local_ip to '" + local_ip.toString() + "'");
                    settings->StoreIp(LOCAL_IP_SETTING, local_ip);
                } else {
                    errorMessage = "Invalid IP address for local_ip: " + argValue;
                    hasError = true;
                    break;
                }
            } else if (argName == "gateway") {
                if (gateway.fromString(argValue)) {
                    Serial.println("Setting gateway to '" + gateway.toString() + "'");
                    settings->StoreIp(GATEWAY_SETTING, gateway);
                } else {
                    errorMessage = "Invalid IP address for gateway: " + argValue;
                    hasError = true;
                    break;
                }
            } else if (argName == "subnet") {
                if (subnet.fromString(argValue)) {
                    Serial.println("Setting subnet to '" + subnet.toString() + "'");
                    settings->StoreIp(SUBNET_SETTING, subnet);
                } else {
                    errorMessage = "Invalid IP address for subnet: " + argValue;
                    hasError = true;
                    break;
                }
            } else {
                errorMessage = "Invalid parameter: " + argName;
                hasError = true;
                break;
            }
        }
        
        if (hasError) {
            server->send(400, "text/plain", errorMessage);
        } else {
            settings->Commit();
            server->send(200, "text/plain", "Configuration updated.");
        }
        break;
    default:
        server->send(405, "text/plain", "Method not allowed. Use GET or POST.");
        break;
    }
}

// This method is invoked if the requested resource is not found
void handleNotFound() {
    if (!handleFileRead(server->uri())) {
        String message = "File Not Found\n\n";
        message += "URI: ";
        message += server->uri();
        message += "\nMethod: ";
        message += (server->method() == HTTP_GET) ? "GET" : "POST";
        message += "\nArguments: ";
        message += server->args();
        message += "\n";

        for (uint8_t i = 0; i < server->args(); i++) {
            message += " " + server->argName(i) + ": " + server->arg(i) + "\n";
        }

        server->send(404, "text/plain", message);
    }
}

// Initialize the web server and set up the API endpoints
void setupWebServer() {
    server = new WebServer(80);
    server->on("/config", handleConfig);
    server->on("/setup", handleSetup);
    server->on("/stop", handleStop);
    server->on("/reboot", handleReboot);
    server->onNotFound(handleNotFound);
    server->begin();
    Serial.println("Web server started on port 80");
}

// Load configuration from EEPROM
void loadSettings() {
    Serial.println("Loading Settings from ROM...");

    settings->LoadString(SSID_SETTING, &ssid);
    Serial.print("ssid: '");
    Serial.print(ssid);
    Serial.println("'");

    settings->LoadString(PASSWORD_SETTING, &password);
    Serial.print("password: '");
    #ifdef SHOW_PASSWORDS
        Serial.print(password);
    #else
        Serial.print("****");
    #endif
    Serial.println("'");

    settings->LoadIp(LOCAL_IP_SETTING, &local_ip);
    Serial.print("local_ip: '");
    Serial.print(local_ip.toString());
    Serial.println("'");

    settings->LoadIp(GATEWAY_SETTING, &gateway);
    Serial.print("gateway: '");
    Serial.print(gateway.toString());
    Serial.println("'");

    settings->LoadIp(SUBNET_SETTING, &subnet);
    Serial.print("subnet: '");
    Serial.print(subnet.toString());
    Serial.println("'");
}

// Reset configuration defaults and store them in EEPROM
void resetDefaults() {
    ssid = "";
    password = "";
    local_ip.fromString("");
    gateway.fromString("192.168.1.1");
    subnet.fromString("255.255.255.0");
    
    settings->StoreString(SSID_SETTING, ssid);
    settings->StoreString(PASSWORD_SETTING, password);
    settings->StoreIp(LOCAL_IP_SETTING, local_ip);
    settings->StoreIp(GATEWAY_SETTING, gateway);
    settings->StoreIp(SUBNET_SETTING, subnet);

    settings->Commit();
}

void setup() {
    // Initialize serial
    Serial.begin(115200);
    Serial.println();

    // Initialize file system
    #ifdef FORMAT_FILESYSTEM
        FILESYSTEM.format();
    #endif
    if (!FILESYSTEM.begin()) {
        Serial.println("Failed to mount file system");
        #ifdef FORMAT_FILESYSTEM
            Serial.println("File system formatted. Please comment out FORMAT_FILESYSTEM and re-upload.");
        #else
            Serial.println("Consider uncommenting FORMAT_FILESYSTEM for first-time setup.");
        #endif
        return;
    }

    // Initialize and load settings from EEPROM
    settings = new Settings_Module(NUM_SETTINGS);
    loadSettings();

    // Initialize modules
    dac = new DAC_Module();
    pwm = new PWM_Module();

    // Initialize WiFi module
    wifi = new WiFi_Module();
    bool wifi_connected = false;
    #ifndef FORCE_AP
        wifi_connected = wifi->TryConnect(ssid, password, local_ip, gateway, subnet);
    #endif

    if (!wifi_connected) {
        // WiFi connection did not succeed, switch to AP mode
        Serial.println("Could not connect to WiFi. Starting AP...");
        IPAddress ip = wifi->SetupAccessPoint();
        Serial.println("AP started successfully. IP: " + ip.toString());
    } else {
        Serial.println("Successfully connected to WiFi");
    }

    // Initialize web server
    setupWebServer();
    
    Serial.println("Setup complete. Ready to receive requests.");
}

void loop() {
    // Process HTTP requests
    if (server) {
        server->handleClient();
    }
    // Add a small delay to prevent watchdog issues
    delay(1);
}