#include "PWM_Module.h"

void PWM_Module::Stop(ledc_channel_t channel_num, bool highspeed) {
    ledc_mode_t speed_mode = highspeed ? LEDC_HIGH_SPEED_MODE : LEDC_LOW_SPEED_MODE;
    ledc_stop(speed_mode, channel_num, 0);
}

void PWM_Module::Setup(ledc_timer_t timer_num, ledc_channel_t channel_num, bool highspeed, 
                      ledc_timer_bit_t resolution, uint32_t frequency, uint32_t duty, 
                      int phase, int out_pin) {
    ledc_mode_t speed_mode = highspeed ? LEDC_HIGH_SPEED_MODE : LEDC_LOW_SPEED_MODE;

    // Validate parameters
    if (timer_num < LEDC_TIMER_0 || timer_num > LEDC_TIMER_3) {
        Serial.println("Invalid timer number: " + String(timer_num));
        return;
    }
    
    if (channel_num < LEDC_CHANNEL_0 || channel_num > LEDC_CHANNEL_7) {
        Serial.println("Invalid channel number: " + String(channel_num));
        return;
    }
    
    if (resolution < LEDC_TIMER_1_BIT || resolution > LEDC_TIMER_14_BIT) {
        Serial.println("Invalid resolution: " + String(resolution));
        return;
    }
    
    if (frequency == 0) {
        Serial.println("Frequency cannot be zero");
        return;
    }
    
    if (out_pin < 0 || out_pin > 39) {
        Serial.println("Invalid output pin: " + String(out_pin));
        return;
    }

    ledc_timer_config_t timer_config; 
    timer_config.speed_mode = speed_mode;       // timer mode
    timer_config.duty_resolution = resolution;  // resolution of PWM duty, e.g. LEDC_TIMER_10_BIT
    timer_config.timer_num = timer_num;         // timer index
    timer_config.freq_hz = frequency;           // frequency of PWM signal
    timer_config.clk_cfg = LEDC_AUTO_CLK;      // auto select the source clock

    ledc_channel_config_t channel_config = {
        .gpio_num   = out_pin,
        .speed_mode = speed_mode,
        .channel    = channel_num,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = timer_num,
        .duty       = duty,
        .hpoint     = phase
    };

    ledc_timer_config(&timer_config);
    ledc_channel_config(&channel_config);
}
