# µPoff - External deep sleep circuit
*µPoff* is a circuit placed high-side between your power source and your microcontroller. It is designed to cut power to the µC until a external sensor activates it. Additionally a onboard RTC can wake the µC in programmed intervals.

It is mainly aimed at battery powered sensor applications like door sensors, which also should report their battery level and presence regularly.

## Pinout
@TODO

## Connecting your µC

Remove the positive connection between your power source and µC. Connect the battery to **VCC** and your µC to **OUT**. Additionally connect **GND** to your existing ground. The external sensor should pull **SW** low to activate the circuit. Lastly run a wire between one of your GPIO-Pins and **ON**. If you want to use the included RTC you'll also need to connect the I²C-Lines **SDA** and **SCL**. On AVR and STM32 based microcontrollers you'll have to use the dedicated I²C pins, on ESP8266, ESP32 you can use any free GPIO.

## Software control

Usually power only stays on as long as **SW** is low, however most tasks take longer to complete. To keep power on it is crucial to drive **ON** high as soon as your µC is started. When finished you can turn off power by driving **ON** LOW.

### Dependencies

 * Wire.h (usually included)
 * RTCLib (https://github.com/NeiroNx/RTCLib - do not use the Adafruit Fork!)

### @TODO

## Power
While sleeping current draw for *µPoff* is ~10µA. Output should be fine for up to 1A.

## Common problems

### Boot-time vs. signal time
The external sensor has to keep **SW** low until the µC is running and driving **ON**. Some µCs take several hundreds of milliseconds to boot. If your sensors pulse is too short you might be able to stretch it using some R+C magic.

 
