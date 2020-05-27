#include "debugTerm.hpp"

uint8_t* DebugTerm_t::next(uint8_t *position){
  ++position;
  if(position >= &txBuffer_[DEBUG_TX_BUFFER_SIZE]) //if reach end of buffer
    position = txBuffer_;
  return position;
}

uint8_t* DebugTerm_t::prev(uint8_t *position){
  --position;
  if(position <= txBuffer_) //if reach begin of buffer
    position = &txBuffer_[DEBUG_TX_BUFFER_SIZE] - 1;
  return position;
}

DebugTerm_t::DebugTerm_t(void){
  readyPosition_ = txBuffer_;
  endPosition_   = txBuffer_;  
}

void DebugTerm_t::init(UART_HandleTypeDef *uart){
  uart_ = uart;
  txState_ = false;
}

HAL_StatusTypeDef DebugTerm_t::putChar(uint8_t symbol){
  HAL_StatusTypeDef status = HAL_OK; 
  
  if(endPosition_ == prev(readyPosition_)){ //return if new position reach current transmiter position
    status = HAL_BUSY;
  }
  else{ //else put new symbol to the buffer
    *endPosition_ = symbol;
    endPosition_ = next(endPosition_);
    status = HAL_OK;
    startTx();
  }
  return status;
}

void DebugTerm_t::txReady(void){
  if(readyPosition_ == endPosition_){ //all data send ready or object blocked by putchar action
    txState_ = false; //set flag to off
  }
  else{
    txState_ = true;
    HAL_UART_Transmit_IT(uart_, readyPosition_, 1);
    readyPosition_ = next(readyPosition_);
  }
}

void DebugTerm_t::startTx(void){
  if(txState_) //return if already on
    return;
  txReady();
}

void DebugTerm_t::stopTx(void){
  HAL_UART_AbortTransmit_IT(uart_);
  txState_ = false;
}
