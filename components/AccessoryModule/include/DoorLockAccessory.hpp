#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <ButtonModuleInterface.hpp>
#include <RelayModuleInterface.hpp>

#include "DoorLockAccessoryInterface.hpp"

/**
 * @brief Implementation of the Door Lock Accessory.
 */
class DoorLockAccessory : public DoorLockAccessoryInterface
{
public:
    /**
     * @brief Constructor for DoorLockAccessory.
     *
     * @param relayModule Pointer to the relay module interface.
     * @param buttonModule Pointer to the button module interface.
     * @param openDuration Time in seconds to keep the door open.
     */
    DoorLockAccessory(RelayModuleInterface * relayModule, ButtonModuleInterface * buttonModule, uint8_t openDuration = 5);

    /**
     * @brief Destructor for DoorLockAccessory.
     */
    ~DoorLockAccessory();

    /**
     * @brief Set the lock state of the door lock.
     *
     * @param lock The state to set the door lock to.
     */
    void setState(DoorLockState lock) override;

    /**
     * @brief Get the current lock state of the door lock.
     *
     * @return The current state of the door lock.
     */
    DoorLockState getState() override;

    /**
     * @brief Set the callback function for reporting.
     *
     * @param callback Callback function pointer.
     * @param callbackParam Optional parameter to be passed to the callback function.
     */
    void setReportCallback(ReportCallback callback, CallbackParam * callbackParam = nullptr) override;

    /**
     * @brief Method to identify the door lock accessory.
     */
    void identify() override;

private:
    /**
     * @brief Static function to handle button press.
     *
     * @param instance Pointer to the DoorLockAccessory instance.
     */
    static void buttonCallback(void * instance);

    void openDoor();
    static void openDoorTask(void * pvParameters);
    void closeDoor();

    RelayModuleInterface * m_relayModule;   ///< Pointer to the relay module.
    ButtonModuleInterface * m_buttonModule; ///< Pointer to the button module.
    uint8_t m_openDuration;                 ///< Time in seconds to keep the door open.
    TaskHandle_t m_openDoorTaskHandle;      ///< Task handle for the open door task.

    ReportCallback m_reportCallback;       ///< Callback function for reporting attributes.
    CallbackParam * m_reportCallbackParam; ///< Parameter to be passed to the callback function.

    TaskHandle_t m_identifyHandler; ///< Task handler for the identify function.

    // Delete copy constructor and assignment operator
    DoorLockAccessory(const DoorLockAccessory &)             = delete;
    DoorLockAccessory & operator=(const DoorLockAccessory &) = delete;
};
