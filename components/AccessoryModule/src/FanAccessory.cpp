#include "FanAccessory.hpp"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char * TAG = "FanAccessory";

FanAccessory::FanAccessory(RelayModuleInterface * relayModule, ButtonModuleInterface * buttonModule) :
    m_relayModule(relayModule), m_buttonModule(buttonModule), m_reportCallback(nullptr), m_reportCallbackParam(nullptr),
    m_identifyHandler(nullptr)
{
    ESP_LOGI(TAG, "FanAccessory created");
    if (m_buttonModule)
    {
        m_buttonModule->setSinglePressCallback(buttonCallback, this);
    }
}

FanAccessory::~FanAccessory()
{
    ESP_LOGI(TAG, "FanAccessory destroyed");
}

void FanAccessory::setPower(bool power)
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
    m_reportCallback      = callback;
    m_reportCallbackParam = callbackParam;
}

void FanAccessory::identify()
{
    ESP_LOGI(TAG, "Identifying FanAccessory");

    if (m_identifyHandler)
    {
        ESP_LOGW(TAG, "Identification sequence already running");
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
            FanAccessory * fanAccessory = static_cast<FanAccessory *>(instance);

            fanAccessory->m_relayModule->setPower(false);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            fanAccessory->m_relayModule->setPower(true);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            fanAccessory->m_relayModule->setPower(false);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            fanAccessory->m_relayModule->setPower(true);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            fanAccessory->m_relayModule->setPower(false);

            ESP_LOGD(TAG, "Identification sequence complete");

            fanAccessory->m_identifyHandler = nullptr;
            vTaskDelete(nullptr);
        },
        "identify", CONFIG_A_M_FAN_ACCESSORY_IDENTIFY_STACK_SIZE, this, CONFIG_A_M_FAN_ACCESSORY_IDENTIFY_PRIORITY,
        &m_identifyHandler);
}

void FanAccessory::buttonCallback(void * instance)
{
    FanAccessory * fanAccessory = static_cast<FanAccessory *>(instance);
    bool newPowerState          = !fanAccessory->getPower();
    ESP_LOGI(TAG, "Button pressed, toggling power to %s", newPowerState ? "ON" : "OFF");

    fanAccessory->setPower(newPowerState);
    if (fanAccessory->m_reportCallback)
    {
        ESP_LOGD(TAG, "Invoking report callback");
        fanAccessory->m_reportCallback(fanAccessory->m_reportCallbackParam, false);
    }
}
