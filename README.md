# µPoff - External deep sleep circuit
*µPoff* is a circuit placed high-side between your power source and your microcontroller. It is designed to cut power to the µC until a external sensor activates it. Additionally a onboard RTC can wake the µC in programmed intervals.

It is mainly aimed at battery powered sensor applications like door sensors, which also should report their battery level and presence regularly.

## Pinout

![Pinout diagram](/doc/RTC-Pinout.png?raw=true)

## PCB
### 0402 Package 
<p float="left">
  <img src="/doc/PCB_0402_top.png" width="425" />
  <img src="/doc/PCB_0402_bottom.png" width="425" /> 
</p>

### 0603 Package 
<p float="left">
  <img src="/doc/PCB_0603_top.png" width="425" />
  <img src="/doc/PCB_0603_bottom.png" width="425" /> 
</p>

## Connecting your µC

Remove the positive connection between your power source and µC. Connect the battery to **VCC** and your µC to **OUT**. Additionally connect **GND** to your existing ground. The external sensor should pull **^SW** low to activate the circuit. Lastly run a wire between one of your GPIO-Pins and **ON**. If you want to use the included RTC you'll also need to connect the I²C-Lines **SDA** and **SCL**. On AVR and STM32 based microcontrollers you'll have to use the predefined I²C pins, on ESP8266, ESP32 you can use any free GPIO.

## Software control

Usually power only stays on as long as **^ON** is low, however most tasks take longer to complete. To keep power on it is crucial to drive **ON** high as soon as your µC is started. When finished you can turn off power by driving **ON** LOW.

### Dependencies

 * Wire.h (usually included)
 * elpaso Rtc_Pcf8563 (https://github.com/elpaso/Rtc_Pcf8563)

### Manual wake only
```
#include "upoff.h"
UPOFF upoff;

void setup() {
  upoff.on(D1); //ON connected to D1
}

void loop() {
  //do something
  upoff.off();
}
```

### Manual and timed wake

```
#include "upoff.h"
UPOFF upoff;

bool reason;

void setup() {
  reason = upoff.on(D1, D2, D3); //D1 = ON; D2 = SDA; D3 = SCL
  //reason = upoff.on(D1, true); //Use predefined I²C-Pins

  /* Set clock */
  while(!upoff.isValid(false)) {
    upoff.setTime(20, 02, 02, 02, 02, 02); //2020-02-02 02:02:02
    yield();
  }
}

void loop() {
  pinMode(LED_BUILTIN, OUTPUT);
  if(reason) {
    /* either first boot or caused by RTC */
    byte loops=5;
    while(loops > 0) {
      digitalWrite(LED_BUILTIN, LOW);
      delay(100);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
      loops--;
    }
  }else{
    /* manual trigger */
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
  }

  /* Sleep for 15 seconds */
  upoff.off(15);
}
```

## Power
While sleeping current draw for *µPoff* is ~10µA. Output should be fine for up to 1A.

## Common problems

### Boot-time vs. signal time
The external sensor has to keep **^ON** low until the µC is running and driving **ON**. Some µCs take several hundreds of milliseconds to boot. If your sensors pulse is too short you might be able to stretch it using some R+C magic.

 
