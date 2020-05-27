#include "mqttClient.hpp"

#include <string.h>

MqttClient_t *(MqttClient_t::object_);

MqttClient_t::MqttClient_t(void){
  client_                  = NULL;
  doConnect_               = false;
  doDisconnect_            = false;
  doPublish_               = false;

  connected_               = false;
    
  ipaddr_.addr             = NULL;
  port_                    = NULL;
  object_                  = this;
  connected_               = false;
  
  publishTopic_   = NULL;
  publishPayload_ = NULL;
}

MqttClient_t::~MqttClient_t(void){

}

void MqttClient_t::init(uint8_t  ipaddrP0, 
                        uint8_t  ipaddrP1, 
                        uint8_t  ipaddrP2, 
                        uint8_t  ipaddrP3,
                       uint16_t  port,
                  const uint8_t *clientId,
                  const uint8_t *clientUser,
                  const uint8_t *clientPass,
                  const uint8_t *caCrt,
                   const size_t  caCrtLen,
                  const uint8_t *clientCrt,
                   const size_t  clientCrtLen,
                  const uint8_t *clientKey,
                   const size_t  clientKeyLen,
                       uint16_t  keepAliveTime){
  
  mbedtls_net_init(NULL);
  IP4_ADDR(&ipaddr_, ipaddrP0, ipaddrP1, ipaddrP2, ipaddrP3);
  port_ = port;
  
  ci_.client_id   = (const char *)clientId;
  ci_.client_user = (const char *)clientUser;
  ci_.client_pass = (const char *)clientPass;
  ci_.will_topic  = 0;
  ci_.will_msg    = 0;
  ci_.will_qos    = 0;
  ci_.will_retain = 0;
  ci_.keep_alive  = keepAliveTime;
  doConnect_      = false;
  ci_.tls_config = altcp_tls_create_config_client_2wayauth((const unsigned char*)caCrt, caCrtLen,
                                                           (const unsigned char*)clientKey, clientKeyLen, 0, 0,
                                                           (const unsigned char*)clientCrt, clientCrtLen);
  //to do check if tls congig init ok
  doConnect_ = true;
    
  publishTimeout_ = 0;
}

void MqttClient_t::connectionCb(mqtt_client_t *client, 
                                void *arg, 
                                mqtt_connection_status_t status){
  err_t err;
  if(status == MQTT_CONNECT_ACCEPTED) {
    printf("mqtt_connection_cb: Successfully connected\n");
    
    /* Setup callback for incoming publish requests */
    mqtt_set_inpub_callback(client, incomingPublishCb, incomingDataCb, arg);
    
    /* Subscribe to a topic named "subtopic" with QoS level 1, call mqtt_sub_request_cb with result */ 
    err = mqtt_subscribe(client, "subtopic", 1, subRequestCb, arg);

    if(err != ERR_OK) {
      printf("mqtt_subscribe return: %d\n", err);
    }
    object_->connected_ = true;
  } else {
    printf("mqtt_connection_cb: Disconnected, reason: %d\n", status);
    
    /* Its more nice to be connected, so try to reconnect */
    object_->doConnect();
  }  
}

/*Callbacs realized through static methods. Work with no-static class members realized through 
pointer to active object. Maybe there is better solution*/
void MqttClient_t::incomingPublishCb(void *arg, const char *topic, u32_t tot_len){
  printf("Incoming publish at topic %s with total length %u\n", topic, (unsigned int)tot_len);

  /* Decode topic string into a user defined reference */
  if(strcmp(topic, "print_payload") == 0) {
    object_->inpubId_ = 0;
  } else if(topic[0] == 'A') {
    /* All topics starting with 'A' might be handled at the same way */
    object_->inpubId_ = 1;
  } else {
    /* For all other topics */
    object_->inpubId_ = 2;
  }
}

