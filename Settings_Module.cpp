#include "Settings_Module.h"

Settings_Module::Settings_Module(int num_handles) {
    if (!EEPROM.begin(1000)) {
        Serial.println("Failed to initialise EEPROM");
    }

    for (int i = 0; i < num_handles; i++)
    {
        addresses[i] = EEPROM.readInt(current_address);
        current_address += sizeof(int32_t);
    }
}

void Settings_Module::registerAddress(int handle, int length) {
    addresses[handle] = current_address;
    current_address += length;
    EEPROM.writeInt(handle * sizeof(uint32_t), addresses[handle]);
}

void Settings_Module::StoreString(int handle, String value) {
    // Validate handle
    if (handle < 0 || handle >= MAX_NUM_ENTRIES) {
        Serial.println("Invalid handle for StoreString: " + String(handle));
        return;
    }
    
    registerAddress(handle, STRBUF_LEN);
    EEPROM.writeString(addresses[handle], value);
}

void Settings_Module::StoreULong(int handle, unsigned long value) {
    // Validate handle
    if (handle < 0 || handle >= MAX_NUM_ENTRIES) {
        Serial.println("Invalid handle for StoreULong: " + String(handle));
        return;
    }
    
    registerAddress(handle, sizeof(unsigned long));
    EEPROM.writeULong(addresses[handle], value);
}

void Settings_Module::StoreIp(int handle, IPAddress value) {
    // Validate handle
    if (handle < 0 || handle >= MAX_NUM_ENTRIES) {
        Serial.println("Invalid handle for StoreIp: " + String(handle));
        return;
    }
    
    registerAddress(handle, IPADDR_LEN);
    EEPROM.writeString(addresses[handle], value.toString());
}

void Settings_Module::Commit() {
    if (!EEPROM.commit()) {
        Serial.println("Failed to commit EEPROM data");
    }
}

void Settings_Module::LoadString(int handle, String* dest) {
    // Validate handle
    if (handle < 0 || handle >= MAX_NUM_ENTRIES) {
        Serial.println("Invalid handle for LoadString: " + String(handle));
        *dest = "";
        return;
    }
    
    *dest = EEPROM.readString(addresses[handle]);
}

void Settings_Module::LoadULong(int handle, unsigned long* dest) {
    // Validate handle
    if (handle < 0 || handle >= MAX_NUM_ENTRIES) {
        Serial.println("Invalid handle for LoadULong: " + String(handle));
        *dest = 0;
        return;
    }
    
    *dest = EEPROM.readULong(addresses[handle]);
}

void Settings_Module::LoadIp(int handle, IPAddress* value) {
    // Validate handle
    if (handle < 0 || handle >= MAX_NUM_ENTRIES) {
        Serial.println("Invalid handle for LoadIp: " + String(handle));
        *value = IPAddress(0, 0, 0, 0);
        return;
    }
    
    String buf = EEPROM.readString(addresses[handle]);
    if (!value->fromString(buf)) {
        Serial.println("Failed to parse IP address: " + buf);
        *value = IPAddress(0, 0, 0, 0);
    }
}

Settings_Module::~Settings_Module() {
    // Nothing to clean up, but the destructor is here for completeness
}
