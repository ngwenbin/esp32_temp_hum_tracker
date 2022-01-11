/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#include "dht11.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "sdkconfig.h"

#define TAG "SENSOR"
#define SENSOR_PIN 32

void DHT_task(void *pvParameter) {
    DHT11_init(GPIO_NUM_27);

    while (1) {
        struct dht11_reading data = DHT11_read();

        switch (data.status) {
            case DHT11_CRC_ERROR:
                ESP_LOGE(TAG, "CRC ERROR, Error with checksum");
                break;

            case DHT11_TIMEOUT_ERROR:
                ESP_LOGE(TAG, "Timeout error");
                break;

            case DHT11_OK:
                ESP_LOGI(TAG, "Temp: %d, Hum: %d", data.temperature,
                         data.humidity);
                break;
        }

        vTaskDelay(3000 / portTICK_RATE_MS);
    }
}

void app_main(void) {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    xTaskCreate(&DHT_task, "DHT_task", 2048, NULL, 5, NULL);
}
