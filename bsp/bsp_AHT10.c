//
// Created by chairdeng on 2023/11/21.
//

#include "bsp_AHT10.h"

static void Delay_us(uint16_t usec);

static void AHT10_SDA_State(GPIO_PinState PinState);

static void AHT10_SCL_State(GPIO_PinState PinState);

static void AHT10_SDA_INIT(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint32_t Mode, uint32_t Pull);

static uint8_t AHT10Check(void);

static void AHT10Reset(void);

static void Delay_us(uint16_t usec) {
    uint16_t i = 4;

    usec >>= 1;

    while (usec--) {
        while (i--) { ;
        }
    }
}

void AHT10_SDA_INIT(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint32_t Mode, uint32_t Pull) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_Pin;
    GPIO_InitStruct.Mode = Mode;
    GPIO_InitStruct.Pull = Pull;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

static void AHT10_SDA_State(GPIO_PinState PinState) {
    HAL_GPIO_WritePin(AHT10_SDA_GPIO_Port, AHT10_SDA_Pin, PinState);
}

static void AHT10_SCL_State(GPIO_PinState PinState) {
    HAL_GPIO_WritePin(AHT10_SCL_GPIO_Port, AHT10_SCL_Pin, PinState);
}

static uint8_t read_sda_data(void) {
    return HAL_GPIO_ReadPin(AHT10_SDA_GPIO_Port, AHT10_SDA_Pin);
}

static void AHT10_IIC_Start(void) {
    AHT10_SDA_INIT_OUT;
    AHT10_SDA_H;
    AHT10_SCL_H;
    AHT10_Delay_us(4);
    AHT10_SDA_L;
    AHT10_Delay_us(4);
    AHT10_SCL_L;
}

static void AHT10_IIC_Stop(void) {
    AHT10_SDA_INIT_OUT;
    AHT10_SDA_L;
    AHT10_SCL_L;
    AHT10_Delay_us(4);
    AHT10_SCL_H;
    AHT10_SDA_H;
    AHT10_Delay_us(4);
}

static uint8_t AHT10_IIC_Wait_Ack(void) {
    uint8_t ucErrTime = 0;
    AHT10_SDA_INIT_IN;
    AHT10_SDA_H;
    AHT10_Delay_us(1);
    AHT10_SCL_H;
    AHT10_Delay_us(1);
    while (read_sda_data()) {
        ucErrTime++;
        if (ucErrTime > 250) {
            AHT10_IIC_Stop();
            return 1;
        }
    }
    AHT10_SCL_L;
    return 0;
}

static void AHT10_IIC_Ack(void) {
    AHT10_SCL_L;
    AHT10_SDA_INIT_OUT;
    AHT10_SDA_L;
    AHT10_Delay_us(2);
    AHT10_SCL_H;
    AHT10_Delay_us(2);
    AHT10_SCL_L;
}

static void AHT10_IIC_NAck(void) {
    AHT10_SCL_L;
    AHT10_SDA_INIT_OUT;
    AHT10_SDA_H;
    AHT10_Delay_us(2);
    AHT10_SCL_H;
    AHT10_Delay_us(2);
    AHT10_SCL_L;
}

static void AHT10_IIC_Send_Byte(uint8_t txd) {
    AHT10_SDA_INIT_OUT;
    AHT10_SCL_L;
    for (uint8_t t = 0; t < 8; t++) {
        if ((txd & 0x80) >> 7)//高位先传，&1000 0000后将第八位右移至第一位
            AHT10_SDA_H;
        else
            AHT10_SDA_L;
        txd <<= 1;//每传完一位后，低位向高位进1
        AHT10_Delay_us(2);
        AHT10_SCL_H;
        AHT10_Delay_us(2);
        AHT10_SCL_L;
        AHT10_Delay_us(2);
    }
}

static uint8_t AHT10_IIC_Read_Byte(unsigned char ack) {
    unsigned char i, receive = 0;
    AHT10_SDA_INIT_IN;
    for (i = 0; i < 8; i++) {
        AHT10_SCL_L;
        AHT10_Delay_us(2);
        AHT10_SCL_H;
        receive <<= 1;
        if (read_sda_data())
            receive++;
        AHT10_Delay_us(1);
    }
    if (!ack)
        AHT10_IIC_NAck();//发送nack
    else
        AHT10_IIC_Ack(); //发送ack
    return receive;
}

static uint8_t AHT10Check(void) {
    uint8_t ack = 0;
    AHT10_IIC_Start();
    AHT10_IIC_Send_Byte(AHT10_ADDRESS);
    ack = AHT10_IIC_Wait_Ack();
    AHT10_IIC_Stop();
    return ack;//返回1则未检测到
}

static void AHT10Reset(void) {
    AHT10_IIC_Start();
    AHT10_IIC_Send_Byte(AHT10_WRITE);
    AHT10_IIC_Wait_Ack();
    AHT10_IIC_Send_Byte(0xba);
    AHT10_IIC_Wait_Ack();
    AHT10_IIC_Stop();
}

void AHT10Init() {
    AHT10_IIC_Start();
    AHT10_IIC_Send_Byte(AHT10_ADDRESS);
    AHT10_IIC_Send_Byte(0xe1);    //AHT10启动的初始化指令
    AHT10_IIC_Send_Byte(0x08);
    AHT10_IIC_Send_Byte(0x00);
    AHT10_IIC_Stop();
    AHT10_Delay_ms(40);//延时40MS让传感器稳定
}

uint8_t AHT10ReadData(uint16_t *temperature, uint16_t *humidity) {
    uint8_t ack;
    uint32_t SRH = 0, ST = 0;
    uint8_t databuff[6];
    AHT10_IIC_Start();
    AHT10_IIC_Send_Byte(AHT10_WRITE);
    AHT10_IIC_Wait_Ack();
    AHT10_IIC_Send_Byte(0xac);
    AHT10_IIC_Wait_Ack();
    AHT10_IIC_Send_Byte(0x33);
    AHT10_IIC_Wait_Ack();
    AHT10_IIC_Send_Byte(0x00);
    AHT10_IIC_Wait_Ack();
    AHT10_IIC_Stop();
    AHT10_Delay_ms(80);//延时等待数据读取。
    AHT10_IIC_Start();
    AHT10_IIC_Send_Byte(AHT10_READ);
    AHT10_IIC_Wait_Ack();
    ack = AHT10_IIC_Read_Byte(1);
    if ((ack & 0x40) == 0) {
        databuff[0] = AHT10_IIC_Read_Byte(1);
        databuff[1] = AHT10_IIC_Read_Byte(1);
        databuff[2] = AHT10_IIC_Read_Byte(1);
        databuff[3] = AHT10_IIC_Read_Byte(1);
        databuff[4] = AHT10_IIC_Read_Byte(0);
        AHT10_IIC_Stop();
        SRH = (databuff[0] << 12) + (databuff[1] << 4) + (databuff[2] >> 4);
        ST = ((databuff[2] & 0X0f) << 16) + (databuff[3] << 8) + (databuff[4]);

        *humidity = SRH * 1000 / 1024 / 1024;
        *temperature = ST * 200 * 10 / 1024 / 1024 - 500;
        return 1;
    }
    AHT10_IIC_Stop();
    return 0;
}
