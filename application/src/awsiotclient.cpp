#include "awsiotclient.hpp"

#include "aws_iot_config.h"
#include "aws_iot_mqtt_client.h"
#include "aws_iot_mqtt_client_interface.h"
#include "aws_iot_version.h"

namespace app {

AwsIotClient::AwsIotClient(const std::string_view& clientId)
    : clientId_(clientId),
      clientHandler_(static_cast<Handler*>(new AWS_IoT_Client{}),
                     deleteHandler_) {}

common::Error AwsIotClient::init(char* hostUrl,
                                 const Certificates certificates) {
  IoT_Client_Init_Params mqttInitParams = iotClientInitParamsDefault;
  mqttInitParams.enableAutoReconnect = false;
  mqttInitParams.pHostURL = hostUrl;
  mqttInitParams.port = CONFIG_AWS_IOT_MQTT_PORT;

  mqttInitParams.pRootCALocation =
      reinterpret_cast<const char*>(certificates.rootCA);
  mqttInitParams.pDeviceCertLocation =
      reinterpret_cast<const char*>(certificates.cert);
  mqttInitParams.pDevicePrivateKeyLocation =
      reinterpret_cast<const char*>(certificates.privateKey);

  mqttInitParams.mqttCommandTimeout_ms = MQTT_COMMAND_TIMEOUT_MS;
  mqttInitParams.tlsHandshakeTimeout_ms = TLS_HANDSHAKE_TIMEOUT_MS;
  mqttInitParams.isSSLHostnameVerify = true;
  mqttInitParams.disconnectHandler = [](AWS_IoT_Client* awsClient, void* arg) {
    assert(arg);
    AwsIotClient* client = static_cast<AwsIotClient*>(arg);
    client->disconnectCb_(client->disconnectArg_);
  };
  mqttInitParams.disconnectHandlerData = this;

  AWS_IoT_Client* client = static_cast<AWS_IoT_Client*>(clientHandler_.get());
  IoT_Error_t iotErrorCode = aws_iot_mqtt_init(client, &mqttInitParams);
  if (iotErrorCode != SUCCESS) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

void AwsIotClient::setDisconnectCallback(disconnectCallback cb,
                                         common::Argument arg) {
  disconnectCb_ = cb;
  disconnectArg_ = arg;
}

common::Error AwsIotClient::subscribe(const std::string_view& topic,
                                      const Qos qos, subscribeCallback cb,
                                      common::Argument arg) {
  subscribeCallbacks_[topic] = {cb, arg};
  SubscribeCallbackData* data = &subscribeCallbacks_[topic];

  IoT_Error_t iotErrorCode = aws_iot_mqtt_subscribe(
      static_cast<AWS_IoT_Client*>(clientHandler_.get()), topic.data(),
      topic.size(), static_cast<QoS>(qos),
      [](AWS_IoT_Client* client, char* topicName, uint16_t topicNameLen,
         IoT_Publish_Message_Params* params, void* arg) {
        assert(arg);
        SubscribeCallbackData* data = static_cast<SubscribeCallbackData*>(arg);
        if (data->cb) {
          data->cb(topicName, topicNameLen, params->payload, params->payloadLen,
                   data->arg);
        }
      },
      data);

  if (iotErrorCode != SUCCESS) {
    subscribeCallbacks_.erase(topic);
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

common::Error AwsIotClient::connect() {
  IoT_Client_Connect_Params connectParams = iotClientConnectParamsDefault;
  connectParams.keepAliveIntervalInSec = MQTT_KEEP_ALIVE_INTERVAL_S;
  connectParams.isCleanSession = true;
  connectParams.MQTTVersion = MQTT_3_1_1;

  connectParams.pClientID = clientId_.data();
  connectParams.clientIDLen = clientId_.size();
  connectParams.isWillMsgPresent = false;

  IoT_Error_t iotErrorCode = aws_iot_mqtt_connect(
      static_cast<AWS_IoT_Client*>(clientHandler_.get()), &connectParams);
  if (iotErrorCode != SUCCESS) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

common::Error AwsIotClient::disconnect() {
  IoT_Error_t iotErrorCode = aws_iot_mqtt_disconnect(
      static_cast<AWS_IoT_Client*>(clientHandler_.get()));
  if (iotErrorCode != SUCCESS) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

common::Error AwsIotClient::publish(const std::string_view& topic,
                                    char* payload, size_t payloadSize) {
  if (payload == nullptr || payloadSize > PAYLOAD_SIZE) {
    return common::Error::INVALID_ARG;
  }

  IoT_Publish_Message_Params publishMessageParams{};
  publishMessageParams.qos = QOS1;
  publishMessageParams.payloadLen = payloadSize;
  publishMessageParams.payload = payload;
  publishMessageParams.isRetained = 0;

  IoT_Error_t iotErrorCode =
      aws_iot_mqtt_publish(static_cast<AWS_IoT_Client*>(clientHandler_.get()),
                           topic.data(), topic.size(), &publishMessageParams);
  if (iotErrorCode != SUCCESS) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

void AwsIotClient::yield() {
  aws_iot_mqtt_yield(static_cast<AWS_IoT_Client*>(clientHandler_.get()),
                     YIELD_TIMEOUT_MS);
}

void AwsIotClient::deleteHandler_(Handler* clientHandler) {
  if (clientHandler) {
    delete static_cast<AWS_IoT_Client*>(clientHandler);
  }
}

} // namespace app
