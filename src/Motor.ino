
#include "config.h"
//  git clone -b development https://github.com/tzapu/WiFiManager.git
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#define ARDUINO_OTA_UPDATE  
#ifdef ARDUINO_OTA_UPDATE
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#endif
#include <PubSubClient.h>
#include <WiFi.h>

unsigned long lastMillis = 0;

bool initial = 1;
uint8_t omm = 99;
bool otaStart = false;
uint32_t targetTime = 0;       // for next 1 second timeout
TTGOClass *ttgo;

/* ------------------ WIFI LOGIN ------------------- */
const char* ssid = "BTIA_JEJ1SGP";
const char* password = "4OoMHjCVIjnU";
// const char* ssid = "{SSID OF WIFI}";
// const char* password = "{PASSWORD}";


/* ------------------ MQTT IP ADDRESS ------------------- */
// const char* mqtt_server = "192.168.0.151";
const char* mqtt_server = "192.168.88.221";
// const char* mqtt_server = "192.168.43.182";

/* ------------------ Wifi + MQTT inits ------------------- */
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
int deg = -180;
long now = 0;
String lastMessage = " ";



/*----------- setup WiFi ------------*/

void setupWiFi()
{
    Serial.println();
    Serial.print("Connecting to");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    ttgo->tft->fillScreen(TFT_BLACK);
    ttgo->tft->setTextColor(TFT_WHITE, TFT_BLACK);
    ttgo->tft->drawCentreString("Attempting to connect to Network...",ttgo->tft->width()/2, ttgo->tft->height()/2,2);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    ttgo->tft->fillScreen(TFT_BLACK);
    ttgo->tft->setTextColor(TFT_GREEN, TFT_BLACK);
    String connection= "wifi connected to : ";
    connection += WiFi.SSID();
    ttgo->tft->drawCentreString(connection,ttgo->tft->width()/2, ttgo->tft->height()/2,2);
    Serial.print("Wifi Connected to: ");
    Serial.println(WiFi.localIP());
}


void reconnect() {
    //Loop until reconnect
    ttgo->tft->fillScreen(TFT_BLACK);
    ttgo->tft->setTextColor(TFT_WHITE, TFT_BLACK);
    while (!client.connected()) {
        ttgo->tft->drawCentreString("Attempting MQTT Connection...",ttgo->tft->width()/2, ttgo->tft->height()/2,2);
        Serial.print("Attempting MQTT connection...");
        if (client.connect("ESP8266Client")){
            Serial.println("connected");
            client.subscribe("/hello");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 3 seconds");
        }
        delay(3000);
    }
}

/* ---------- setup OTA Update if needed ---------- */
void setupOTA()
{
#ifdef ARDUINO_OTA_UPDATE
    // Port defaults to 3232
    // ArduinoOTA.setPort(3232);

    // Hostname defaults to esp3232-[MAC]
    ArduinoOTA.setHostname("T-Wristband");

    // No authentication by default
    // ArduinoOTA.setPassword("admin");

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
            type = "sketch";
        else // U_SPIFFS
            type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        Serial.println("Start updating " + type);
        otaStart = true;
        ttgo->tft->fillScreen(TFT_BLACK);
        ttgo->tft->drawString("Updating...", ttgo->tft->width() / 2 - 20, 55 );
    })
    .onEnd([]() {
        Serial.println("\nEnd");
        delay(500);
    })
    .onProgress([](unsigned int progress, unsigned int total) {
        // Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        int percentage = (progress / (total / 100));
        ttgo->tft->setTextDatum(TC_DATUM);
        ttgo->tft->setTextPadding(ttgo->tft->textWidth(" 888% "));
        ttgo->tft->drawString(String(percentage) + "%", 145, 35);
        drawProgressBar(10, 30, 120, 15, percentage, TFT_WHITE, TFT_BLUE);
    })
    .onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");

        ttgo->tft->fillScreen(TFT_BLACK);
        ttgo->tft->drawString("Update Failed", ttgo->tft->width() / 2 - 20, 55 );
        delay(3000);
        otaStart = false;
        initial = 1;
        targetTime = millis() + 1000;
        ttgo->tft->fillScreen(TFT_BLACK);
        ttgo->tft->setTextDatum(TL_DATUM);
        omm = 99;
    });

    ArduinoOTA.begin();
#endif
}

/*-------------------- Setup MQTT Connection ---------------- */
void setupMQTT() {
    client.setServer(mqtt_server,1883);
    client.setCallback(callback);
    ttgo->tft->fillScreen(TFT_BLACK);
    ttgo->tft->setTextColor(TFT_WHITE, TFT_BLACK);
    ttgo->tft->drawCentreString("Establishing MQTT connection...",ttgo->tft->width()/2, ttgo->tft->height()/2,2);    
}


/* ------------------ DISPLAYS ------------------ */

void drawbigX(int duration = 200) {
    ttgo->tft->setTextColor(TFT_RED,TFT_BLACK);
    ttgo->tft->fillScreen(TFT_BLACK);
    uint16_t x1 = ttgo->tft->width()/4;
    uint16_t y1 = ttgo->tft->height()/4;
    for (uint16_t a = 0; a < 5; a++) {
        ttgo->tft->drawLine(x1+a, y1, (3*x1) + a, (3*y1),TFT_RED);
    }
    for (uint16_t a = 0; a < 5; a++) {
        ttgo->tft->drawLine(x1, y1+a, (3*x1), (3*y1) + a,TFT_RED);
    }
    for (uint16_t a = 0; a < 5; a++) {
        ttgo->tft->drawLine(x1+a, (3*y1),(3*x1) + a, y1, TFT_RED);
    }
    for (uint16_t a = 0; a < 5; a++) {
        ttgo->tft->drawLine(x1,(3*y1) - a, (3*x1), y1 - a,TFT_RED);
    }
    
    /*----------- DRAW 'HDPE'------------*/
    ttgo->tft->setTextSize(3);
    ttgo->tft->setTextColor(TFT_YELLOW,TFT_BLACK);
    ttgo->tft->drawCentreString("HDPE",ttgo->tft->width()/2,ttgo->tft->height()/4,2);
    delay(duration);
}

