#ifndef SPISENSOR_HPP
#define SPISENSOR_HPP

#if defined (STM32F070xB) \
||  defined (STM32F072xB)
#include "stm32f0xx_hal.h"
#elif defined (STM32F765xx) \
||    defined (STM32F767xx)
#include "stm32f7xx_hal.h" 
#elif defined (STM32H743xx)
#include "stm32h7xx_hal.h"
#endif

#define CS_ON  GPIO_PIN_RESET
#define CS_OFF GPIO_PIN_SET

#define SPI_BUFFER_SIZE 10U

union FloatConv_t{
  float float_;
  uint8_t byte_[4];
};

class SpiSensor_t{
private:
  enum State_t{
    WAITING,
    WRITE,
    READ,
    IDLE
  } state_;
  
  HAL_StatusTypeDef  status_;
  
  SPI_HandleTypeDef *spi_;
  GPIO_TypeDef      *csPort_;
  uint16_t           csPin_;
  
  uint8_t            in_ [SPI_BUFFER_SIZE];
  uint8_t            out_[SPI_BUFFER_SIZE]; 
  
  
  float value_;
  uint8_t counter_;
  
  HAL_StatusTypeDef txRxStart(uint8_t num);
  HAL_StatusTypeDef txStart(uint8_t num);
  void    ssEnable (void);
  void    ssDisable(void);
public:
  SpiSensor_t(void);
  void init(SPI_HandleTypeDef *spi,
            GPIO_TypeDef *csPort,
            uint16_t csPin);
  float getValue(void);
  void endTransmition(void);
  void poll(void);
};

#endif