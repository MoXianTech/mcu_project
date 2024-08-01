#ifndef __LWIP_THREAD_H__
#define __LWIP_THREAD_H__


/* 用户需要根据设备信息完善以下宏定义中的三元组内容 */
#define PRODUCT_KEY         "i98lEHgwEA1"                                                       /* ProductKey->阿里云颁发的产品唯一标识，11位长度的英文数字随机组合 */
#define DEVICE_NAME         "test_2"                                                       /* DeviceName->用户注册设备时生成的设备唯一编号，支持系统自动生成，也可支持用户添加自定义编号，产品维度内唯一  */
#define DEVICE_SECRET       "5d358af97c70cd8d5feb08d733df2656"                                  /* DeviceSecret->设备密钥，与DeviceName成对出现，可用于一机一密的认证方案  */
/* MQTT地址与端口 */
#define HOST_NAME           "iot-06z00ho0nths9re.mqtt.iothub.aliyuncs.com"                  /* 阿里云域名 */
#define HOST_PORT           1883                                                                /* 阿里云域名端口，固定1883 */
/* 根据三元组内容计算得出的数值 */
#define CLIENT_ID           "i98lEHgwEA1.test_2|securemode=2,signmethod=hmacsha256,timestamp=1721961835249|"  /* 客户端ID */
#define USER_NAME           DEVICE_NAME"&"PRODUCT_KEY                                           /* 客户端用户名 */
#define PASSWORD            "66aa130cebd5baa4721ae5b5798d370c4582760ce8be953f566b354febe9fcd0"                          /* 由MQTT_Password工具计算得出的连接密码 */
/* 发布与订阅 */
#define DEVICE_PUBLISH      "/"PRODUCT_KEY"/"DEVICE_NAME"/user/update"                          /* 发布 */
#define DEVICE_SUBSCRIBE    "/"PRODUCT_KEY"/"DEVICE_NAME"/user/get"                             /* 订阅 */

#define DEVICE_SENSOR_INFO_POS    "/sys/i98lEHgwEA1/"DEVICE_NAME"/thing/event/property/post"


void lwip_thread(void *pvparams);

#endif
