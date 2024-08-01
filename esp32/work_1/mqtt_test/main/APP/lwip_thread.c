#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "lwip/apps/mqtt.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "mqtt_client.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_netif.h"
#include "lcd.h"
#include "cJSON.h"
#include "thread_init.h"
#include "lwip_thread.h"
#include "wifi_config.h"


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
            ESP_LOGI(__FUNCTION__, "TOPIC = %.*s\r\n", event->topic_len, event->topic);
            ESP_LOGI(__FUNCTION__, "DATA = %.*s\r\n", event->data_len, event->data);

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
void lwip_thread_demo(void)
{

}

static void lwip_init(esp_mqtt_client_handle_t *mqtt_client_handle, cJSON *json_handle)
{
    /* 设置客户端的信息量 */
    esp_mqtt_client_config_t mqtt_cfg = {
    .broker.address.hostname = HOST_NAME,                   /* MQTT地址 */
    .broker.address.port = HOST_PORT,                       /* MQTT端口号 */
    .broker.address.transport = MQTT_TRANSPORT_OVER_TCP,    /* TCP模式 */
    .credentials.client_id = CLIENT_ID,                     /* 设备名称 */
    .credentials.username = (char *)USER_NAME,               /* 产品ID */
    .credentials.authentication.password = PASSWORD,        /* 计算出来的密码 */
    };

    *mqtt_client_handle = esp_mqtt_client_init(&mqtt_cfg);

    esp_mqtt_client_register_event(*mqtt_client_handle, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(*mqtt_client_handle);
}

static void lwip_deinit(esp_mqtt_client_handle_t *mqtt_client_handle)
{
    esp_mqtt_client_stop(*mqtt_client_handle);
}

void lwip_send_demo(esp_mqtt_client_handle_t *mqtt_client_handle)
{
    char mqtt_publish_data[] =
    "{ \"params\":{\
		\"test_bufffer_send\":\"test_press_num = 123456\",\
		\"Temperature\":80,\
		\"OutputVoltage\":6\
        }\
	}";

    esp_mqtt_client_publish(*mqtt_client_handle, DEVICE_SENSOR_INFO_POS, mqtt_publish_data, strlen(mqtt_publish_data), 1, 0);
}

void lwip_send_once()
{

}

void lwip_thread(void *pvparams)
{
    esp_mqtt_client_handle_t mqtt_client_handle;
    thread_pvparam_t *thread_pvparam = (thread_pvparam_t *)pvparams;
    queue_temp_humi_t queue_temp_humi = {0};
    cJSON *mqtt_json_base = NULL, *mqtt_json_param = NULL;
    char *mqtt_buffer = NULL;

    wifi_sta_init();
    lwip_init(&mqtt_client_handle, mqtt_json_base);

    mqtt_json_base = cJSON_CreateObject();
    mqtt_json_param = cJSON_CreateObject();

    cJSON_AddItemToObject(mqtt_json_base, "params", mqtt_json_param);
    cJSON_AddItemToObject(mqtt_json_param, "Temperature", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(mqtt_json_param, "Humidity", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(mqtt_json_param, "illumination", cJSON_CreateNumber(0));


    while(1)
    {
        if (thread_pvparam->queue_temp_humi != NULL)
        {
            xQueueReceive(thread_pvparam->queue_temp_humi, &queue_temp_humi, 1000);
            cJSON_ReplaceItemInObject(mqtt_json_param, "Temperature", cJSON_CreateNumber(queue_temp_humi.temp_value));
            cJSON_ReplaceItemInObject(mqtt_json_param, "Humidity", cJSON_CreateNumber(queue_temp_humi.humi_value));
            mqtt_buffer = cJSON_Print(mqtt_json_base);
            esp_mqtt_client_publish(mqtt_client_handle, DEVICE_SENSOR_INFO_POS, mqtt_buffer, strlen(mqtt_buffer), 1, 0);
            cJSON_free(mqtt_buffer);
        }

        vTaskDelay(100);
    }

    lwip_deinit(&mqtt_client_handle);
}