void MqttClient_t::incomingDataCb(void *arg, const u8_t *data, u16_t len, u8_t flags){
  printf("Incoming publish payload with length %d, flags %u\n", len, (unsigned int)flags);

  if(flags & MQTT_DATA_FLAG_LAST) {
    /* Last fragment of payload received (or whole part if payload fits receive buffer
       See MQTT_VAR_HEADER_BUFFER_LEN)  */

    /* Call function or do action depending on reference, in this case inpub_id */
    if(object_->inpubId_ == 0) {
      /* Don't trust the publisher, check zero termination */
      if(data[len-1] == 0) {
        printf("mqtt_incoming_data_cb: %s\n", (const char *)data);
      }
    } else if(object_->inpubId_ == 1) {
      /* Call an 'A' function... */
    } else {
      printf("mqtt_incoming_data_cb: Ignoring payload...\n");
    }
  } else {
    /* Handle fragmented payload, store in buffer, write to file or whatever */
  }
}

void MqttClient_t::subRequestCb(void *arg, err_t err){
  //to do
}

void MqttClient_t::subRequest(void *arg, err_t result)
{
  /* Just print the result code here for simplicity, 
     normal behaviour would be to take some action if subscribe fails like 
     notifying user, retry subscribe or disconnect from server */
  printf("Subscribe result: %d\n", result);
}

/*Called when publish is complete either with sucess or failure */
void MqttClient_t::pubRequestCb(void *arg, err_t result){
  if(result != ERR_OK) {
    printf("Publish result: %d\n", result);
    object_->state_ = IDLE;
  }
  object_->doPublish_ = false;
}

/*Simple publishing private method*/
void MqttClient_t::doPublish(void){
  err_t err;
  u8_t qos = 2;    /* 0 1 or 2, see MQTT specification */
  u8_t retain = 0; /* No don't retain such crappy payload... */
  err = mqtt_publish(client_, publishTopic_, publishPayload_, strlen(publishPayload_), qos, retain, pubRequestCb, &arg_);
  if(err != ERR_OK) {
    printf("Publish err: %d\n", err);
  }
  else{
    printf("Mqtt publishing\n");
    publishTimeout_ = 10;
    state_ = PUBLISHING;
  }
}

/*Check if mqtt is connected*/
bool MqttClient_t::isConnected(void){
  return mqtt_client_is_connected(client_);
}

/*Make connect to broker*/
void MqttClient_t::doConnect(void){
  if(client_ == NULL){
    client_ = mqtt_client_new();
    if(client_ == NULL)
      printf("mqtt: client allocation fails\n");
    else
      printf("mqtt: client allocation success\n");
  }
  
  err_t err;
  
  err = mqtt_client_connect(client_, &ipaddr_, port_, connectionCb, 0, &ci_);
  
  if(err != ERR_OK){
    printf("mqtt_connect return %d\n", err);
    connected_ = false;
    state_ = IDLE;
  }
  else{
    printf("mqtt_connect OK \n");
    connected_ = true;
  }
}

/*Do disconnect from broker*/
void MqttClient_t::doDisconnect(void){
  mqtt_disconnect(client_);
  connected_ = false;
}

/*Public publishing method for external calls*/
bool MqttClient_t::publish(const char *publishTopic, const char *publishPayload){
  if(doPublish_)
    return true;
  publishTopic_   = publishTopic;
  publishPayload_ = publishPayload;
  doPublish_      = true;
  return false;
}

/*Main logic machine*/
void MqttClient_t::poll(void){
  switch(state_){
  case DISCONNECTING:
    if(!connected_){
      doDisconnect_ = false;
      state_ = IDLE;
    }
    break;
  case CONNECTING:
    if(connected_){
      doConnect_ = false;
      state_ = IDLE;
    }
    break;
  case PUBLISHING:
    if((!doPublish_) || (publishTimeout_ == 0))
      state_ = IDLE;
    else if(publishTimeout_ > 0)
      --publishTimeout_;
    break;
  case IDLE:
    if(doConnect_){
      state_ = CONNECTING;
      doConnect();
      }
    else if(doDisconnect_){
      state_ = DISCONNECTING;
      doDisconnect();
      }
    else if(doPublish_){
      doPublish();
    }
    break;
  default:
    printf("MqttClient_t error: default poll case\n");
    break;
  }

}