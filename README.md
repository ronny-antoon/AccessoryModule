# AccessoryModule

## Overview

The `AccessoryModule` provides a set of classes and interfaces for managing various accessories, such as door locks, blinds, fans, and buttons, in an embedded system. It includes interfaces for defining accessory behaviors and concrete implementations for specific accessories. The module utilizes FreeRTOS for task management and includes logging for traceability.

## Components

### Interfaces

- **BaseAccessoryInterface**: Base interface for all accessories.
- **BlindAccessoryInterface**: Interface for blind accessory functionalities.
- **DoorLockAccessoryInterface**: Interface for door lock accessory functionalities.
- **FanAccessoryInterface**: Interface for fan accessory functionalities.
- **StatelessButtonAccessoryInterface**: Interface for stateless button accessory functionalities.

### Concrete Implementations

- **BlindAccessory**: Implementation of the blind accessory.
- **DoorLockAccessory**: Implementation of the door lock accessory.
- **FanAccessory**: Implementation of the fan accessory.
- **StatelessButtonAccessory**: Implementation of the stateless button accessory.

## Usage

### Initialization

To use an accessory, create an instance of its implementation class, passing the required module interfaces (e.g., `RelayModuleInterface`, `ButtonModuleInterface`) to its constructor.

```cpp
#include "BlindAccessory.hpp"
#include "DoorLockAccessory.hpp"
#include "FanAccessory.hpp"
#include "StatelessButtonAccessory.hpp"
#include <ButtonModuleInterface.hpp>
#include <RelayModuleInterface.hpp>

// Example initialization
RelayModuleInterface* relayModule = new RelayModule();
ButtonModuleInterface* buttonModule = new ButtonModule();

BlindAccessory* blindAccessory = new BlindAccessory(relayModule, relayModule, buttonModule, buttonModule);
DoorLockAccessory* doorLockAccessory = new DoorLockAccessory(relayModule, buttonModule);
FanAccessory* fanAccessory = new FanAccessory(relayModule, buttonModule);
StatelessButtonAccessory* buttonAccessory = new StatelessButtonAccessory(buttonModule);
```

### Setting Callbacks

Each accessory implementation provides a method to set a callback function for reporting state changes or other events.

```cpp
// Example callback function
void reportCallback(void* parameter) {
    // Handle the callback
}

blindAccessory->setReportCallback(reportCallback, nullptr);
doorLockAccessory->setReportCallback(reportCallback, nullptr);
fanAccessory->setReportCallback(reportCallback, nullptr);
buttonAccessory->setReportCallback(reportCallback, nullptr);
```

### Controlling Accessories

Control the accessories using the methods provided by each implementation class.

BlindAccessory
```cpp
blindAccessory->moveBlindTo(50); // Move the blind to 50% position
uint8_t currentPosition = blindAccessory->getCurrentPosition();
uint8_t targetPosition = blindAccessory->getTargetPosition();
```

DoorLockAccessory
```cpp
doorLockAccessory->setState(DoorLockAccessory::DoorLockState::LOCKED); // Lock the door
DoorLockAccessory::DoorLockState currentState = doorLockAccessory->getState();
```

FanAccessory
```cpp
fanAccessory->setPower(true); // Turn on the fan
bool isFanOn = fanAccessory->getPower();
```

StatelessButtonAccessory
```cpp
StatelessButtonAccessory::PressType lastPressType = buttonAccessory->getLastPressType();
```

### Identification

Each accessory can be identified using the identify method, which may trigger an LED blink sequence or similar action.
```cpp
blindAccessory->identify();
doorLockAccessory->identify();
fanAccessory->identify();
buttonAccessory->identify();
```

### Logging

The module utilizes ESP-IDF logging for traceability. Ensure that logging is configured in your project to capture these logs.

### License

This project is licensed under the MIT License - see the LICENSE file for details.