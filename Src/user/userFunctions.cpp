#include "userFunctions.hpp"
#include "cmsis_os.h"
#include "debugTerm.hpp"
#include "mqttClient.hpp"
//#include "spiSensor.hpp"
#include "certificates.hpp"

DebugTerm_t  debugTerm;

MqttClient_t mqttClient;
const uint8_t mqttClientId[] = "nucleo";
const uint8_t mqttUser[]     = "user";
const uint8_t mqttPassword[] = "password";

osThreadId_t mqttTaskHandle;
const osThreadAttr_t mqttTaskAttributes = {
  .name = "mqttTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 2048 * 4
};

osThreadId_t spiTaskHandle;
const osThreadAttr_t spiTaskAttributes = {
  .name = "spiTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};

/*Init debug terminal*/
void initDebugTerm(UART_HandleTypeDef *uart){
  debugTerm.init(uart);
}

/*Call from putCh function*/
HAL_StatusTypeDef putDebugChar(uint8_t symbol){
  return debugTerm.putChar(symbol);
}

/*Usart interrupt*/
void debugTxComplette(void){
#ifdef DEBUG_PORT_ON
    debugTerm.txReady();
#endif
}

/*Init MQTT client*/
void initMqtt(void){
 mqttClient.init(192, 168, 0, 99, 8883, 
                  mqttClientId, mqttUser, mqttPassword, 
                  mbedtls_ca_crt,  mbedtls_ca_crt_len,
                  mbedtls_client_crt,  mbedtls_client_crt_len, 
                  mbedtls_client_key, mbedtls_client_key_len, 60);

}

/*OS poled mqtt task*/
void mqttTask(void *argument){
  static char str[100];
  static uint16_t value = 0;
  while(1){
    vTaskDelay(1000);
    mqttClient.poll();
    if(mqttClient.isConnected()){
      sprintf(str, "value = %d", value/*spiSensor.getValue()*/);
      printf("Publishing:");
      printf(str);
      printf("\n");
      if(!mqttClient.publish("sensor", str))
        ++value;
    }
    
  }
}

/*SPI interrupt*/
/*void sensorTxReady(void){
  spiSensor.endTransmition();
}
void initSpiSensor(SPI_HandleTypeDef *spi, 
                   GPIO_TypeDef *csPort, 
                   uint16_t csPin){
  spiSensor.init(spi, csPort, csPin);
}
*/
/*
void spiSensorTask(void *argument){
  while(1){
    spiSensor.poll();
    vTaskDelay(1000);
  }
}
*/

void Success_Handler(void){
  while(1){
    vTaskDelay(1000);
  }
}

void startThreads(void){
  mqttTaskHandle = osThreadNew(mqttTask, NULL, &mqttTaskAttributes);
  //spiTaskHandle  = osThreadNew(spiSensorTask, NULL, &spiTaskAttributes);
}