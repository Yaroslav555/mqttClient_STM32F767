#ifndef USERFUNCTIONS_HPP
#define USERFUNCTIONS_HPP

#if defined (STM32F070xB) \
|| defined (STM32F072xB)
#include "stm32f0xx_hal.h"
#elif defined (STM32F207xx)
#include "stm32f2xx_hal.h"
#elif defined (STM32F765xx)\
||    defined (STM32F767xx)
#include "stm32f7xx_hal.h"
#elif defined (STM32H743xx)
#include "stm32h7xx_hal.h"
#endif

#define DEBUG_PORT_ON
#define DEBUG_UART huart3

#ifdef __cplusplus
extern "C" {
#endif
void Success_Handler(void);

void debugTxComplette(void);
void initDebugTerm(UART_HandleTypeDef *uart);
HAL_StatusTypeDef putDebugChar(uint8_t symbol);

void initMqtt(void);
void mqttTask(void *argument);

/*void initSpiSensor(SPI_HandleTypeDef *spi, 
                   GPIO_TypeDef *csPort, 
                   uint16_t csPin);*/
void sensorTxReady(void);
void spiSensorTask(void *argument);

void startThreads(void);
#ifdef __cplusplus
}
#endif

#endif
