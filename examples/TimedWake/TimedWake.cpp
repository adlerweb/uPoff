#include <Arduino.h>
#include "upoff.h"

UPOFF upoff;

bool reason;

void setup() {
  reason = upoff.on(D1, D3, D2);

  /* Anti BS */
  while(!upoff.isValid(false)) {
    upoff.setTime(20, 02, 02, 02, 02, 02);
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