void drawGreenTick(int duration = 200) {
    ttgo->tft->setTextColor(TFT_GREEN,TFT_BLACK);
    ttgo->tft->fillScreen(TFT_BLACK);

    uint16_t x1 = ttgo->tft->width()/4;
    uint16_t y1 = 3 * (ttgo->tft->height()/4);
    uint16_t tenPercentHeight = ttgo->tft->height()/10;
    uint16_t tenPercentWidth = ttgo->tft->width()/10;

    for (uint16_t a = 0; a < 10; a++) {
        ttgo->tft->drawLine(x1+a,y1 - a,x1+tenPercentWidth + a, y1 - a + tenPercentHeight, TFT_GREEN);
    }

    x1 += tenPercentWidth; y1 += tenPercentHeight;

    uint16_t end_x1 = 3 * (ttgo->tft->width()/4);
    uint16_t end_y1 = ttgo->tft->height()/4;

    for (uint16_t a = 0; a < 10; a++) {
        ttgo->tft->drawLine(x1+a,y1 + a,end_x1 + a, end_y1 + a, TFT_GREEN);
    }

    /*----------- DRAW 'PET'------------*/
    ttgo->tft->setTextSize(3);
    ttgo->tft->setTextColor(TFT_GREENYELLOW,TFT_BLACK);
    ttgo->tft->drawCentreString("PET",ttgo->tft->width()/2,ttgo->tft->height()/4,2);

    delay(duration);
}

void drawProgressBar(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint8_t percentage, uint16_t frameColor, uint16_t barColor)
{
    if (percentage == 0) {
        ttgo->tft->fillRoundRect(x0, y0, w, h, 3, TFT_BLACK);
    }
    uint8_t margin = 2;
    uint16_t barHeight = h - 2 * margin;
    uint16_t barWidth = w - 2 * margin;
    ttgo->tft->drawRoundRect(x0, y0, w, h, 3, frameColor);
    ttgo->tft->fillRect(x0 + margin, y0 + margin, barWidth * percentage / 100.0, barHeight, barColor);
}


void HMI_Display() {

    uint16_t midx = ttgo->tft->width()/2;
    uint16_t midy = ttgo->tft->height()/2;
    int length = 100;
    uint16_t leftx,lefty,rightx,righty;

    deg = deg == 180 ? -180 : deg + 10;
    int rightdeg = deg < 160 ? deg + 20 : deg - 340;
    leftx = midx + length * cos(DEG_TO_RAD * deg);
    lefty = midy + length * sin(DEG_TO_RAD * deg);
    rightx = midx + length * cos(DEG_TO_RAD * rightdeg);
    righty = midy + length * sin(DEG_TO_RAD * rightdeg);
    ttgo->tft->setTextColor(TFT_RED,TFT_BLACK);
    ttgo->tft->fillScreen(TFT_BLACK);
    ttgo->tft->fillCircle(midx,midy,40,TFT_SKYBLUE);
    ttgo->tft->fillCircle(midx,midy,26,TFT_BLACK);
    ttgo->tft->fillTriangle(midx,midy,leftx,lefty,rightx,righty,TFT_BLACK);
    delay(50);
    
    
}


void callback(char* topic, byte* message, unsigned int length) {

    /*------------ DEBUG MSG SERIAL PRINT--------------*/
    String messageTemp;
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }
    Serial.println();

    /*-------------LOGIC--------------*/
    if (messageTemp == lastMessage)
        return;
    if (messageTemp == "1") {
        ttgo->motor->onec(1000);
        drawbigX(1000);
        // delay(200);
        // }
    } else if (messageTemp == "0") {
        drawGreenTick(1000);
            // delay(200);
        // }

    }
    lastMessage = messageTemp;
    /*--------- TEXT -----------*/
    ttgo->tft->fillScreen(TFT_BLACK);
    ttgo->tft->setTextColor(TFT_WHITE, TFT_BLACK);
    ttgo->tft->drawString("Pick Object",ttgo->tft->width()/2, ttgo->tft->height()/2);


}



/* --------------------- SETUP AND LOOP ------------ */

void setup()
{
    Serial.begin(115200);
    ttgo = TTGOClass::getWatch();
    ttgo->begin();
    ttgo->openBL();

    //! begin motor attach to 33 pin , In TWatch-2020 it is IO4
    ttgo->motor_begin();

    // ttgo->tft->fillScreen(TFT_BLACK);
    // ttgo->tft->setTextColor(TFT_WHITE, TFT_BLACK);
    // ttgo->tft->drawString("Please pick the right object",  50, 100, 2);
    setupWiFi();
    setupMQTT();
    setupOTA();
    //attach touch screen interrupt pin
    pinMode(TP_INT, INPUT);
}


void loop()
{
    #ifdef ARDUINO_OTA_UPDATE
    ArduinoOTA.handle();
    #endif
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
    now = millis();
    // if(now - lastMsg > 5000) {
    //     lastMsg = now;
    // }
    HMI_Display();
    // if (digitalRead(TP_INT) == LOW) {
    //     ttgo->motor->onec();
    //     delay(200);
    // }
}
