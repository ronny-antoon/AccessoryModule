#include "PluginAccessory.hpp"

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char * TAG = "PluginAccessory";

PluginAccessory::PluginAccessory(RelayModuleInterface * relayModuleInterface, ButtonModuleInterface * buttonModuleInterface) :
    m_relayModuleInterface(relayModuleInterface), m_buttonModuleInterface(buttonModuleInterface)
{
    ESP_LOGI(TAG, "PluginAccessory created");
    m_buttonModuleInterface->setSinglePressCallback(buttonCallback, this);
}

PluginAccessory::~PluginAccessory()
{
    ESP_LOGI(TAG, "PluginAccessory destroyed");
}

void PluginAccessory::setPower(bool power)
{
    ESP_LOGI(TAG, "Setting power to %s", power ? "ON" : "OFF");
    if (m_relayModuleInterface)
    {
        m_relayModuleInterface->setPower(power);
    }
    else
    {
        ESP_LOGW(TAG, "setPower called, but m_relayModuleInterface is nullptr");
    }
}

bool PluginAccessory::getPower()
{
    if (m_relayModuleInterface)
    {
        bool powerState = m_relayModuleInterface->isOn();
        ESP_LOGI(TAG, "Getting power state: %s", powerState ? "ON" : "OFF");
        return powerState;
    }
    else
    {
        ESP_LOGW(TAG, "getPower called, but m_relayModuleInterface is nullptr");
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
        [](void * instance) {
            ESP_LOGD(TAG, "Starting identification sequence");
            PluginAccessory * pluginAccessory = static_cast<PluginAccessory *>(instance);

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

void PluginAccessory::buttonCallback(void * instance)
{
    PluginAccessory * pluginAccessory = static_cast<PluginAccessory *>(instance);
    bool newPowerState                = !pluginAccessory->getPower();
    ESP_LOGI(TAG, "Button pressed, toggling power to %s", newPowerState ? "ON" : "OFF");

    pluginAccessory->setPower(newPowerState);
    if (pluginAccessory->m_reportCallback)
    {
        ESP_LOGD(TAG, "Invoking report callback");
        pluginAccessory->m_reportCallback(pluginAccessory->m_reportCallbackParam);
    }
}
