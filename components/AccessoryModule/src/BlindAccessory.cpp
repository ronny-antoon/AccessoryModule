#include "BlindAccessory.hpp"
#include "esp_log.h"

static const char * TAG = "BlindAccessory";

BlindAccessory::BlindAccessory(RelayModuleInterface * motorUp, RelayModuleInterface * motorDown, ButtonModuleInterface * buttonUp,
                               ButtonModuleInterface * buttonDown, uint8_t timeToOpen, uint8_t timeToClose) :
    m_motorUp(motorUp), m_motorDown(motorDown), m_buttonUp(buttonUp), m_buttonDown(buttonDown), m_timeToOpen(timeToOpen),
    m_timeToClose(timeToClose), m_blindPosition(0), m_targetPosition(0), m_moveBlindTaskHandle(nullptr),
    m_reportAttributesCallback(nullptr), m_reportAttributesCallbackParameter(nullptr)
{
    ESP_LOGI(
        TAG,
        "Creating BlindAccessory with motorUp: %p, motorDown: %p, buttonUp: %p, buttonDown: %p, timeToOpen: %d, timeToClose: %d",
        motorUp, motorDown, buttonUp, buttonDown, timeToOpen, timeToClose);

    m_buttonUp->onSinglePress(buttonUpFunction, this);
    m_buttonDown->onSinglePress(buttonDownFunction, this);
}

BlindAccessory::~BlindAccessory()
{
    ESP_LOGI(TAG, "Destroying BlindAccessory");
}

void BlindAccessory::moveBlindTo(uint8_t newPosition)
{
    ESP_LOGI(TAG, "moveBlindTo called with newPosition: %d", newPosition);

    // Ensure position is within valid range
    if (newPosition > 100)
    {
        ESP_LOGW(TAG, "New position %d is out of range, setting to 100", newPosition);
        newPosition = 100;
    }
    else if (newPosition < 0)
    {
        ESP_LOGW(TAG, "New position %d is out of range, setting to 0", newPosition);
        newPosition = 0;
    }

    // Delete existing task handle if it exists
    if (m_moveBlindTaskHandle)
    {
        ESP_LOGI(TAG, "Deleting existing moveBlindTask");
        vTaskDelete(m_moveBlindTaskHandle);
        m_moveBlindTaskHandle = nullptr;
    }

    // Set the target position and create a task to move the blind to the target position
    m_targetPosition = newPosition;
    xTaskCreate(moveBlindToTargetTask, "moveBlindToTask", 2048, this, 1, &m_moveBlindTaskHandle);
}

uint8_t BlindAccessory::getCurrentPosition()
{
    ESP_LOGI(TAG, "getCurrentPosition called, returning: %d", m_blindPosition);
    return m_blindPosition;
}

uint8_t BlindAccessory::getTargetPosition()
{
    ESP_LOGI(TAG, "getTargetPosition called, returning: %d", m_targetPosition);
    return m_targetPosition;
}

void BlindAccessory::setReportCallback(ReportCallback callback, CallbackParam * callbackParam)
{
    ESP_LOGI(TAG, "setReportCallback called with callback: %p, parameter: %p", callback, callbackParam);
    m_reportAttributesCallback          = callback;
    m_reportAttributesCallbackParameter = callbackParam;
}

void BlindAccessory::identify()
{
    ESP_LOGI(TAG, "identify called");
    // Run task for 3 seconds to blink the LED
    xTaskCreate(
        [](void * self) {
            ESP_LOGD(TAG, "Starting identification sequence");
            BlindAccessory * blindAccessory = static_cast<BlindAccessory *>(self);
            blindAccessory->m_motorUp->setPower(false);
            blindAccessory->m_motorDown->setPower(false);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            blindAccessory->m_motorUp->setPower(true);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            blindAccessory->m_motorUp->setPower(false);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            blindAccessory->m_motorUp->setPower(true);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            blindAccessory->m_motorUp->setPower(false);

            ESP_LOGD(TAG, "Identification sequence complete");
            // Delete the task
            vTaskDelete(nullptr);
        },
        "identify", 2048, this, 5, nullptr);
}

void BlindAccessory::buttonDownFunction(void * self)
{
    BlindAccessory * blindAccessory = static_cast<BlindAccessory *>(self);
    ESP_LOGI(TAG, "buttonDownFunction called");

    if (blindAccessory->m_blindPosition != blindAccessory->m_targetPosition)
    {
        blindAccessory->moveBlindTo(blindAccessory->m_blindPosition);
    }
    else
    {
        blindAccessory->moveBlindTo(0);
    }
}

void BlindAccessory::buttonUpFunction(void * self)
{
    BlindAccessory * blindAccessory = static_cast<BlindAccessory *>(self);
    ESP_LOGI(TAG, "buttonUpFunction called");

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
    m_motorDown->setPower(false);
    m_motorUp->setPower(true);
}

void BlindAccessory::startMoveDown()
{
    ESP_LOGI(TAG, "startMoveDown called");
    m_motorUp->setPower(false);
    m_motorDown->setPower(true);
}

void BlindAccessory::stopMove()
{
    ESP_LOGI(TAG, "stopMove called");
    m_motorUp->setPower(false);
    m_motorDown->setPower(false);
}

void BlindAccessory::moveBlindToTargetTask(void * self)
{
    BlindAccessory * blindAccessory = static_cast<BlindAccessory *>(self);
    ESP_LOGI(TAG, "moveBlindToTargetTask called");

    // Return if the blind is already at the target position
    if (blindAccessory->m_blindPosition == blindAccessory->m_targetPosition)
    {
        ESP_LOGI(TAG, "Blind is already at the target position: %d", blindAccessory->m_targetPosition);
        blindAccessory->stopMove();
        if (blindAccessory->m_reportAttributesCallback)
        {
            blindAccessory->m_reportAttributesCallback(blindAccessory->m_reportAttributesCallbackParameter);
        }
        blindAccessory->m_moveBlindTaskHandle = nullptr;
        vTaskDelete(nullptr);
        return;
    }

    // Start moving the blind in the determined direction
    bool isMovingUp = blindAccessory->m_targetPosition > blindAccessory->m_blindPosition;

    uint32_t checkInterval; // Time in ms to wait before checking the position again (1% of open/close time)
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
    // Continue moving the blind until the target position is reached
    while (!blindAccessory->targetPositionReached(isMovingUp))
    {
        xTaskDelayUntil(&xLastWakeTime, xFrequency);
        blindAccessory->m_blindPosition += isMovingUp ? 1 : -1;
        if (blindAccessory->m_reportAttributesCallback)
        {
            blindAccessory->m_reportAttributesCallback(blindAccessory->m_reportAttributesCallbackParameter);
        }
    }

    // Stop the motor and report the final position
    blindAccessory->stopMove();
    ESP_LOGI(TAG, "Blind reached the target position: %d", blindAccessory->m_targetPosition);
    if (blindAccessory->m_reportAttributesCallback)
    {
        blindAccessory->m_reportAttributesCallback(blindAccessory->m_reportAttributesCallbackParameter);
    }
    blindAccessory->m_moveBlindTaskHandle = nullptr;
    vTaskDelete(nullptr);
}

bool BlindAccessory::targetPositionReached(bool movingUp)
{
    ESP_LOGI(TAG, "targetPositionReached called, movingUp: %d, current: %d, target: %d", movingUp, m_blindPosition,
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
