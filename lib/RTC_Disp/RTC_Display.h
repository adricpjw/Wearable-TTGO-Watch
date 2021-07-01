#include "config.h"

const char *ntpServer       = "pool.ntp.org";
const long  gmtOffset_sec   = 3600;
const int   daylightOffset_sec = 3600;

class RTC_DISPLAY{
    public:
        RTC_DISPLAY();
        void init();
        void run();
    private:
        char buf[128];
        PCF8563_Class *rtc;
        TFT_eSPI *tft;


};