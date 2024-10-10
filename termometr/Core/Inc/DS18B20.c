#include "DS18B20.h"

void DS18B20_Init(DS18B20 *sensor, TIMER *tim) {
    sensor->tim = tim;
}

void DS18B20_setDataPin(DS18B20 *sensor, int on) {
    if (on) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
    }
}

void DS18B20_toggleDataPin(DS18B20 *sensor) {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0);
}

void DS18B20_setPinOUTPUT(DS18B20 *sensor) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void DS18B20_setPinINPUT(DS18B20 *sensor) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void DS18B20_startSensor(DS18B20 *sensor) {
    DS18B20_setPinOUTPUT(sensor);
    DS18B20_setDataPin(sensor, 0);

    TIMER_delayUS(sensor->tim, 480);
    DS18B20_setPinINPUT(sensor);
    TIMER_delayUS(sensor->tim, 80);
    HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
    TIMER_delayUS(sensor->tim, 400);
}

void DS18B20_writeData(DS18B20 *sensor, uint8_t data) {
    DS18B20_setPinOUTPUT(sensor);

    for (uint8_t i = 0; i < 8; i++) {
        if (data & (1 << i)) {
            DS18B20_setPinOUTPUT(sensor);
            DS18B20_setDataPin(sensor, 0);
            TIMER_delayUS(sensor->tim, 1);

            DS18B20_setPinINPUT(sensor);
            TIMER_delayUS(sensor->tim, 60);
        } else {
            DS18B20_setPinOUTPUT(sensor);
            DS18B20_setDataPin(sensor, 0);
            TIMER_delayUS(sensor->tim, 60);

            DS18B20_setPinINPUT(sensor);
        }
    }
}

uint8_t DS18B20_readData(DS18B20 *sensor) {
    uint8_t value = 0;
    DS18B20_setPinINPUT(sensor);

    for (uint8_t i = 0; i < 8; i++) {
        DS18B20_setPinOUTPUT(sensor);
        DS18B20_setDataPin(sensor, 0);
        TIMER_delayUS(sensor->tim, 2);

        DS18B20_setPinINPUT(sensor);
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)) {
            value |= 1 << i;
        }
        TIMER_delayUS(sensor->tim, 60);
    }
    return value;
}

float DS18B20_readTemperature(DS18B20 *sensor) {
    DS18B20_startSensor(sensor);
    HAL_Delay(1);
    DS18B20_writeData(sensor, 0xCC);
    DS18B20_writeData(sensor, 0x44);
    HAL_Delay(800);
    DS18B20_startSensor(sensor);
    DS18B20_writeData(sensor, 0xCC);
    DS18B20_writeData(sensor, 0xBE);

    uint8_t temp1 = DS18B20_readData(sensor);
    uint8_t temp2 = DS18B20_readData(sensor);

    uint16_t tempCom = (temp2 << 8) | temp1;

    return (float)(tempCom / 16.0);
}