#pragma once

#include "FanAccessoryInterface.hpp"
#include <ButtonModuleInterface.hpp>
#include <RelayModuleInterface.hpp>
#include <esp_log.h>

/**
 * @brief Class representing a fan accessory.
 */
class FanAccessory : public FanAccessoryInterface
{
public:
    /**
     * @brief Constructor for FanAccessory.
     *
     * @param relayModule Pointer to the relay module interface.
     * @param buttonModule Pointer to the button module interface.
     */
    FanAccessory(RelayModuleInterface * relayModule, ButtonModuleInterface * buttonModule);

    /**
     * @brief Destructor for FanAccessory.
     */
    ~FanAccessory();

    /**
     * @brief Sets the power state of the fan accessory.
     *
     * @param power The desired power state.
     */
    void setPower(bool power) override;

    /**
     * @brief Gets the power state of the fan accessory.
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
     * @brief Identifies the fan accessory.
     */
    void identify() override;

private:
    /**
     * @brief Function called when the button is pressed.
     *
     * @param instance Pointer to the instance of the class.
     */
    static void buttonCallback(void * instance);

    RelayModuleInterface * m_relayModule;   ///< Pointer to the relay module interface.
    ButtonModuleInterface * m_buttonModule; ///< Pointer to the button module interface.

    ReportCallback m_reportCallback;       ///< Callback function for reporting.
    CallbackParam * m_reportCallbackParam; ///< Parameter for the callback function.

    TaskHandle_t m_identifyHandler; ///< Task handler for the identify function.

    // Delete the copy constructor and assignment operator
    FanAccessory(const FanAccessory &)             = delete;
    FanAccessory & operator=(const FanAccessory &) = delete;
};
