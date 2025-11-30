#ifndef WiFi_Module_h
#define WiFi_Module_h

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

/**
 * @brief WiFi Module for handling WiFi connectivity
 * 
 * This module provides an interface to connect to WiFi networks or 
 * operate as an access point.
 */
class WiFi_Module {
    public:
        /**
         * @brief Attempt to connect to a WiFi network
         * @param ssid The network SSID to connect to
         * @param password The network password
         * @param local_ip Local IP address (for static IP configuration)
         * @param gateway Gateway IP address (for static IP configuration)
         * @param subnet Subnet mask (for static IP configuration)
         * @return true if connection successful, false otherwise
         */
        bool TryConnect(String ssid, String password, IPAddress local_ip, 
                       IPAddress gateway, IPAddress subnet);
        
        /**
         * @brief Setup the ESP32 as a WiFi access point
         * @return The IP address assigned to the access point
         */
        IPAddress SetupAccessPoint();
    private:
};

#endif


