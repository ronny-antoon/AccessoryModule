#pragma once

#include "BaseAccessoryInterface.hpp"

/**
 * @brief Interface for light accessory functionalities.
 */
class LightAccessoryInterface : public BaseAccessoryInterface
{
public:
    ~LightAccessoryInterface() = default;

    /**
     * @brief Sets the power state of the light accessory.
     *
     * @param powerState The desired power state (true for on, false for off).
     */
    virtual void setPowerState(bool powerState) = 0;

    /**
     * @brief Gets the current power state of the light accessory.
     *
     * @return true if the light is on, false if it is off.
     */
    virtual bool isPowerOn() = 0;

    /**
     * @brief Sets the callback function for reporting to the application.
     *
     * @param callback The callback function.
     * @param callbackParam Optional parameter for the callback function.
     */
    virtual void setReportCallback(ReportCallback callback, CallbackParam * callbackParam = nullptr) override = 0;

    /**
     * @brief Identifies the light accessory.
     */
    virtual void identify() override = 0;
};
