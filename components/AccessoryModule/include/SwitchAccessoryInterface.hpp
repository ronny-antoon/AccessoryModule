#pragma once

#include "BaseAccessoryInterface.hpp"

/**
 * @brief Interface for switch accessory functionalities.
 */
class SwitchAccessoryInterface : public BaseAccessoryInterface
{
public:
    /**
     * @brief Destructor for SwitchAccessoryInterface.
     */
    ~SwitchAccessoryInterface() override = default;

    /**
     * @brief Sets the power state of the switch accessory.
     *
     * @param power The desired power state.
     */
    virtual void setPower(bool power) = 0;

    /**
     * @brief Gets the power state of the switch accessory.
     *
     * @return The current power state.
     */
    virtual bool getPower() = 0;

    /**
     * @brief Sets the callback function for reporting to the application.
     *
     * @param callback The callback function.
     * @param callbackParam Optional parameter for the callback function.
     */
    virtual void setReportCallback(ReportCallback callback, CallbackParam * callbackParam = nullptr) override = 0;

    /**
     * @brief Identifies the switch accessory.
     */
    virtual void identify() override = 0;
};
