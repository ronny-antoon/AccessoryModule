#pragma once

#include <ButtonModuleInterface.hpp>
#include <RelayModuleInterface.hpp>

#include "PluginAccessoryInterface.hpp"

/**
 * @brief Class representing a plugin accessory.
 */
class PluginAccessory : public PluginAccessoryInterface
{
public:
    /**
     * @brief Constructor for PluginAccessory.
     *
     * @param relayModuleInterface Pointer to the relay module interface.
     * @param buttonModuleInterface Pointer to the button module interface.
     */
    PluginAccessory(RelayModuleInterface * relayModuleInterface, ButtonModuleInterface * buttonModuleInterface);

    /**
     * @brief Destructor for PluginAccessory.
     */
    ~PluginAccessory();

    /**
     * @brief Sets the power state of the accessory.
     *
     * @param power The desired power state.
     */
    void setPower(bool power) override;

    /**
     * @brief Gets the power state of the accessory.
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
     * @brief Identifies the accessory.
     */
    void identify() override;

private:
    /**
     * @brief Function called when the button is pressed.
     *
     * @param instance Pointer to the instance of the class.
     */
    static void buttonCallback(void * instance);

    RelayModuleInterface * m_relayModuleInterface;   ///< Pointer to the relay module interface.
    ButtonModuleInterface * m_buttonModuleInterface; ///< Pointer to the button module interface.

    ReportCallback m_reportCallback;       ///< Callback function for reporting.
    CallbackParam * m_reportCallbackParam; ///< Parameter for the callback function.

    TaskHandle_t m_identifyHandler; ///< Task handler for the identify function.

    // Delete the copy constructor and assignment operator.
    PluginAccessory(const PluginAccessory &)             = delete;
    PluginAccessory & operator=(const PluginAccessory &) = delete;
};
