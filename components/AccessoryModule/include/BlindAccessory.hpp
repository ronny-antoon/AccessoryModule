#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <ButtonModuleInterface.hpp>
#include <RelayModuleInterface.hpp>

#include "BlindAccessoryInterface.hpp"

/**
 * @brief Class representing a blind accessory.
 */
class BlindAccessory : public BlindAccessoryInterface
{
public:
    /**
     * @brief Constructor for BlindAccessory.
     *
     * @param motorUp Pointer to the relay module interface for moving the blind up.
     * @param motorDown Pointer to the relay module interface for moving the blind down.
     * @param buttonUp Pointer to the button module interface for the up button.
     * @param buttonDown Pointer to the button module interface for the down button.
     * @param timeToOpen Time in seconds to fully open the blind.
     * @param timeToClose Time in seconds to fully close the blind.
     */
    BlindAccessory(RelayModuleInterface * motorUp, RelayModuleInterface * motorDown, ButtonModuleInterface * buttonUp,
                   ButtonModuleInterface * buttonDown, uint8_t timeToOpen = 30, uint8_t timeToClose = 30);

    /**
     * @brief Destructor for BlindAccessory.
     */
    ~BlindAccessory();

    /**
     * @brief Moves the blind to the specified position.
     *
     * @param newPosition The desired position to move the blind to.
     */
    void moveBlindTo(uint8_t newPosition) override;

    /**
     * @brief Gets the current position of the blind.
     *
     * @return The current position of the blind.
     */
    uint8_t getCurrentPosition() override;

    /**
     * @brief Gets the target position of the blind.
     *
     * @return The target position of the blind.
     */
    uint8_t getTargetPosition() override;

    /**
     * @brief Sets the callback function for reporting to the application.
     *
     * @param callback The callback function.
     * @param callbackParam Optional parameter for the callback function.
     */
    void setReportCallback(ReportCallback callback, CallbackParam * callbackParam = nullptr) override;

    /**
     * @brief Identifies the blind accessory.
     */
    void identify() override;

    /**
     * @brief Sets the default position of the blind.
     *
     * @param defaultPosition The default position of the blind.
     */
    void setDefaultPosition(uint8_t defaultPosition) override;

private:
    /**
     * @brief Function called when the down button is pressed.
     *
     * @param instance Pointer to the instance of the class.
     */
    static void buttonDownCallback(void * instance);

    /**
     * @brief Function called when the up button is pressed.
     *
     * @param instance Pointer to the instance of the class.
     */
    static void buttonUpCallback(void * instance);

    /**
     * @brief Starts moving the blind up.
     */
    void startMoveUp();

    /**
     * @brief Starts moving the blind down.
     */
    void startMoveDown();

    /**
     * @brief Stops the blind from moving.
     */
    void stopMove();

    /**
     * @brief Task to move the blind to the target position.
     *
     * @param instance Pointer to the instance of the class.
     */
    static void moveBlindToTargetTask(void * instance);

    /**
     * @brief Checks if the target position has been reached.
     *
     * @param movingUp True if the blind is moving up, false if it is moving down.
     * @return True if the target position has been reached, false otherwise.
     */
    bool targetPositionReached(bool movingUp);

    RelayModuleInterface * m_motorUp;     ///< Pointer to the relay module for moving the blind up.
    RelayModuleInterface * m_motorDown;   ///< Pointer to the relay module for moving the blind down.
    ButtonModuleInterface * m_buttonUp;   ///< Pointer to the button module for the up button.
    ButtonModuleInterface * m_buttonDown; ///< Pointer to the button module for the down button.
    uint8_t m_timeToOpen;                 ///< Time in seconds to fully open the blind.
    uint8_t m_timeToClose;                ///< Time in seconds to fully close the blind.
    uint8_t m_blindPosition;              ///< Current position of the blind.
    uint8_t m_targetPosition;             ///< Target position of the blind.
    TaskHandle_t m_moveBlindTaskHandle;   ///< Task handle for the move blind task.

    ReportCallback m_reportCallback;       ///< Callback function for reporting attributes.
    CallbackParam * m_reportCallbackParam; ///< Parameter to be passed to the callback function.

    // Delete copy constructor and assignment operator
    BlindAccessory(const BlindAccessory &)             = delete;
    BlindAccessory & operator=(const BlindAccessory &) = delete;
};
