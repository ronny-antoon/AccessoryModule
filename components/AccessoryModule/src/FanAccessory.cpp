#include "FanAccessory.hpp"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char * TAG = "FanAccessory";

FanAccessory::FanAccessory(RelayModuleInterface * relayModule, ButtonModuleInterface * buttonModule) :
    m_relayModule(relayModule), m_buttonModule(buttonModule)
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
    xTaskCreate(
        [](void * instance) {
            ESP_LOGD(TAG, "Starting identification sequence");
            FanAccessory * fanAccessory = static_cast<FanAccessory *>(instance);

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
            vTaskDelete(nullptr);
        },
        "identify", CONFIG_A_M_FAN_ACCESSORY_IDENTIFY_STACK_SIZE, this, CONFIG_A_M_FAN_ACCESSORY_IDENTIFY_PRIORITY, nullptr);
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
        fanAccessory->m_reportCallback(fanAccessory->m_reportCallbackParam);
    }
}
