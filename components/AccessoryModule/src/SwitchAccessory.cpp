#include "SwitchAccessory.hpp"

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char * TAG = "SwitchAccessory";

SwitchAccessory::SwitchAccessory(RelayModuleInterface * relayModule, ButtonModuleInterface * buttonModule) :
    m_relayModule(relayModule), m_buttonModule(buttonModule)
{
    ESP_LOGI(TAG, "SwitchAccessory created");
    m_buttonModule->setSinglePressCallback(buttonCallback, this);
}

SwitchAccessory::~SwitchAccessory()
{
    ESP_LOGI(TAG, "SwitchAccessory destroyed");
}

void SwitchAccessory::setPower(bool power)
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

bool SwitchAccessory::getPower()
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

void SwitchAccessory::setReportCallback(ReportCallback callback, CallbackParam * callbackParam)
{
    ESP_LOGI(TAG, "Setting report callback");
    m_reportCallback      = callback;
    m_reportCallbackParam = callbackParam;
}

void SwitchAccessory::identify()
{
    ESP_LOGI(TAG, "Identifying SwitchAccessory");
    xTaskCreate(
        [](void * self) {
            ESP_LOGD(TAG, "Starting identification sequence");
            SwitchAccessory * switchAccessory = static_cast<SwitchAccessory *>(self);

            switchAccessory->setPower(false);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            switchAccessory->setPower(true);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            switchAccessory->setPower(false);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            switchAccessory->setPower(true);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            switchAccessory->setPower(false);

            ESP_LOGD(TAG, "Identification sequence complete");
            vTaskDelete(nullptr);
        },
        "identify", 2048, this, 5, nullptr);
}

void SwitchAccessory::buttonCallback(void * self)
{
    SwitchAccessory * switchAccessory = static_cast<SwitchAccessory *>(self);
    bool newPowerState                = !switchAccessory->getPower();
    ESP_LOGI(TAG, "Button pressed, toggling power to %s", newPowerState ? "ON" : "OFF");

    switchAccessory->setPower(newPowerState);
    if (switchAccessory->m_reportCallback)
    {
        ESP_LOGD(TAG, "Invoking report callback");
        switchAccessory->m_reportCallback(switchAccessory->m_reportCallbackParam);
    }
}
