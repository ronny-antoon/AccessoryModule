#include "StatelessButtonAccessory.hpp"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char * TAG = "StatelessButtonAccessory";

StatelessButtonAccessory::StatelessButtonAccessory(ButtonModuleInterface * buttonModule) :
    m_buttonModule(buttonModule), m_lastPressType(SinglePress), m_reportCallback(nullptr), m_reportCallbackParam(nullptr)
{
    ESP_LOGI(TAG, "StatelessButtonAccessory created");

    if (m_buttonModule)
    {
        m_buttonModule->setSinglePressCallback(
            [](void * instance) {
                handlePress(instance, StatelessButtonAccessoryInterface::PressType::SinglePress, "Single press detected");
            },
            this);

        m_buttonModule->setDoublePressCallback(
            [](void * instance) {
                handlePress(instance, StatelessButtonAccessoryInterface::PressType::DoublePress, "Double press detected");
            },
            this);

        m_buttonModule->setLongPressCallback(
            [](void * instance) {
                handlePress(instance, StatelessButtonAccessoryInterface::PressType::LongPress, "Long press detected");
            },
            this);
    }
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

void StatelessButtonAccessory::handlePress(void * instance, StatelessButtonAccessoryInterface::PressType pressType,
                                           const char * logMessage)
{
    StatelessButtonAccessory * statelessButtonAccessory = static_cast<StatelessButtonAccessory *>(instance);
    statelessButtonAccessory->m_lastPressType           = pressType;
    ESP_LOGI(TAG, "%s", logMessage);
    if (statelessButtonAccessory->m_reportCallback)
    {
        statelessButtonAccessory->m_reportCallback(statelessButtonAccessory->m_reportCallbackParam, false);
    }
}
