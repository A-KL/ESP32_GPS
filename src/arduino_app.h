#pragma once

#ifdef ARDUINO

#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
// #include <xpt2046.h>

#include <colors.h>
#include <geometry.h>
#include <gps.h>
#include <files.h>
#include <maps.h>

#include "../lib/conf.h"

// XPT2046 touch = XPT2046(SPI, TOUCHSCREEN_CS_PIN, TOUCHSCREEN_IRQ_PIN);

void sleepInit(){
    // digitalWrite( GPS_CE, HIGH); // GPS low power mode disabled
    gpio_wakeup_enable( (gpio_num_t )TFT_OFF_BUTTON, GPIO_INTR_LOW_LEVEL);
    esp_sleep_enable_gpio_wakeup();
}

void sleepLoop()
{
    if (digitalRead(TFT_OFF_BUTTON) == LOW) {
        digitalWrite(TFT_BLK_PIN, LOW);
        // digitalWrite( GPS_CE, LOW); // GPS low power mode. TODO: this way needs cold restart => don't work for tracking
        // setCpuFrequencyMhz(40); // TODO: check 20,10
        log_d("Enter TFT_OFF_BUTTON");
        esp_sleep_wakeup_cause_t wakeup_reason;
        delay(400); // button debounce
        do {
            // esp_sleep_enable_timer_wakeup( 10 * 1000000);
            log_d("esp_light_sleep_start");
            esp_light_sleep_start();
            wakeup_reason = esp_sleep_get_wakeup_cause();
            // if( wakeup_reason == ESP_SLEEP_WAKEUP_TIMER){
            //     gpsGetPosition(coord);
            //     // TODO
            // }
        } while( wakeup_reason == ESP_SLEEP_WAKEUP_TIMER);
        log_d("Wakeup");
        digitalWrite( TFT_BLK_PIN, HIGH);
        delay(400); // button debounce
    }
}

void stop()
{
    while(true);
}

#endif