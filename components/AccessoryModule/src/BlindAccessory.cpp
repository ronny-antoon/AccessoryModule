#include "BlindAccessory.hpp"
#include "esp_log.h"

static const char * TAG = "BlindAccessory";

BlindAccessory::BlindAccessory(RelayModuleInterface * motorUp, RelayModuleInterface * motorDown, ButtonModuleInterface * buttonUp,
                               ButtonModuleInterface * buttonDown, uint8_t timeToOpen, uint8_t timeToClose) :
    m_motorUp(motorUp), m_motorDown(motorDown), m_buttonUp(buttonUp), m_buttonDown(buttonDown), m_timeToOpen(timeToOpen),
    m_timeToClose(timeToClose), m_blindPosition(0), m_targetPosition(0), m_moveBlindTaskHandle(nullptr), m_reportCallback(nullptr),
    m_reportCallbackParam(nullptr)
{
    ESP_LOGI(TAG, "Creating BlindAccessory with timeToOpen: %d, timeToClose: %d", timeToOpen, timeToClose);

    if (m_buttonUp)
    {
        m_buttonUp->setSinglePressCallback(buttonUpCallback, this);
    }
    if (m_buttonDown)
    {
        m_buttonDown->setSinglePressCallback(buttonDownCallback, this);
    }
}

BlindAccessory::~BlindAccessory()
{
    ESP_LOGI(TAG, "Destroying BlindAccessory");

    if (m_moveBlindTaskHandle)
    {
        ESP_LOGI(TAG, "Deleting existing moveBlindTask");
        vTaskDelete(m_moveBlindTaskHandle);
        m_moveBlindTaskHandle = nullptr;
    }
}

void BlindAccessory::moveBlindTo(uint8_t newPosition)
{
    ESP_LOGI(TAG, "moveBlindTo called with newPosition: %d", newPosition);

    if (newPosition > 100)
    {
        ESP_LOGW(TAG, "New position %d is out of range, setting to 100", newPosition);
        newPosition = 100;
    }

    if (m_moveBlindTaskHandle)
    {
        ESP_LOGI(TAG, "Deleting existing moveBlindTask");
        vTaskDelete(m_moveBlindTaskHandle);
        m_moveBlindTaskHandle = nullptr;
    }

    m_targetPosition = newPosition;
    xTaskCreate(moveBlindToTargetTask, "moveBlindToTask", CONFIG_A_M_BLIND_ACCESSORY_MOVING_STACK_SIZE, this,
                CONFIG_A_M_BLIND_ACCESSORY_MOVING_PRIORITY, &m_moveBlindTaskHandle);
}

uint8_t BlindAccessory::getCurrentPosition()
{
    ESP_LOGD(TAG, "getCurrentPosition called, returning: %d", m_blindPosition);
    return m_blindPosition;
}

uint8_t BlindAccessory::getTargetPosition()
{
    ESP_LOGD(TAG, "getTargetPosition called, returning: %d", m_targetPosition);
    return m_targetPosition;
}

void BlindAccessory::setReportCallback(ReportCallback callback, CallbackParam * callbackParam)
{
    ESP_LOGI(TAG, "setReportCallback called ");
    m_reportCallback      = callback;
    m_reportCallbackParam = callbackParam;
}

void BlindAccessory::identify()
{
    ESP_LOGI(TAG, "identify called");
    xTaskCreate(
        [](void * instance) {
            ESP_LOGD(TAG, "Starting identification sequence");

            BlindAccessory * blindAccessory = static_cast<BlindAccessory *>(instance);
            blindAccessory->startMoveDown();
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            blindAccessory->startMoveUp();
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            blindAccessory->startMoveDown();
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            blindAccessory->startMoveUp();
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            blindAccessory->stopMove();

            ESP_LOGD(TAG, "Identification sequence complete");
            vTaskDelete(nullptr);
        },
        "identify", CONFIG_A_M_BLIND_ACCESSORY_IDENTIFY_STACK_SIZE, this, CONFIG_A_M_BLIND_ACCESSORY_IDENTIFY_PRIORITY, nullptr);
}

void BlindAccessory::buttonDownCallback(void * instance)
{
    BlindAccessory * blindAccessory = static_cast<BlindAccessory *>(instance);
    ESP_LOGI(TAG, "buttonDownCallback called");

    if (blindAccessory->m_blindPosition != blindAccessory->m_targetPosition)
    {
        blindAccessory->moveBlindTo(blindAccessory->m_blindPosition);
    }
    else
    {
        blindAccessory->moveBlindTo(0);
    }
}

