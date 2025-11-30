#include "WiFi_Module.h"

bool WiFi_Module::TryConnect(String ssid, String password, IPAddress local_ip, IPAddress gateway, IPAddress subnet) {
    bool wifi_connected = false;

    bool wifi_configured = ssid[0];
    if (wifi_configured) {
        WiFi.disconnect(true);  //disconnect form wifi to set new wifi connection
        WiFi.mode(WIFI_STA); //init wifi mode

        // Set static IP, gateway and subnet if the static IP is specified
        if (local_ip[0] != 0) {
            Serial.println("Configuring static IP...");
            if (!WiFi.config(local_ip, gateway, subnet)) {
                Serial.println("STA Failed to configure");
            }
        }

        // Connect using WPA personal
        WiFi.begin(ssid.c_str(), password.c_str());

        int ctr = 10;
        Serial.println("Attempting to connect to WiFi...");
        while (WiFi.status() != WL_CONNECTED && ctr-- > 0) {
            Serial.print(".");
            delay(1000);
        }

        wifi_connected = (ctr >= 0);
        
        if (wifi_connected) {
            Serial.println("\nWiFi connected successfully!");
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());
        } else {
            Serial.println("\nWiFi connection failed!");
        }
    }

    return wifi_connected;
}

IPAddress WiFi_Module::SetupAccessPoint()
{
    // Run device as access point with SSID "SigGen"
    WiFi.disconnect(true);
    Serial.println("Setting up Access Point...");
    WiFi.mode(WIFI_AP);
    WiFi.softAP("SigGen");
    IPAddress ip = WiFi.softAPIP();
    Serial.println("Access Point started successfully!");
    Serial.print("AP IP address: ");
    Serial.println(ip);
    return ip;
}
