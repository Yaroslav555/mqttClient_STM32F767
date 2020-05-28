##About project

Simple project of mqtt client for STM32F767. 
Based on:
- STM32Cube_FW_F7_V1.16.0
- FREERTOS CMSIS_V2 
- LWIP 2.1.2 
- MBEDTLS 2.16.2

##Functionality

Just connect with TLS 1.2 to broker by IPv4 and send messages.

##ToDo
MqttClient_t class:
 - write mechanism for incomming mqtt telegrams
 - improve logic machine for reconnect and processing network collisions
 - rework mechanism for callbaks
 - DNS name resolving