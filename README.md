# Embedded Pump Controller
Arduino project for controlling the pump of a hot tub. The application features automatic programs for all seasons. Also, you're able to switch the pump on/off manually.

The setup consists of an Arduino Nano (ATmega328P) controlling a relay. The pump is connected through an extension cord to the relay. The whole controlling unit and the pump is powered by mains power.

## Setup
1. Plug the unit to mains power. The lights will blink during boot up
2. Select the automatic mode: The corresponding automatic modes are indicated with the blinking light. Press the button to change between the different automatic modes.
  - RED: Summer mode (3h on, every 12h)
  - YELLOW: Spring/Autumn mode (3h on, every 6h)
  - Green: Winter mode (2h on, every 4h)
3. Long press button to select automatic mode

The device is now setup.

## How to use
Manually switching pump on and off by pressing the button. The light indicates the pump state.

Turn on automatic mode by long pressing the button. The light indicates the pump state.

The controll unit features a safe mode. If the relay has been changed too rapidly it will enter a safe mode and turn off the pump. All leds will blink during safe mode. Reboot device to continue.

## Pump state
-	RED: pump is off
-	GREEN: pump is on
-	YELLOW with blinking RED: automatic mode is on and pump is off
-	YELLOW with blinking GREEN: automatic mode is on and pump is on




# Wire diagram
The wire diagram shows how the Arduino is setup for the project. The light bulb in the picture is the realy switch.

![arduino wire diagram](https://user-images.githubusercontent.com/5174743/60017200-ddabed80-9690-11e9-8005-9418ab7f838c.JPG)
