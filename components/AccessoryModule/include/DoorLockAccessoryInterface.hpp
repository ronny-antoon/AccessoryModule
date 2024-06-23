#pragma once

#include "BaseAccessoryInterface.hpp"

/**
 * @brief Interface for door lock accessory functionalities.
 */
class DoorLockAccessoryInterface : public BaseAccessoryInterface
{
public:
    /**
     * @brief Enum representing the state of the door lock.
     */
    enum class DoorLockState
    {
        LOCKED,  ///< The door is locked.
        UNLOCKED ///< The door is unlocked.
    };

    /**
     * @brief Destructor for DoorLockAccessoryInterface.
     */
    ~DoorLockAccessoryInterface() override = default;

    /**
     * @brief Sets the state of the door lock.
     *
     * @param lock The desired state of the door lock.
     */
    virtual void setState(DoorLockState lock) = 0;

    /**
     * @brief Gets the state of the door lock.
     *
     * @return The current state of the door lock.
     */
    virtual DoorLockState getState() = 0;

    /**
     * @brief Sets the callback function for reporting to the application.
     *
     * @param callback The callback function.
     * @param callbackParam Optional parameter for the callback function.
     */
    virtual void setReportCallback(ReportCallback callback, CallbackParam * callbackParam = nullptr) override = 0;

    /**
     * @brief Identifies the door lock accessory.
     */
    virtual void identify() override = 0;
};
