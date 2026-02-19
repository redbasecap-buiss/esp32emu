// MCPWM DC Motor control example
// Controls a brushed DC motor with variable speed and direction.

#include <Arduino.h>
#include "driver/mcpwm.h"

#define MOTOR_A_GPIO 15
#define MOTOR_B_GPIO 16

void setup() {
    Serial.begin(115200);
    Serial.println("MCPWM DC Motor Control");

    // Initialize MCPWM GPIO
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM_TIMER_0, MOTOR_A_GPIO, MOTOR_B_GPIO);

    // Configure MCPWM
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 1000;    // 1kHz
    pwm_config.cmpr_a = 0.0f;
    pwm_config.cmpr_b = 0.0f;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    pwm_config.counter_mode = MCPWM_COUNTER_EQUAL_ZERO;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
}

void motorForward(float duty) {
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, duty);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, 0.0f);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);
}

void motorReverse(float duty) {
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 0.0f);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, duty);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);
}

void motorStop() {
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 0.0f);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, 0.0f);
}

void loop() {
    // Accelerate forward
    Serial.println("🔄 Forward: accelerating...");
    for (float duty = 0; duty <= 100; duty += 25) {
        motorForward(duty);
        Serial.printf("  Forward duty: %.0f%%\n", duty);
        delay(500);
    }

    // Stop
    Serial.println("🛑 Stopping");
    motorStop();
    delay(1000);

    // Accelerate reverse
    Serial.println("🔄 Reverse: accelerating...");
    for (float duty = 0; duty <= 100; duty += 25) {
        motorReverse(duty);
        Serial.printf("  Reverse duty: %.0f%%\n", duty);
        delay(500);
    }

    // Stop
    Serial.println("🛑 Stopping");
    motorStop();
    delay(1000);
}
