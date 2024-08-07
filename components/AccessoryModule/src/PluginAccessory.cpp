#include "PluginAccessory.hpp"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char * TAG = "PluginAccessory";

PluginAccessory::PluginAccessory(RelayModuleInterface * relayModuleInterface, ButtonModuleInterface * buttonModuleInterface) :
    m_relayModuleInterface(relayModuleInterface), m_buttonModuleInterface(buttonModuleInterface), m_reportCallback(nullptr),
    m_reportCallbackParam(nullptr), m_identifyHandler(nullptr)
{
    ESP_LOGI(TAG, "PluginAccessory created");
    if (m_buttonModuleInterface)
    {
        m_buttonModuleInterface->setSinglePressCallback(buttonCallback, this);
    }
}

PluginAccessory::~PluginAccessory()
{
    ESP_LOGI(TAG, "PluginAccessory destroyed");
}

void PluginAccessory::setPower(bool power)
{
    ESP_LOGI(TAG, "Setting power to %s", power ? "ON" : "OFF");
    if (m_identifyHandler)
    {
        ESP_LOGW(TAG, "setPower called, but identify in progress");
        return;
    }
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

    if (m_identifyHandler)
    {
        ESP_LOGW(TAG, "Identify task already running");
        return;
    }

    if (!m_relayModuleInterface)
    {
        ESP_LOGW(TAG, "Relay module not available, cannot identify");
        return;
    }

    xTaskCreate(
        [](void * instance) {
            ESP_LOGD(TAG, "Starting identification sequence");
            PluginAccessory * pluginAccessory = static_cast<PluginAccessory *>(instance);

            pluginAccessory->m_relayModuleInterface->setPower(false);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            pluginAccessory->m_relayModuleInterface->setPower(true);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            pluginAccessory->m_relayModuleInterface->setPower(false);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            pluginAccessory->m_relayModuleInterface->setPower(true);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            pluginAccessory->m_relayModuleInterface->setPower(false);

            ESP_LOGD(TAG, "Identification sequence complete");

            pluginAccessory->m_identifyHandler = nullptr;
            vTaskDelete(nullptr);
        },
        "identify", CONFIG_A_M_PLUGIN_ACCESSORY_IDENTIFY_STACK_SIZE, this, CONFIG_A_M_PLUGIN_ACCESSORY_IDENTIFY_PRIORITY,
        &m_identifyHandler);
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
        pluginAccessory->m_reportCallback(pluginAccessory->m_reportCallbackParam, false);
    }
}
