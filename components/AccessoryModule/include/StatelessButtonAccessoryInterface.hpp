#pragma once

#include "BaseAccessoryInterface.hpp"

/**
 * @brief Interface for stateless button accessory functionalities.
 */
class StatelessButtonAccessoryInterface : public BaseAccessoryInterface
{
public:
    /**
     * @brief Enum representing the type of button press.
     */
    enum PressType
    {
        SinglePress = 1, ///< Represents a single press event.
        DoublePress = 2, ///< Represents a double press event.
        LongPress   = 3  ///< Represents a long press event.
    };

    /**
     * @brief Destructor for StatelessButtonAccessoryInterface.
     */
    ~StatelessButtonAccessoryInterface() override = default;

    /**
     * @brief Sets the callback function for reporting to the application.
     *
     * @param callback The callback function.
     * @param callbackParam Optional parameter for the callback function.
     */
    virtual void setReportCallback(ReportCallback callback, CallbackParam * callbackParam = nullptr) override = 0;

    /**
     * @brief Identifies the stateless button accessory.
     */
    virtual void identify() override = 0;

    /**
     * @brief Gets the last press type.
     *
     * @return The type of the last press.
     */
    virtual PressType getLastPressType() = 0;
};
