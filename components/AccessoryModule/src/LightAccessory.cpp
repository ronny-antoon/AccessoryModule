#include "LightAccessory.hpp"

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char * TAG = "LightAccessory";

LightAccessory::LightAccessory(RelayModuleInterface * relayModule, ButtonModuleInterface * buttonModule) :
    m_relayModule(relayModule), m_buttonModule(buttonModule)
{
    ESP_LOGI(TAG, "LightAccessory created");
    m_buttonModule->setSinglePressCallback(buttonCallback, this);
}

LightAccessory::~LightAccessory()
{
    ESP_LOGI(TAG, "LightAccessory destroyed");
}

void LightAccessory::setPowerState(bool powerState)
{
    ESP_LOGI(TAG, "Setting power to %s", powerState ? "ON" : "OFF");
    if (m_relayModule)
    {
        m_relayModule->setPower(powerState);
    }
    else
    {
        ESP_LOGW(TAG, "setPowerState called, but m_relayModule is nullptr");
    }
}

bool LightAccessory::isPowerOn()
{
    if (m_relayModule)
    {
        bool powerState = m_relayModule->isOn();
        ESP_LOGI(TAG, "Getting power state: %s", powerState ? "ON" : "OFF");
        return powerState;
    }
    else
    {
        ESP_LOGW(TAG, "isPowerOn called, but m_relayModule is nullptr");
        return false;
    }
}

void LightAccessory::setReportCallback(ReportCallback callback, CallbackParam * callbackParam)
{
    ESP_LOGI(TAG, "Setting report callback");
    m_reportCallback      = callback;
    m_reportCallbackParam = callbackParam;
}

void LightAccessory::identify()
{
    ESP_LOGI(TAG, "Identifying LightAccessory");
    xTaskCreate(
        [](void * instance) {
            ESP_LOGD(TAG, "Starting identification sequence");
            LightAccessory * lightAccessory = static_cast<LightAccessory *>(instance);

            lightAccessory->setPowerState(false);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            lightAccessory->setPowerState(true);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            lightAccessory->setPowerState(false);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            lightAccessory->setPowerState(true);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            lightAccessory->setPowerState(false);

            ESP_LOGD(TAG, "Identification sequence complete");
            vTaskDelete(nullptr);
        },
        "identify", 2048, this, 5, nullptr);
}

void LightAccessory::buttonCallback(void * instance)
{
    LightAccessory * lightAccessory = static_cast<LightAccessory *>(instance);
    bool newPowerState              = !lightAccessory->isPowerOn();
    ESP_LOGI(TAG, "Button pressed, toggling power to %s", newPowerState ? "ON" : "OFF");

    lightAccessory->setPowerState(newPowerState);
    if (lightAccessory->m_reportCallback)
    {
        ESP_LOGD(TAG, "Invoking report callback");
        lightAccessory->m_reportCallback(lightAccessory->m_reportCallbackParam);
    }
}
