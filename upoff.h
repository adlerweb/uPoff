/**
  uPoff 

  @copyright 2020 Florian Knodt, www.adlerweb.info
*/

#include <Arduino.h>
#include "Wire.h"
#ifndef UPOFF_NORTC
    #include <Rtc_Pcf8563.h>
#endif

#if defined(ESP8266)
    #include <ESP8266WiFi.h>
#elif defined(ESP32)
    #include <WiFi.h>
 #elif defined(__AVR__)
    #include <avr/sleep.h>
#endif

#ifndef UPOFF__H
    #define UPOFF__H

    class UPOFF{
        private:
            /**
             * Pin unsed to keep power on
             */
            int _uPoffOnPin;

        public:
            #ifndef UPOFF_NORTC
                /**
                 * RTC access
                 */
                Rtc_Pcf8563 rtc;
            #endif

            /**
             * Turn power off
             */
            void off(void);

            /**
             * Keep power on
             * @param int Pin used to keep power on ("ON")
             * @return bool always false
             */
            bool on(int onPin);

            #ifndef UPOFF_NORTC
                /**
                 * Check reason for wakeup
                 * @return bool true = timed wake; false = external wake
                 */
                bool rtcCheckReason(void);

                /**
                 * Keep power on
                 * @param int Pin used to keep power on ("ON")
                 * @param bool Use I²C if true
                 * @return bool true = timed wake; false = external wake
                 */
                bool on(int onPin, bool i2c);

                #if defined(ESP8266) or defined(ESP32)
                    /**
                     * Keep power on
                     * @param int Pin used to keep power on ("ON")
                     * @param int Pin used for I²C SDA
                     * @param int Pin used for I²C SCL
                     * @return bool true = timed wake; false = external wake
                     */
                    bool on(int onPin, int sda, int scl);
                #endif

                /**
                 * Check if RTC is running without a problem
                 */
                bool isValid(bool checkVolt=true);

                /**
                 * Set RTC date and time
                 * @param year Year
                 * @param month Month
                 * @param day Day
                 * @param hour Hour
                 * @param minute Minute
                 * @param second Second
                 */
                void setTime(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

                /**
                 * Get RTC date and time
                 * @return String current time in YYYY-mm-dd HH:MM:SS
                 */
                String getTime(void);

                /**
                 * Turn power off and set timer
                 * @param unsigned long seconds to wake
                 */
                void off(unsigned long seconds);
            #endif

    };

#endif /* UPOFF__H */
