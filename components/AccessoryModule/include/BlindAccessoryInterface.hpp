#pragma once

#include "BaseAccessoryInterface.hpp"

/**
 * @brief Interface for blind accessory functionalities.
 */
class BlindAccessoryInterface : public BaseAccessoryInterface
{
public:
    /**
     * @brief Destructor for BlindAccessoryInterface.
     */
    ~BlindAccessoryInterface() override = default;

    /**
     * @brief Moves the blind to the specified position.
     *
     * @param newPosition The desired position to move the blind to.
     */
    virtual void moveBlindTo(uint8_t newPosition) = 0;

    /**
     * @brief Gets the current position of the blind.
     *
     * @return The current position of the blind.
     */
    virtual uint8_t getCurrentPosition() = 0;

    /**
     * @brief Gets the target position of the blind.
     *
     * @return The target position of the blind.
     */
    virtual uint8_t getTargetPosition() = 0;

    /**
     * @brief Sets the callback function for reporting to the application.
     *
     * @param callback The callback function.
     * @param callbackParam Optional parameter for the callback function.
     */
    virtual void setReportCallback(ReportCallback callback, CallbackParam * callbackParam = nullptr) override = 0;

    /**
     * @brief Identifies the blind accessory.
     */
    virtual void identify() override = 0;

    /**
     * @brief Sets the default position of the blind.
     *
     * @param defaultPosition The default position of the blind.
     */
    virtual void setDefaultPosition(uint8_t defaultPosition) = 0;
};
