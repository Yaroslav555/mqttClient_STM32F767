#ifndef MQTTCLIENT_HPP
#define MQTTCLIENT_HPP

#if defined (STM32F070xB) || defined (STM32F072xB)
#include "stm32f0xx_hal.h"
#elif defined (STM32F765xx)
#include "stm32f7xx_hal.h"
#elif defined (STM32H743xx)
#include "stm32h7xx_hal.h"
#endif

#include "net_sockets.h"
#include "mqtt.h"
#include "x509_crt.h"
#include "altcp_tls.h"
#include "ssl.h"
#include "entropy.h"
#include "ctr_drbg.h"


struct altcp_tls_config {
  mbedtls_ssl_config conf;
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context ctr_drbg;
  mbedtls_x509_crt *cert;
  mbedtls_pk_context *pkey;
  mbedtls_x509_crt *ca;
#if defined(MBEDTLS_SSL_CACHE_C) && ALTCP_MBEDTLS_SESSION_CACHE_TIMEOUT_SECONDS
  /** Inter-connection cache for fast connection startup */
  struct mbedtls_ssl_cache_context cache;
#endif
};

#ifdef __cplusplus
extern "C" {
#endif
  enum MqttState_t{
  DISCONNECTING,
  CONNECTING,
  PUBLISHING,
  IDLE
  };
  
  
class MqttClient_t{
private:
  static MqttClient_t *object_; //temporary solution
  //command flags
  bool                       doConnect_    : 1,
                             doDisconnect_ : 1,
                             doPublish_    : 1;
  //state flags
  bool                       connected_    : 1;
  
  MqttState_t                state_;
  
  ip_addr_t                  ipaddr_;
  uint16_t                   port_;
  mqtt_client_t             *client_;
  mqtt_connect_client_info_t ci_;
  int16_t                    inpubId_;
  int16_t                    arg_;
  const char                *publishTopic_;
  const char                *publishPayload_;
  uint16_t                   publishTimeout_;
  

  
  static void connectionCb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status);
  static void incomingPublishCb(void *arg, const char *topic, u32_t tot_len);
  static void incomingDataCb(void *arg, const u8_t *data, u16_t len, u8_t flags);
  static void subRequestCb(void *arg, err_t err);
  static void subRequest(void *arg, err_t result);
  static void pubRequestCb(void *arg, err_t err);
  void publish(void);
  void doDisconnect(void);
  void doConnect(void);
  void doPublish(void);
  
public:
  MqttClient_t(void);
  ~MqttClient_t(void);
  void init(uint8_t  ipaddrP0, 
            uint8_t  ipaddrP1, 
            uint8_t  ipaddrP2, 
            uint8_t  ipaddrP3,
           uint16_t  port,
         const uint8_t *clientId,
         const uint8_t *clientUser,/** User name and password, set to NULL if not used */
         const uint8_t *clientPass,
         const uint8_t *caCrt,
       const size_t  caCrtLen,
         const uint8_t *clientCrt,
       const size_t  clientCrtLen,
         const uint8_t *clientKey,
       const size_t  clientKeyLen,
         uint16_t    keepAliveTime);
  bool publish(const char *publishTopic, const char *publishPayload);
  bool isConnected(void);
  void poll(void);
};

#ifdef __cplusplus
}
#endif

#endif
