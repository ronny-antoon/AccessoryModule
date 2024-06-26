#include "DoorLockAccessory.hpp"

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char * TAG = "DoorLockAccessory";

DoorLockAccessory::DoorLockAccessory(RelayModuleInterface * relayModule, ButtonModuleInterface * buttonModule,
                                     uint8_t openDuration) :
    m_relayModule(relayModule), m_buttonModule(buttonModule), m_openDuration(openDuration), m_openDoorTaskHandle(nullptr)
{
    ESP_LOGI(TAG, "DoorLockAccessory created");
    m_buttonModule->setSinglePressCallback(buttonCallback, this);
}

DoorLockAccessory::~DoorLockAccessory()
{
    ESP_LOGI(TAG, "DoorLockAccessory destroyed");
}

void DoorLockAccessory::setState(DoorLockState state)
{
    ESP_LOGI(TAG, "Setting state to %s", state == DoorLockState::LOCKED ? "LOCKED" : "UNLOCKED");
    if (state == DoorLockState::LOCKED)
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
    DoorLockState state = m_relayModule->isOn() ? DoorLockState::UNLOCKED : DoorLockState::LOCKED;
    ESP_LOGI(TAG, "Current state is %s", state == DoorLockState::LOCKED ? "LOCKED" : "UNLOCKED");
    return state;
}

void DoorLockAccessory::setReportCallback(ReportCallback callback, CallbackParam * callbackParam)
{
    ESP_LOGI(TAG, "Setting report callback");
    m_reportCallback      = callback;
    m_reportCallbackParam = callbackParam;
}

void DoorLockAccessory::identify()
{
    ESP_LOGI(TAG, "Identifying DoorLockAccessory");
    xTaskCreate(
        [](void * instance) {
            ESP_LOGD(TAG, "Starting identification sequence");
            DoorLockAccessory * doorLockAccessory = static_cast<DoorLockAccessory *>(instance);
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
            vTaskDelete(nullptr);
        },
        "identify", 2048, this, 5, nullptr);
}

void DoorLockAccessory::buttonCallback(void * instance)
{
    DoorLockAccessory * doorLockAccessory = static_cast<DoorLockAccessory *>(instance);
    doorLockAccessory->setState(doorLockAccessory->getState() == DoorLockState::LOCKED ? DoorLockState::UNLOCKED
                                                                                       : DoorLockState::LOCKED);
}

void DoorLockAccessory::openDoor()
{
    if (getState() == DoorLockState::LOCKED)
    {
        ESP_LOGI(TAG, "Opening door");
        m_relayModule->setPower(true);
        if (m_reportCallback)
        {
            m_reportCallback(m_reportCallbackParam);
        }
        xTaskCreate(openDoorTask, "openDoor", 2048, this, 5, &m_openDoorTaskHandle);
    }
    else if (getState() == DoorLockState::UNLOCKED && m_openDoorTaskHandle != nullptr)
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
    vTaskDelay(doorLockAccessory->m_openDuration * 1000 / portTICK_PERIOD_MS);
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
    ESP_LOGI(TAG, "Closing door");
    m_relayModule->setPower(false);
    if (m_reportCallback)
    {
        m_reportCallback(m_reportCallbackParam);
    }
}
