#include "StatelessButtonAccessory.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char * TAG = "StatelessButtonAccessory";

StatelessButtonAccessory::StatelessButtonAccessory(ButtonModuleInterface * buttonModule) :
    m_buttonModule(buttonModule), m_lastPressType(SinglePress)
{
    ESP_LOGI(TAG, "StatelessButtonAccessory created");

    m_buttonModule->onSinglePress(
        [](void * self) {
            StatelessButtonAccessory * statelessButtonAccessory = static_cast<StatelessButtonAccessory *>(self);
            statelessButtonAccessory->m_lastPressType           = StatelessButtonAccessoryInterface::PressType::SinglePress;
            ESP_LOGI(TAG, "Single press detected");
            if (statelessButtonAccessory->m_reportCallback)
            {
                statelessButtonAccessory->m_reportCallback(statelessButtonAccessory->m_reportCallbackParam);
            }
        },
        this);

    m_buttonModule->onDoublePress(
        [](void * self) {
            StatelessButtonAccessory * statelessButtonAccessory = static_cast<StatelessButtonAccessory *>(self);
            statelessButtonAccessory->m_lastPressType           = StatelessButtonAccessoryInterface::PressType::DoublePress;
            ESP_LOGI(TAG, "Double press detected");
            if (statelessButtonAccessory->m_reportCallback)
            {
                statelessButtonAccessory->m_reportCallback(statelessButtonAccessory->m_reportCallbackParam);
            }
        },
        this);

    m_buttonModule->onLongPress(
        [](void * self) {
            StatelessButtonAccessory * statelessButtonAccessory = static_cast<StatelessButtonAccessory *>(self);
            statelessButtonAccessory->m_lastPressType           = StatelessButtonAccessoryInterface::PressType::LongPress;
            ESP_LOGI(TAG, "Long press detected");
            if (statelessButtonAccessory->m_reportCallback)
            {
                statelessButtonAccessory->m_reportCallback(statelessButtonAccessory->m_reportCallbackParam);
            }
        },
        this);
}

StatelessButtonAccessory::~StatelessButtonAccessory()
{
    ESP_LOGI(TAG, "StatelessButtonAccessory destroyed");
}

void StatelessButtonAccessory::setReportCallback(ReportCallback callback, CallbackParam * callbackParam)
{
    ESP_LOGI(TAG, "Setting report callback");
    m_reportCallback      = callback;
    m_reportCallbackParam = callbackParam;
}

void StatelessButtonAccessory::identify()
{
    ESP_LOGI(TAG, "Identifying StatelessButtonAccessory");
    // Implement the identification logic if needed
}

StatelessButtonAccessoryInterface::PressType StatelessButtonAccessory::getLastPressType()
{
    ESP_LOGI(TAG, "Getting last press type: %d", m_lastPressType);
    return m_lastPressType;
}
