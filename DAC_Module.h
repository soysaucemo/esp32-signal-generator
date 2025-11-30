#ifndef DAC_Module_h
#define DAC_Module_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "soc/rtc_io_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "soc/rtc.h"

#include "driver/dac.h"

/**
 * @brief DAC模块，用于使用ESP32内置DAC生成正弦波
 * 
 * 该模块提供接口以配置和控制ESP32的
 * 内置DAC用于生成正弦波。
 */
class DAC_Module {
    public:
        /**
         * @brief 停止指定通道的DAC输出
         * @param channel 要停止的DAC通道 (DAC_CHANNEL_1 或 DAC_CHANNEL_2)
         */
        void Stop(dac_channel_t channel);
        
        /**
         * @brief 为正弦波生成设置DAC
         * @param channel 要配置的DAC通道 (DAC_CHANNEL_1 或 DAC_CHANNEL_2)
         * @param clk_div 时钟分频值 (0-7)
         * @param frequency 频率步进值 (0x0001 - 0xFFFF)
         * @param scale 输出缩放 (0-3, 详情见 dac_scale_set)
         * @param phase 相位偏移值 (0x00 - 0xFF)
         * @param invert 反相模式 (0-3, 详情见 dac_invert_set)
         */
        void Setup(dac_channel_t channel, int clk_div, int frequency, int scale, int phase, int invert);
        
    private:
        /**
         * @brief 在DAC通道上启用余弦波形发生器
         * @param channel 要启用的DAC通道
         * @param invert 反相模式
         */
        void dac_cosine_enable(dac_channel_t channel, int invert);
        
        /**
         * @brief 禁用DAC通道上的余弦波形发生器
         * @param channel 要禁用的DAC通道
         */
        void dac_cosine_disable(dac_channel_t channel);
        
        /**
         * @brief 设置两个DAC通道共用的内部CW发生器的频率
         * @param clk_8m_div 时钟分频 (0b000 - 0b111)
         * @param frequency_step 频率步进 (范围 0x0001 - 0xFFFF)
         */
        void dac_frequency_set(int clk_8m_div, int frequency_step);
        
        /**
         * @brief 缩放DAC通道的输出
         * @param channel 要缩放的DAC通道
         * @param scale 缩放值 (0: 无缩放, 1: 1/2, 2: 1/4, 3: 1/8)
         */
        void dac_scale_set(dac_channel_t channel, int scale);
        
        /**
         * @brief 偏移DAC通道的输出
         * @param channel 要偏移的DAC通道
         * @param offset 偏移值 (0x00 - 0xFF)
         */
        void dac_offset_set(dac_channel_t channel, int offset);
        
        /**
         * @brief 反相DAC通道的输出模式
         * @param channel 要反相的DAC通道
         * @param invert 反相模式 (0: 不反相, 1: 全部反相, 2: 反相MSB, 3: 除MSB外全部反相)
         */
        void dac_invert_set(dac_channel_t channel, int invert);
};

#endif

