#pragma once

#include <ButtonModuleInterface.hpp>
#include <RelayModuleInterface.hpp>

#include "LightAccessoryInterface.hpp"

/**
 * @brief Concrete implementation of the LightAccessoryInterface.
 */
class LightAccessory : public LightAccessoryInterface
{
public:
    /**
     * @brief Constructs a LightAccessory object.
     *
     * @param relayModuleInterface Pointer to the relay module.
     * @param buttonModuleInterface Pointer to the button module.
     */
    LightAccessory(RelayModuleInterface * relayModuleInterface, ButtonModuleInterface * buttonModuleInterface);

    /**
     * @brief Destructor for LightAccessory.
     */
    ~LightAccessory();

    /**
     * @brief Sets the power state of the light accessory.
     *
     * @param powerState The desired power state (true for on, false for off).
     */
    void setPowerState(bool powerState) override;

    /**
     * @brief Gets the current power state of the light accessory.
     *
     * @return true if the light is on, false if it is off.
     */
    bool isPowerOn() override;

    /**
     * @brief Sets the callback function for reporting to the application.
     *
     * @param callback The callback function.
     * @param callbackParam Optional parameter for the callback function.
     */
    void setReportCallback(ReportCallback callback, CallbackParam * callbackParam = nullptr) override;

    /**
     * @brief Identifies the light accessory.
     */
    void identify() override;

private:
    /**
     * @brief Button callback function.
     *
     * @param instance Pointer to the LightAccessory object.
     */
    static void buttonCallback(void * instance);

    RelayModuleInterface * m_relayModule;   ///< Pointer to the relay module interface.
    ButtonModuleInterface * m_buttonModule; ///< Pointer to the button module interface.

    ReportCallback m_reportCallback;       ///< Callback function for reporting attributes.
    CallbackParam * m_reportCallbackParam; ///< Parameter to be passed to the callback function.
};
