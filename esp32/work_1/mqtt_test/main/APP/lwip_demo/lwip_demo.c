/**
 ****************************************************************************************************
 * @file        udp.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-08-26
 * @brief       LWIP实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 ESP32-S3 开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "lwip_demo.h"

int g_publish_flag = 0;/* 发布成功标志位 */
static const char *TAG = "MQTT_EXAMPLE";
char g_lcd_buff[100] = {0};

/**
 * @brief       错误日记
 * @param       message     :错误消息
 * @param       error_code  :错误码
 * @retval      无
 */
static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0)
    {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

/**
 * @brief       注册接收MQTT事件的事件处理程序
 * @param       handler_args:注册到事件的用户数据
 * @param       base        :处理程序的事件库
 * @param       event_id    :接收到的事件的id
 * @param       event_data  :事件的数据
 * @retval      无
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;

    switch ((esp_mqtt_event_id_t)event_id)
    {
        case MQTT_EVENT_CONNECTED:      /* 连接事件 */
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_publish(client, "/topic/qos1", "data_3", 0, 1, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
            ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
            g_publish_flag = 1;
            /* 订阅主题 */
            msg_id = esp_mqtt_client_subscribe(client, DEVICE_SUBSCRIBE, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED:   /* 断开连接事件 */

            break;

        case MQTT_EVENT_SUBSCRIBED:     /* 取消事件 */
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:   /* 取消订阅事件 */
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:      /* 发布事件 */
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:           /* 接收数据事件 */
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);

            break;
        case MQTT_EVENT_ERROR:

            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
            {
                log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
                log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
                log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
                ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
            }

            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}

/**
 * @brief       lwip_demo进程
 * @param       无
 * @retval      无
 */
void lwip_demo(void)
{
    char mqtt_publish_data[] =
    "{ \"params\":{\
		\"test_bufffer_send\":\"test_press_num = 123456\",\
		\"Temperature\":80,\
		\"OutputVoltage\":6\
        }\
	}";

    char *mqtt_buffer = NULL;
    uint16_t ir, als, ps;
    uint16_t temp = 0x00, humi = 0x00;
    cJSON *mqtt_bot = NULL, *mqtt_bot_1 = NULL;
    
    /* 设置客户端的信息量 */ 
    esp_mqtt_client_config_t mqtt_cfg = {
    .broker.address.hostname = HOST_NAME,                   /* MQTT地址 */
    .broker.address.port = HOST_PORT,                       /* MQTT端口号 */
    .broker.address.transport = MQTT_TRANSPORT_OVER_TCP,    /* TCP模式 */
    .credentials.client_id = CLIENT_ID,                     /* 设备名称 */
    .credentials.username = (char *)USER_NAME,               /* 产品ID */
    .credentials.authentication.password = PASSWORD,        /* 计算出来的密码 */
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);

    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

    temperature_sensor_init();

    mqtt_bot = cJSON_CreateObject();
    mqtt_bot_1 = cJSON_CreateObject();
    cJSON_AddItemToObject(mqtt_bot, "params", mqtt_bot_1);
    cJSON_AddItemToObject(mqtt_bot_1, "Temperature", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(mqtt_bot_1, "Humidity", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(mqtt_bot_1, "illumination", cJSON_CreateNumber(0));

    while(1)
    {
        ap3216c_read_data(&ir, &ps, &als);              /* 读取数据  */
        ATH20_read_data(&temp, &humi);
        cJSON_ReplaceItemInObject(mqtt_bot_1, "Temperature", cJSON_CreateNumber(temp/10.0));
        cJSON_ReplaceItemInObject(mqtt_bot_1, "Humidity", cJSON_CreateNumber(humi/10.0));
        cJSON_ReplaceItemInObject(mqtt_bot_1, "illumination", cJSON_CreateNumber(als));
        mqtt_buffer = cJSON_Print(mqtt_bot);
        ESP_LOGI("lwip", "%s\n", mqtt_buffer);
        if (g_publish_flag == 1)
        {
            esp_mqtt_client_publish(client, DEVICE_VALUE_POS, mqtt_buffer, strlen(mqtt_buffer), 1, 0);
        }

        lcd_show_num(0, 170, ir, 5, 16, BLUE);    /* 显示IR数据 */
        lcd_show_num(50, 170, ps, 5, 16, BLUE);    /* 显示PS数据 */
        lcd_show_num(100, 170, als, 5, 16, BLUE);   /* 显示ALS数据  */

        sprintf(g_lcd_buff, "temp %0.1f humi %0.1f%%", temp/10.0, humi/10.0);
        cJSON_free(mqtt_buffer);
        lcd_show_string(0, 210, 240, 16, 16, g_lcd_buff, RED);
        vTaskDelay(6000);
    }

    esp_mqtt_client_stop(client);
}

