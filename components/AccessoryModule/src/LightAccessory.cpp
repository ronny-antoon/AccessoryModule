#include "LightAccessory.hpp"

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char * TAG = "LightAccessory";

LightAccessory::LightAccessory(RelayModuleInterface * relayModuleInterface, ButtonModuleInterface * buttonModuleInterface) :
    m_relayModuleInterface(relayModuleInterface), m_buttonModuleInterface(buttonModuleInterface)
{
    ESP_LOGI(TAG, "LightAccessory created");
    m_buttonModuleInterface->onSinglePress(buttonCallback, this);
}

LightAccessory::~LightAccessory()
{
    ESP_LOGI(TAG, "LightAccessory destroyed");
}

void LightAccessory::setPowerState(bool powerState)
{
    ESP_LOGI(TAG, "Setting power to %s", powerState ? "ON" : "OFF");
    if (m_relayModuleInterface)
    {
        m_relayModuleInterface->setPower(powerState);
    }
    else
    {
        ESP_LOGW(TAG, "setPowerState called, but m_relayModuleInterface is nullptr");
    }
}

bool LightAccessory::isPowerOn()
{
    if (m_relayModuleInterface)
    {
        bool powerState = m_relayModuleInterface->isOn();
        ESP_LOGI(TAG, "Getting power state: %s", powerState ? "ON" : "OFF");
        return powerState;
    }
    else
    {
        ESP_LOGW(TAG, "isPowerOn called, but m_relayModuleInterface is nullptr");
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
    // Run task for 3 seconds to blink the LED
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

            // Delete the task
            vTaskDelete(nullptr);
        },
        "identify", 2048, this, 5, nullptr);
}

void LightAccessory::buttonCallback(void * instance)
{
    LightAccessory * lightAccessory = static_cast<LightAccessory *>(instance);
    bool newState                   = !lightAccessory->isPowerOn();
    ESP_LOGI(TAG, "Button pressed, toggling power to %s", newState ? "ON" : "OFF");

    lightAccessory->setPowerState(newState);
    if (lightAccessory->m_reportCallback)
    {
        ESP_LOGD(TAG, "Invoking report callback");
        lightAccessory->m_reportCallback(lightAccessory->m_reportCallbackParam);
    }
}
