#pragma once

#include "StatelessButtonAccessoryInterface.hpp"
#include <ButtonModuleInterface.hpp>
#include <esp_log.h>

/**
 * @brief Class representing a stateless button accessory.
 */
class StatelessButtonAccessory : public StatelessButtonAccessoryInterface
{
public:
    /**
     * @brief Constructor for StatelessButtonAccessory.
     *
     * @param buttonModule Pointer to the button module interface.
     */
    StatelessButtonAccessory(ButtonModuleInterface * buttonModule);

    /**
     * @brief Destructor for StatelessButtonAccessory.
     */
    ~StatelessButtonAccessory();

    /**
     * @brief Sets the callback function for reporting to the application.
     *
     * @param callback The callback function.
     * @param callbackParam Optional parameter for the callback function.
     */
    void setReportCallback(ReportCallback callback, CallbackParam * callbackParam = nullptr) override;

    /**
     * @brief Identifies the stateless button accessory.
     */
    void identify() override;

    /**
     * @brief Gets the last press type.
     *
     * @return The type of the last press.
     */
    PressType getLastPressType() override;

private:
    ButtonModuleInterface * m_buttonModule; ///< Pointer to the button module interface.
    PressType m_lastPressType;              ///< Stores the type of the last press.

    ReportCallback m_reportCallback;       ///< Callback function for reporting.
    CallbackParam * m_reportCallbackParam; ///< Parameter for the callback function.

    /**
     * @brief Handles the button press.
     *
     * @param self Pointer to the StatelessButtonAccessory object.
     * @param pressType The type of the press.
     * @param logMessage The message to log.
     */
    static void handlePress(void * self, PressType pressType, const char * logMessage);
};
