/*
 * motor_driver.c
 *
 *  Created on: Nov 5, 2025
 *      Author: YJHeo01
 */

#include "motor_driver.h"

static inline uint32_t map1000toARR(uint32_t arr, uint16_t val){
    if (val > 1000) val = 1000;
    return (arr * val) / 1000U;
}

void DirPwm_Init(DirPwmMotor* m,
                 TIM_HandleTypeDef* htim, uint32_t channel,
                 GPIO_TypeDef* dir_port, uint16_t dir_pin,
                 uint8_t invert) {
    m->htim        = htim;
    m->pwm_channel = channel;
    m->dir_port    = dir_port;
    m->dir_pin     = dir_pin;
    m->invert      = invert;
    m->arr         = __HAL_TIM_GET_AUTORELOAD(htim);

    // 초기: 정방향(또는 invert 반영) & 듀티 0
    HAL_GPIO_WritePin(m->dir_port, m->dir_pin,
                      (invert ? GPIO_PIN_RESET : GPIO_PIN_SET));

    HAL_TIM_PWM_Start(m->htim, m->pwm_channel);
    __HAL_TIM_SET_COMPARE(m->htim, m->pwm_channel, 0);
}

void DirPwm_SetSpeed(DirPwmMotor* m, int16_t speed){
    if (!m || !m->htim) return;

    if (speed > 1000) speed = 1000;
    if (speed < -1000) speed = -1000;

    uint8_t forward = (speed >= 0);
    uint16_t mag = (speed >= 0) ? (uint16_t)speed : (uint16_t)(-speed);
    if (m->invert) forward = !forward;

    HAL_GPIO_WritePin(m->dir_port, m->dir_pin, forward ? GPIO_PIN_SET : GPIO_PIN_RESET);
    __HAL_TIM_SET_COMPARE(m->htim, m->pwm_channel, map1000toARR(m->arr, mag));
}

void DirPwm_Coast(DirPwmMotor* m){
    if (!m || !m->htim) return;
    __HAL_TIM_SET_COMPARE(m->htim, m->pwm_channel, 0);
}
