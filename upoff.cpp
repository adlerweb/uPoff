/**
  uPoff 

  @copyright 2020 Florian Knodt, www.adlerweb.info
*/

#ifndef UPOFF__CPP
    #define UPOFF__CPP
    //#define UPOFF_DEBUG

    #include "upoff.h"

    bool UPOFF::on(int onPin) {
        _uPoffOnPin = onPin;
        pinMode(_uPoffOnPin, OUTPUT);
        digitalWrite(_uPoffOnPin, HIGH);
        return false; //External reason
    }

    #ifndef UPOFF_NORTC

        bool UPOFF::rtcCheckReason() {
            bool status = false;
            rtc.getDateTime(); //Required to update status registers
            if(rtc.alarmActive() && rtc.alarmEnabled()) status = true;
            //if(rtc.timerActive() && rtc.timerEnabled()) status = true;
            if(rtc.timerActive()) status = true; //timerEnabled() was always 0 for me. Bug in library/silicon?
            
            rtc.clearAlarm();
            rtc.clearTimer();

            return status;
        }

        bool UPOFF::on(int onPin, bool i2c) {
            on(onPin);
            if(i2c) {
                Wire.begin();
            }

            return rtcCheckReason();
        }

        /* ESP8266 and ESP32 allow to specify custom pins for SDA/SCL */
        #if defined(ESP8266) or defined(ESP32)
            /**
             * Keep power on
             * @param int Pin used to keep power on ("ON")
             * @param int Pin used for I²C SDA
             * @param int Pin used for I²C SCL
             * @return true if initialized
             */
            bool UPOFF::on(int onPin, int sda, int scl) {
                on(onPin);
                Wire.begin(sda, scl);
                
                return rtcCheckReason();
            }
        #endif

        bool UPOFF::isValid(bool checkVolt) {
             if(checkVolt && !rtc.getVoltLow()) {
                #ifdef UPOFF_DEBUG
                    Serial.println("E:Voltage");
                #endif
                return false;
             }
             byte temp;
             temp = rtc.getMonth();
             if(temp > 12 || temp == 0 ) {
                #ifdef UPOFF_DEBUG
                    Serial.println("E:Month");
                #endif
                return false;
             }
             temp = rtc.getDay();
             if(temp > 31 || temp == 0 ) {
                #ifdef UPOFF_DEBUG
                    Serial.println("E:Day");
                #endif
                return false;
             }
             temp = rtc.getHour();
             if(temp > 24 ) {
                #ifdef UPOFF_DEBUG
                    Serial.println("E:Hour");
                #endif
                return false;
             }
             temp = rtc.getMinute();
             if(temp > 59 ) {
                #ifdef UPOFF_DEBUG
                    Serial.println("E:Minute");
                #endif
                return false;
             }
             temp = rtc.getSecond();
             if(temp > 59 ) {
                #ifdef UPOFF_DEBUG
                    Serial.println("E:Second");
                #endif
                return false;
             }
             return true;
        }

        void UPOFF::setTime(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
            byte weekday = rtc.whatWeekday(day, month, 0, year);
            rtc.initClock();
            delay(10);
            rtc.setDateTime(day, weekday, month, 0, year, hour, minute, second);
            delay(10);
            rtc.clearVoltLow();
        }

        String UPOFF::getTime(void) {
            String out;
            out += rtc.formatDate(RTCC_DATE_ASIA); //Actually ISO 8601 international format. Use it.
            out += " ";
            out += rtc.formatTime(RTCC_TIME_HMS);
            return out;
        }

        void UPOFF::off(unsigned long seconds) {
            if(seconds < 240) {
                //When below 4 minutes we're using a second-countdown
                #ifdef UPOFF_DEBUG
                    Serial.print("Setting timer to ");
                    Serial.print(seconds);
                    Serial.println(" Seconds @ 1Hz");
                #endif
                rtc.setTimer(seconds, TMR_1Hz, false);
            }else{
                byte dev = seconds % 60;
                seconds /= 60;
                if(dev >= 30)
                    seconds++;

                #ifdef UPOFF_DEBUG
                    Serial.print("Not enough bits - calculated ");
                    Serial.print(seconds);
                    Serial.println(" Minutes");
                #endif

                if(seconds < 240) {
                    //Between 4 minutes and 4 hours we're using a minute based countdown
                    //Triggers might deviate up to 30 seconds from target

                    #ifdef UPOFF_DEBUG
                        Serial.print("Setting timer to ");
                        Serial.print(seconds);
                        Serial.println(" Minutes @ 1/60Hz");
                    #endif

                    rtc.setTimer(seconds, TMR_1MIN, false);
                }else{
                    //Above 4 hours we use the alarm functionality. With luck it works
                    //even tho the calculations are…uhm…interesting

                    #ifdef UPOFF_DEBUG
                        Serial.println("Switching to alarm mode. Current Time:");
                        Serial.println(getTime());
                    #endif

                    uint32_t target = 0;
                    target += rtc.getDay() * 24 * 60;
                    target += rtc.getHour() * 60;
                    target += rtc.getMinute();

                    #ifdef UPOFF_DEBUG
                        Serial.println("Second calculation:");
                        Serial.print(target);
                    #endif

                    target += seconds;

                    #ifdef UPOFF_DEBUG
                        Serial.print(" + ");
                        Serial.print(seconds);
                        Serial.print(" = ");
                        Serial.println(target);
                    #endif

                    byte aDay = (target / (24*60));
                    target -= (aDay * (24*60));
                    byte aHour = (target / 60);
                    target -= (aHour * 60);
                    byte aMinute = target;

                    #ifdef UPOFF_DEBUG
                        Serial.print("D:");
                        Serial.print(aDay);
                        Serial.print(" H:");
                        Serial.print(aHour);
                        Serial.print(" M:");
                        Serial.println(aMinute);
                    #endif

                    byte daysThisMonth = rtc.daysInMonth(rtc.getCentury(), rtc.getYear(), rtc.getMonth());

                    if(aDay > daysThisMonth) {
                        #ifdef UPOFF_DEBUG
                            Serial.print("Target day ");
                            Serial.print(aDay);
                            Serial.print(" is above this month limit of ");
                            Serial.print(daysThisMonth);
                            Serial.print(" -> new Day is ");
                        #endif
                        aDay -= daysThisMonth;
                        #ifdef UPOFF_DEBUG
                            Serial.println(aDay);
                        #endif
                    }
                    
                    rtc.setAlarm(aMinute, aHour, aDay, 0xFF);
                }
            }

            off();
        }
    #endif

    /**
     * Turn power off
     */
    void UPOFF::off(void) {
        digitalWrite(_uPoffOnPin, LOW);
        
        //better safe than sorry…
        #if defined(ESP8266)
            WiFi.mode( WIFI_OFF );
            WiFi.forceSleepBegin();
            ESP.deepSleep(0);
        #elif defined(ESP32)
            WiFi.mode(WIFI_OFF);
            btStop();
            esp_deep_sleep_start();
        #elif defined(__AVR__)
            set_sleep_mode(SLEEP_MODE_PWR_DOWN);
            sleep_enable();
            sleep_mode();
        #elif defined(_STM32_) and defined(HAL_PWR_MODULE_ENABLED)
            __HAL_RCC_PWR_CLK_ENABLE(); // Enable Power Control clock
            HAL_PWREx_EnableUltraLowPower(); // Ultra low power mode
            
            // Switch to STOPMode
            HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
        #endif

        while(1) {
            #if defined(ESP8266) || defined(ESP32)
                yield();
            #endif
        }
    }

#endif /* UPOFF__CPP */
