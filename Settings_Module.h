#ifndef Settings_Module_h
#define Settings_Module_h

#include "EEPROM.h"

#define STRBUF_LEN 256
#define IPADDR_LEN 16

#define MAX_NUM_ENTRIES 256

/**
 * @brief 设置模块，用于从EEPROM存储和检索配置数据
 * 
 * 该模块提供接口以从ESP32的EEPROM内存中
 * 存储和检索各种类型的配置数据。
 */
class Settings_Module {
    public:
        /**
         * @brief 构造函数，用于初始化设置模块
         * @param num_handles 要管理的设置句柄数
         */
        Settings_Module(int num_handles);
        
        /**
         * @brief 析构函数，用于清理设置模块
         */
        ~Settings_Module();
        
        /**
         * @brief 在EEPROM中存储字符串值
         * @param handle 设置的句柄/索引
         * @param value 要存储的字符串值
         */
        void StoreString(int handle, String value);
        
        /**
         * @brief 在EEPROM中存储无符号长整型值
         * @param handle 设置的句柄/索引
         * @param value 要存储的无符号长整型值
         */
        void StoreULong(int handle, unsigned long value);
        
        /**
         * @brief 在EEPROM中存储IP地址
         * @param handle 设置的句柄/索引
         * @param value 要存储的IP地址
         */
        void StoreIp(int handle, IPAddress value);
        
        /**
         * @brief 提交所有更改到EEPROM
         */
        void Commit();
        
        /**
         * @brief 从EEPROM加载字符串值
         * @param handle 设置的句柄/索引
         * @param dest 指向用于存储加载值的字符串的指针
         */
        void LoadString(int handle, String* dest);
        
        /**
         * @brief 从EEPROM加载无符号长整型值
         * @param handle 设置的句柄/索引
         * @param dest 指向用于存储加载值的无符号长整型的指针
         */
        void LoadULong(int handle, unsigned long* dest);
        
        /**
         * @brief Load an IP address from EEPROM
         * @param handle The handle/index for the setting
         * @param value Pointer to the IP address to store the loaded value
         */
        void LoadIp(int handle, IPAddress* value);
        
    private:
        int addresses[MAX_NUM_ENTRIES];  ///< Array to store the addresses of each setting
        int current_address = 0;         ///< Current address in EEPROM
        void registerAddress(int handle, int length);  ///< Register a new address for a setting
};

#endif


