#include "spiSensor.hpp"
#include <string.h>
#include <stdio.h>

void SpiSensor_t::ssEnable (void){
  HAL_GPIO_WritePin(csPort_, csPin_, CS_ON);
}

void SpiSensor_t::ssDisable(void){
  HAL_GPIO_WritePin(csPort_, csPin_, CS_OFF);
}

SpiSensor_t::SpiSensor_t(void){
  status_  = HAL_OK;
  spi_     = NULL;
  csPort_  = NULL;
  csPin_   = 0;
  value_   = 0;
  counter_ = 0;
  state_   = WAITING;
}

HAL_StatusTypeDef SpiSensor_t::txRxStart(uint8_t num){
  if(HAL_SPI_GetState(spi_) != HAL_SPI_STATE_READY)
    return HAL_BUSY;
  if((spi_->Init.CLKPhase != SPI_PHASE_1EDGE)
  || (spi_->Init.CLKPolarity != SPI_POLARITY_LOW)){
    spi_->Init.CLKPolarity = SPI_POLARITY_LOW;
    spi_->Init.CLKPhase = SPI_PHASE_1EDGE;
    HAL_SPI_Init(spi_);
  }
  ssEnable();
  return(HAL_SPI_TransmitReceive_IT(spi_, out_, in_, num));
}

HAL_StatusTypeDef SpiSensor_t::txStart(uint8_t num){
  if(HAL_SPI_GetState(spi_) != HAL_SPI_STATE_READY)
    return HAL_BUSY;
  if((spi_->Init.CLKPhase != SPI_PHASE_1EDGE)
  || (spi_->Init.CLKPolarity != SPI_POLARITY_LOW)){
    spi_->Init.CLKPhase    = SPI_PHASE_1EDGE;
    spi_->Init.CLKPolarity = SPI_POLARITY_LOW;
    status_ = HAL_SPI_Init(spi_);
  }
  ssEnable();
  return(HAL_SPI_Transmit_IT(spi_, out_, num));
}

void SpiSensor_t::endTransmition(void){
  ssDisable();
  state_ = IDLE;
}

void SpiSensor_t::init(SPI_HandleTypeDef *spi,
                       GPIO_TypeDef *csPort,
                       uint16_t csPin){
  spi_    = spi;
  csPort_ = csPort;
  csPin_  = csPin;
  HAL_GPIO_WritePin(csPort_, csPin_, CS_OFF);
  state_  = IDLE;
}

float SpiSensor_t::getValue(void){
  return value_;
}

void SpiSensor_t::poll(void){
  FloatConv_t val;
  switch(state_){
  case WAITING:
    break;
  case WRITE:
    break;
  case READ:
    ++counter_;
    val.float_ = counter_;
      
    out_[0] = val.byte_[0];
    out_[1] = val.byte_[1];
    out_[2] = val.byte_[2];
    out_[3] = val.byte_[3];
      
    val.byte_[0] = in_[0];
    val.byte_[1] = in_[1];
    val.byte_[2] = in_[2];
    val.byte_[3] = in_[3];
    if((val.float_ >= -1) && (val.float_ <= 1))
      value_ = val.float_;
    printf("value = %f\n", value_);
    txRxStart(4);
    state_ = WAITING;
    break;
  case IDLE:
    state_ = READ;
    break;
  }
}
