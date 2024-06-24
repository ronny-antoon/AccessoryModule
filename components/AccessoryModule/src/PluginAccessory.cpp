#include "PluginAccessory.hpp"

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char * TAG = "PluginAccessory";

PluginAccessory::PluginAccessory(RelayModuleInterface * relayModule, ButtonModuleInterface * buttonModule) :
    m_relayModule(relayModule), m_buttonModule(buttonModule)
{
    ESP_LOGI(TAG, "PluginAccessory created");
    m_buttonModule->setSinglePressCallback(buttonCallback, this);
}

PluginAccessory::~PluginAccessory()
{
    ESP_LOGI(TAG, "PluginAccessory destroyed");
}

void PluginAccessory::setPower(bool power)
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

bool PluginAccessory::getPower()
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

void PluginAccessory::setReportCallback(ReportCallback callback, CallbackParam * callbackParam)
{
    ESP_LOGI(TAG, "Setting report callback");
    m_reportCallback      = callback;
    m_reportCallbackParam = callbackParam;
}

void PluginAccessory::identify()
{
    ESP_LOGI(TAG, "Identifying PluginAccessory");
    xTaskCreate(
        [](void * self) {
            ESP_LOGD(TAG, "Starting identification sequence");
            PluginAccessory * pluginAccessory = static_cast<PluginAccessory *>(self);

            pluginAccessory->setPower(false);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            pluginAccessory->setPower(true);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            pluginAccessory->setPower(false);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            pluginAccessory->setPower(true);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            pluginAccessory->setPower(false);

            ESP_LOGD(TAG, "Identification sequence complete");
            vTaskDelete(nullptr);
        },
        "identify", 2048, this, 5, nullptr);
}

void PluginAccessory::buttonCallback(void * self)
{
    PluginAccessory * pluginAccessory = static_cast<PluginAccessory *>(self);
    bool newPowerState                = !pluginAccessory->getPower();
    ESP_LOGI(TAG, "Button pressed, toggling power to %s", newPowerState ? "ON" : "OFF");

    pluginAccessory->setPower(newPowerState);
    if (pluginAccessory->m_reportCallback)
    {
        ESP_LOGD(TAG, "Invoking report callback");
        pluginAccessory->m_reportCallback(pluginAccessory->m_reportCallbackParam);
    }
}
