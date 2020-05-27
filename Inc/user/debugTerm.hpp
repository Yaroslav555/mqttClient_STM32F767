#ifndef DEBUGPORT_HPP
#define DEBUGPORT_HPP

#if defined (STM32F070xB) \
||  defined (STM32F072xB)
#include "stm32f0xx_hal.h"
#elif defined (STM32F207xx)
#include "stm32f2xx_hal.h"
#elif defined (STM32F765xx) \
||    defined (STM32F767xx)
#include "stm32f7xx_hal.h"
#elif defined (STM32H743xx)
#include "stm32h7xx_hal.h"
#endif

#define DEBUG_TX_BUFFER_SIZE 1000

#ifdef __cplusplus
extern "C" {
#endif

  class DebugTerm_t{
  private:
    bool txState_ : 1;
    UART_HandleTypeDef *uart_;
    uint8_t  txBuffer_[DEBUG_TX_BUFFER_SIZE];
    uint8_t *readyPosition_;
    uint8_t *endPosition_;
    
    uint8_t* next(uint8_t *position);
    uint8_t* prev(uint8_t *position);
  public:
    DebugTerm_t(void);
    void init(UART_HandleTypeDef *uart);
    HAL_StatusTypeDef putChar(uint8_t symbol);
    void txReady(void);
    void startTx(void);
    void stopTx(void);
  };
  
#ifdef __cplusplus
}
#endif

#endif
