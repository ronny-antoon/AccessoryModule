#include "SwitchAccessory.hpp"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char * TAG = "SwitchAccessory";

SwitchAccessory::SwitchAccessory(RelayModuleInterface * relayModule, ButtonModuleInterface * buttonModule) :
    m_relayModule(relayModule), m_buttonModule(buttonModule), m_reportCallback(nullptr), m_reportCallbackParam(nullptr),
    m_identifyHandler(nullptr)
{
    ESP_LOGI(TAG, "SwitchAccessory created");
    if (m_buttonModule)
    {
        m_buttonModule->setSinglePressCallback(buttonCallback, this);
    }
}

SwitchAccessory::~SwitchAccessory()
{
    ESP_LOGI(TAG, "SwitchAccessory destroyed");
}

void SwitchAccessory::setPower(bool power)
{
    ESP_LOGI(TAG, "Setting power to %s", power ? "ON" : "OFF");
    if (m_identifyHandler)
    {
        ESP_LOGW(TAG, "setPower called, but identify in progress");
        return;
    }

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

    if (m_identifyHandler)
    {
        vTaskDelete(m_identifyHandler);
        m_identifyHandler = nullptr;
    }

    if (!m_relayModule)
    {
        ESP_LOGW(TAG, "identify called, but m_relayModule is nullptr");
        return;
    }

    xTaskCreate(
        [](void * instance) {
            ESP_LOGD(TAG, "Starting identification sequence");
            SwitchAccessory * switchAccessory = static_cast<SwitchAccessory *>(instance);

            switchAccessory->m_relayModule->setPower(false);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            switchAccessory->m_relayModule->setPower(true);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            switchAccessory->m_relayModule->setPower(false);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            switchAccessory->m_relayModule->setPower(true);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            switchAccessory->m_relayModule->setPower(false);

            ESP_LOGD(TAG, "Identification sequence complete");

            switchAccessory->m_identifyHandler = nullptr;
            vTaskDelete(nullptr);
        },
        "identify", CONFIG_A_M_SWITCH_ACCESSORY_IDENTIFY_STACK_SIZE, this, CONFIG_A_M_SWITCH_ACCESSORY_IDENTIFY_PRIORITY,
        &m_identifyHandler);
}

void SwitchAccessory::buttonCallback(void * instance)
{
    SwitchAccessory * switchAccessory = static_cast<SwitchAccessory *>(instance);
    bool newPowerState                = !switchAccessory->getPower();
    ESP_LOGI(TAG, "Button pressed, toggling power to %s", newPowerState ? "ON" : "OFF");

    switchAccessory->setPower(newPowerState);
    if (switchAccessory->m_reportCallback)
    {
        ESP_LOGD(TAG, "Invoking report callback");
        switchAccessory->m_reportCallback(switchAccessory->m_reportCallbackParam, false);
    }
}
