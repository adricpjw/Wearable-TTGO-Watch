#include "RTC_Display.h"
extern TTGOClass *ttgo;

bool rtcIrq = false;

RTC_DISPLAY::RTC_DISPLAY()
{
    rtc = ttgo->rtc;
    tft = ttgo->tft;
}

void RTC_DISPLAY::init(){

        //init and get the time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        tft->println("Failed to obtain time, Restart in 3 seconds");
        Serial.println("Failed to obtain time, Restart in 3 seconds");
        delay(3000);
        esp_restart();
        while (1);
    }
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    tft->println("Time synchronization succeeded");
    // Sync local time to external RTC
    rtc->syncToRtc();
    tft->setFreeFont(&FreeMonoOblique9pt7b);
}


void RTC_DISPLAY::run(){
    tft->drawString(rtc->formatDateTime(), 15, 80, 7);
    tft->drawString(rtc->formatDateTime(PCF_TIMEFORMAT_MM_DD_YYYY), 60, 160);
    delay(1000);
}