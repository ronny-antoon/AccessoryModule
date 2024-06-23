#include "FanAccessory.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char * TAG = "FanAccessory";

FanAccessory::FanAccessory(RelayModuleInterface * relayModule, ButtonModuleInterface * buttonModule) :
    m_relayModule(relayModule), m_buttonModule(buttonModule)
{
    ESP_LOGI(TAG, "FanAccessory created");
    m_buttonModule->onSinglePress(buttonFunction, this);
}

FanAccessory::~FanAccessory()
{
    ESP_LOGI(TAG, "FanAccessory destroyed");
}

void FanAccessory::setPower(bool power)
{
    ESP_LOGI(TAG, "Setting power to %s", power ? "ON" : "OFF");
    if (m_relayModule)
    {
        m_relayModule->setPower(power);
    }
    else
    {
        ESP_LOGW(TAG, "setPower called, but m_relayModule is nullptr");
    }
}

bool FanAccessory::getPower()
{
    if (m_relayModule)
    {
        bool powerState = m_relayModule->isOn();
        ESP_LOGI(TAG, "Getting power state: %s", powerState ? "ON" : "OFF");
        return powerState;
    }
    else
    {
        ESP_LOGW(TAG, "getPower called, but m_relayModule is nullptr");
        return false;
    }
}

void FanAccessory::setReportCallback(ReportCallback callback, CallbackParam * callbackParam)
{
    ESP_LOGI(TAG, "Setting report callback");
    m_reportAttributesCallback          = callback;
    m_reportAttributesCallbackParameter = callbackParam;
}

void FanAccessory::identify()
{
    ESP_LOGI(TAG, "Identifying FanAccessory");
    // Run task for 3 seconds to blink the LED
    xTaskCreate(
        [](void * self) {
            ESP_LOGD(TAG, "Starting identification sequence");
            FanAccessory * fanAccessory = static_cast<FanAccessory *>(self);
            fanAccessory->setPower(false);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            fanAccessory->setPower(true);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            fanAccessory->setPower(false);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            fanAccessory->setPower(true);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            fanAccessory->setPower(false);

            ESP_LOGD(TAG, "Identification sequence complete");
            // Delete the task
            vTaskDelete(nullptr);
        },
        "identify", 2048, this, 5, nullptr);
}

void FanAccessory::buttonFunction(void * self)
{
    FanAccessory * fanAccessory = static_cast<FanAccessory *>(self);
    bool newPowerState          = !fanAccessory->getPower();
    ESP_LOGI(TAG, "Button pressed, toggling power to %s", newPowerState ? "ON" : "OFF");

    fanAccessory->setPower(newPowerState);
    if (fanAccessory->m_reportAttributesCallback)
    {
        ESP_LOGD(TAG, "Invoking report callback");
        fanAccessory->m_reportAttributesCallback(fanAccessory->m_reportAttributesCallbackParameter);
    }
}
