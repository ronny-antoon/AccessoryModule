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

    ReportCallback m_reportAttributesCallback;           ///< Callback function for reporting.
    CallbackParam * m_reportAttributesCallbackParameter; ///< Parameter for the callback function.
};
