//
// Created by chairdeng on 2023/11/21.
//

#ifndef LORA_EXPERIMENT_BSP_AHT10_H
#define LORA_EXPERIMENT_BSP_AHT10_H

#include "stdint-gcc.h"

#include "main.h"

#define AHT10_ADDRESS 0x70
#define AHT10_WRITE 0x70
#define AHT10_READ 0x71

#define AHT10_Delay_us(time)                                Delay_us(time)
#define AHT10_Delay_ms(time)                                HAL_Delay(time)

#define AHT10_SDA_GPIO_Port GPIOC
#define AHT10_SDA_Pin GPIO_PIN_1

#define AHT10_SCL_GPIO_Port GPIOD
#define AHT10_SCL_Pin GPIO_PIN_6

#define AHT10_SDA_INIT_OUT AHT10_SDA_INIT(AHT10_SDA_GPIO_Port,AHT10_SDA_Pin,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL)
#define AHT10_SDA_INIT_IN  AHT10_SDA_INIT(AHT10_SDA_GPIO_Port,AHT10_SDA_Pin,GPIO_MODE_INPUT,GPIO_PULLUP)
#define AHT10_SDA_H        AHT10_SDA_State(GPIO_PIN_SET)
#define AHT10_SDA_L        AHT10_SDA_State(GPIO_PIN_RESET)

#define AHT10_SCL_H        AHT10_SCL_State(GPIO_PIN_SET)
#define AHT10_SCL_L        AHT10_SCL_State(GPIO_PIN_RESET)

extern void AHT10Init(void);

extern uint8_t AHT10ReadData(uint16_t *temperature, uint16_t *humidity);

#endif //LORA_EXPERIMENT_BSP_AHT10_H
