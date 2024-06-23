#include "DoorLockAccessory.hpp"

DoorLockAccessory::DoorLockAccessory(RelayModuleInterface * relayModule, ButtonModuleInterface * buttonModule, uint8_t openTime) :
    m_relayModule(relayModule), m_buttonModule(buttonModule), m_openTime(openTime), m_openDoorTaskHandle(nullptr)
{
    ESP_LOGI(TAG, "DoorLockAccessory created");
    m_buttonModule->onSinglePress(buttonFunction, this);
}

DoorLockAccessory::~DoorLockAccessory()
{
    ESP_LOGI(TAG, "DoorLockAccessory destroyed");
}

void DoorLockAccessory::setState(DoorLockState lock)
{
    if (lock == DoorLockState::LOCKED)
    {
        closeDoor();
    }
    else
    {
        openDoor();
    }
}

DoorLockAccessoryInterface::DoorLockState DoorLockAccessory::getState()
{
    return m_relayModule->isOn() ? DoorLockState::UNLOCKED : DoorLockState::LOCKED;
}

void DoorLockAccessory::setReportCallback(ReportCallback callback, CallbackParam * callbackParam)
{
    ESP_LOGI(TAG, "Setting report callback");
    m_reportAttributesCallback          = callback;
    m_reportAttributesCallbackParameter = callbackParam;
}

void DoorLockAccessory::identify()
{
    ESP_LOGI(TAG, "Identifying DoorLockAccessory");
    // Run task for 3 seconds to blink the LED
    xTaskCreate(
        [](void * self) {
            ESP_LOGD(TAG, "Starting identification sequence");
            DoorLockAccessory * doorLockAccessory = static_cast<DoorLockAccessory *>(self);
            doorLockAccessory->m_relayModule->setPower(false);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            doorLockAccessory->m_relayModule->setPower(true);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            doorLockAccessory->m_relayModule->setPower(false);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            doorLockAccessory->m_relayModule->setPower(true);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            doorLockAccessory->m_relayModule->setPower(false);

            ESP_LOGD(TAG, "Identification sequence complete");
            // delete the task
            vTaskDelete(nullptr);
        },
        "identify", 2048, this, 5, nullptr);
}

void DoorLockAccessory::buttonFunction(void * self)
{
    DoorLockAccessory * doorLockAccessory = static_cast<DoorLockAccessory *>(self);
    doorLockAccessory->setState(doorLockAccessory->getState() == DoorLockState::LOCKED ? DoorLockState::UNLOCKED
                                                                                       : DoorLockState::LOCKED);
}

void DoorLockAccessory::openDoor()
{
    if (getState() == DoorLockState::LOCKED)
    {
        m_relayModule->setPower(true);
        if (m_reportAttributesCallback)
        {
            m_reportAttributesCallback(m_reportAttributesCallbackParameter);
        }
        xTaskCreate(openDoorTask, "openDoor", 2048, this, 5, &m_openDoorTaskHandle);
    }
    if (getState() == DoorLockState::UNLOCKED && m_openDoorTaskHandle != nullptr)
    {
        vTaskDelete(m_openDoorTaskHandle);
        m_openDoorTaskHandle = nullptr;
        m_relayModule->setPower(true);
        xTaskCreate(openDoorTask, "openDoor", 2048, this, 5, &m_openDoorTaskHandle);
    }
}

void DoorLockAccessory::openDoorTask(void * pvParameters)
{
    DoorLockAccessory * doorLockAccessory = static_cast<DoorLockAccessory *>(pvParameters);
    vTaskDelay(doorLockAccessory->m_openTime * 1000 / portTICK_PERIOD_MS);
    doorLockAccessory->closeDoor();
    doorLockAccessory->m_openDoorTaskHandle = nullptr;
    vTaskDelete(nullptr);
}

void DoorLockAccessory::closeDoor()
{
    if (m_openDoorTaskHandle != nullptr)
    {
        vTaskDelete(m_openDoorTaskHandle);
        m_openDoorTaskHandle = nullptr;
    }
    m_relayModule->setPower(false);
    if (m_reportAttributesCallback)
    {
        m_reportAttributesCallback(m_reportAttributesCallbackParameter);
    }
}
