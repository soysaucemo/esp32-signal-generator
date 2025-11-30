#ifndef PWM_Module_h
#define PWM_Module_h

#include "driver/ledc.h"

/**
 * @brief PWM模块，用于使用ESP32的LEDC（LED控制）外设生成方波
 * 
 * 该模块提供接口以配置和控制ESP32的
 * LEDC外设用于生成方波。
 */
class PWM_Module {
    public:
        /**
         * @brief 停止指定通道的PWM输出
         * @param channel_num 要停止的LEDC通道
         * @param highspeed 通道是否处于高速模式
         */
        void Stop(ledc_channel_t channel_num, bool highspeed);
        
        /**
         * @brief 为方波生成设置PWM
         * @param timer_num 要使用的LEDC定时器 (0-3)
         * @param channel_num 要使用的LEDC通道 (0-7)
         * @param highspeed 是否使用高速模式 (true) 或低速模式 (false)
         * @param resolution 定时器的分辨率（以位为单位）
         * @param frequency PWM信号的频率（以Hz为单位）
         * @param duty 占空比值（取决于分辨率）
         * @param phase 相位偏移值
         * @param out_pin 用于输出信号的GPIO引脚
         */
        void Setup(ledc_timer_t timer_num, ledc_channel_t channel_num, bool highspeed, 
                  ledc_timer_bit_t resolution, uint32_t frequency, uint32_t duty, 
                  int phase, int out_pin);
    private:
};

#endif

