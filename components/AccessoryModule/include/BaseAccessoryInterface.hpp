#pragma once

/**
 * @brief Interface for base accessory functionalities.
 */
class BaseAccessoryInterface
{
public:
    /**
     * @brief Type definition for callback function used for reporting.
     *
     * @param parameter Pointer to user-defined data.
     */
    using ReportCallback = void (*)(void *);

    /**
     * @brief Type definition for any callback parameter type.
     */
    using CallbackParam = void;

    virtual ~BaseAccessoryInterface() = default;

    /**
     * @brief Sets the callback function for reporting to the application.
     *
     * @param callback The callback function.
     * @param callbackParam Optional parameter for the callback function.
     */
    virtual void setReportCallback(ReportCallback callback, CallbackParam * callbackParam = nullptr) = 0;

    /**
     * @brief Identifies the accessory.
     */
    virtual void identify() = 0;
};