void BlindAccessory::buttonUpCallback(void * instance)
{
    BlindAccessory * blindAccessory = static_cast<BlindAccessory *>(instance);
    ESP_LOGI(TAG, "buttonUpCallback called");

    if (blindAccessory->m_blindPosition != blindAccessory->m_targetPosition)
    {
        blindAccessory->moveBlindTo(blindAccessory->m_blindPosition);
    }
    else
    {
        blindAccessory->moveBlindTo(100);
    }
}

void BlindAccessory::startMoveUp()
{
    ESP_LOGI(TAG, "startMoveUp called");
    if (m_motorDown)
    {
        m_motorDown->setPower(false);
    }
    if (m_motorUp)
    {
        m_motorUp->setPower(true);
    }
}

void BlindAccessory::startMoveDown()
{
    ESP_LOGI(TAG, "startMoveDown called");
    if (m_motorUp)
    {
        m_motorUp->setPower(false);
    }
    if (m_motorDown)
    {
        m_motorDown->setPower(true);
    }
}

void BlindAccessory::stopMove()
{
    ESP_LOGI(TAG, "stopMove called");
    if (m_motorUp)
    {
        m_motorUp->setPower(false);
    }
    if (m_motorDown)
    {
        m_motorDown->setPower(false);
    }
}

void BlindAccessory::moveBlindToTargetTask(void * instance)
{
    BlindAccessory * blindAccessory = static_cast<BlindAccessory *>(instance);
    ESP_LOGI(TAG, "moveBlindToTargetTask called");

    if (blindAccessory->m_blindPosition == blindAccessory->m_targetPosition)
    {
        ESP_LOGI(TAG, "Blind is already at the target position: %d", blindAccessory->m_targetPosition);
        blindAccessory->stopMove();
        if (blindAccessory->m_reportCallback)
        {
            blindAccessory->m_reportCallback(blindAccessory->m_reportCallbackParam);
        }
        blindAccessory->m_moveBlindTaskHandle = nullptr;
        vTaskDelete(nullptr);
        return;
    }

    bool isMovingUp = blindAccessory->m_targetPosition > blindAccessory->m_blindPosition;

    uint32_t checkInterval;
    if (isMovingUp)
    {
        checkInterval = 1000 * (blindAccessory->m_timeToOpen) / 100;
    }
    else
    {
        checkInterval = 1000 * (blindAccessory->m_timeToClose) / 100;
    }

    ESP_LOGI(TAG, "Starting to move the blind %s", isMovingUp ? "up" : "down");
    if (isMovingUp)
    {
        blindAccessory->startMoveUp();
    }
    else
    {
        blindAccessory->startMoveDown();
    }

    TickType_t xLastWakeTime    = xTaskGetTickCount();
    const TickType_t xFrequency = checkInterval / portTICK_PERIOD_MS;

    while (!blindAccessory->targetPositionReached(isMovingUp))
    {
        xTaskDelayUntil(&xLastWakeTime, xFrequency);
        blindAccessory->m_blindPosition += isMovingUp ? 1 : -1;
        if (blindAccessory->m_reportCallback)
        {
            blindAccessory->m_reportCallback(blindAccessory->m_reportCallbackParam);
        }
    }

    blindAccessory->stopMove();
    ESP_LOGI(TAG, "Blind reached the target position: %d", blindAccessory->m_targetPosition);
    if (blindAccessory->m_reportCallback)
    {
        blindAccessory->m_reportCallback(blindAccessory->m_reportCallbackParam);
    }
    blindAccessory->m_moveBlindTaskHandle = nullptr;
    vTaskDelete(nullptr);
}

bool BlindAccessory::targetPositionReached(bool movingUp)
{
    ESP_LOGD(TAG, "targetPositionReached called, movingUp: %d, current: %d, target: %d", movingUp, m_blindPosition,
             m_targetPosition);
    if (movingUp)
    {
        return m_blindPosition >= m_targetPosition;
    }
    else
    {
        return m_blindPosition <= m_targetPosition;
    }
}

void BlindAccessory::setDefaultPosition(uint8_t defaultPosition)
{
    ESP_LOGI(TAG, "setDefaultPosition called with defaultPosition: %d", defaultPosition);
    m_blindPosition  = defaultPosition;
    m_targetPosition = defaultPosition;
}
