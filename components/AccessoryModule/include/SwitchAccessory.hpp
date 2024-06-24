#pragma once

#include "SwitchAccessoryInterface.hpp"
#include <ButtonModuleInterface.hpp>
#include <RelayModuleInterface.hpp>

/**
 * @brief Class representing a switch accessory.
 */
class SwitchAccessory : public SwitchAccessoryInterface
{
public:
    /**
     * @brief Constructor for SwitchAccessory.
     *
     * @param relayModule Pointer to the relay module interface.
     * @param buttonModule Pointer to the button module interface.
     */
    SwitchAccessory(RelayModuleInterface * relayModule, ButtonModuleInterface * buttonModule);

    /**
     * @brief Destructor for SwitchAccessory.
     */
    ~SwitchAccessory();

    /**
     * @brief Sets the power state of the switch accessory.
     *
     * @param power The desired power state.
     */
    void setPower(bool power) override;

    /**
     * @brief Gets the power state of the switch accessory.
     *
     * @return The current power state.
     */
    bool getPower() override;

    /**
     * @brief Sets the callback function for reporting to the application.
     *
     * @param callback The callback function.
     * @param callbackParam Optional parameter for the callback function.
     */
    void setReportCallback(ReportCallback callback, CallbackParam * callbackParam = nullptr) override;

    /**
     * @brief Identifies the switch accessory.
     */
    void identify() override;

private:
    /**
     * @brief Function called when the button is pressed.
     *
     * @param self Pointer to the instance of the class.
     */
    static void buttonCallback(void * self);

    RelayModuleInterface * m_relayModule;   ///< Pointer to the relay module interface.
    ButtonModuleInterface * m_buttonModule; ///< Pointer to the button module interface.

    ReportCallback m_reportCallback;       ///< Callback function for reporting.
    CallbackParam * m_reportCallbackParam; ///< Parameter for the callback function.
};
