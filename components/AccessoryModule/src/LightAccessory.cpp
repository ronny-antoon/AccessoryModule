#include "LightAccessory.hpp"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char * TAG = "LightAccessory";

LightAccessory::LightAccessory(RelayModuleInterface * relayModule, ButtonModuleInterface * buttonModule) :
    m_relayModule(relayModule), m_buttonModule(buttonModule), m_reportCallback(nullptr), m_reportCallbackParam(nullptr),
    m_identifyHandler(nullptr)
{
    ESP_LOGI(TAG, "LightAccessory created");
    if (m_buttonModule)
    {
        m_buttonModule->setSinglePressCallback(buttonCallback, this);
    }
}

LightAccessory::~LightAccessory()
{
    ESP_LOGI(TAG, "LightAccessory destroyed");

    if (m_identifyHandler)
    {
        vTaskDelete(m_identifyHandler);
        m_identifyHandler = nullptr;
    }
}

void LightAccessory::setPowerState(bool powerState)
{
    ESP_LOGI(TAG, "Setting power to %s", powerState ? "ON" : "OFF");
    if (m_identifyHandler)
    {
        ESP_LOGW(TAG, "setPower called, but identify in progress");
        return;
    }

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

    if (m_identifyHandler)
    {
        ESP_LOGW(TAG, "Identify task already running");
        return;
    }

    if (!m_relayModule)
    {
        ESP_LOGW(TAG, "Relay module not set, cannot identify");
        return;
    }

    xTaskCreate(
        [](void * instance) {
            ESP_LOGD(TAG, "Starting identification sequence");
            LightAccessory * lightAccessory = static_cast<LightAccessory *>(instance);

            lightAccessory->m_relayModule->setPower(false);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            lightAccessory->m_relayModule->setPower(true);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            lightAccessory->m_relayModule->setPower(false);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            lightAccessory->m_relayModule->setPower(true);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            lightAccessory->m_relayModule->setPower(false);

            ESP_LOGD(TAG, "Identification sequence complete");

            lightAccessory->m_identifyHandler = nullptr;
            vTaskDelete(nullptr);
        },
        "identify", CONFIG_A_M_LIGHT_ACCESSORY_IDENTIFY_STACK_SIZE, this, CONFIG_A_M_LIGHT_ACCESSORY_IDENTIFY_PRIORITY,
        &m_identifyHandler);
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
        lightAccessory->m_reportCallback(lightAccessory->m_reportCallbackParam, false);
    }
}
