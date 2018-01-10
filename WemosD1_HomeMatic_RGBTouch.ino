#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Arduino.h>
#include <FS.h>
#include <ArduinoOTA.h>
#include <UTFT.h>
#include <SPI.h>
#include <XPT2046.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>

#define ESP_SPI_FREQ 16000000L

#define IPSIZE                16
#define VARIABLESIZE         255
#define ConfigPortalTimeout  180  //Timeout (Sekunden) des AP-Modus
#define HTTPTimeOut         1500  //Timeout (Millisekunden) für http requests
#define SYNCINTERVAL         30   //Sekunden

const String configJsonFile         = "tc.json";

extern unsigned  wifiicon32[1024];
extern unsigned  RGBhorz_halb[27948];

UTFT myGLCD (ILI9341_S5P, 15, 5, 2);
XPT2046 myTouch(/*cs=*/ 4, /*irq=*/ 5);

HTTPClient http;

char Hostname[] = "WEMOS-HM-RGBTOUCH";

struct wemosnetconfig_t {
  char ip[IPSIZE]      = "0.0.0.0";
  char netmask[IPSIZE] = "0.0.0.0";
  char gw[IPSIZE]      = "0.0.0.0";
} WemosNetConfig;

struct hmconfig_t {
  char ccuIP[IPSIZE]   = "";
  char RGB_Dimmer_Serial[VARIABLESIZE] = "";
  char UP_Dimmer_Serial[VARIABLESIZE] =  "";
  bool enableSync = false;
  byte OnLevel = 100;
} HomeMaticConfig;

bool isDimTouching = false;
bool isRGBTouching = false;
int posX = 20; // Start x
int posY = 10; // Start y
int posEndX = 274;
int posEndY = 65;
int percentX = 312;
int percentY = 8;

int buttonWidth = 50;
int buttonHeight = 60;
int buttonOffX = 20;
int buttonOffY = 170;
int buttonOnX = 244;
int buttonOnY = 170;

int wifiIconX = 210;
int wifiIconY = 108;
int RGBstartX = 20;
int RGBstartY = 100;
int RGBheight = 274;
int RGBwidth = 25;
int RGBbarCount = 2;

boolean isFrameSet = false; // Wert wird true wenn Frame erstmalig geschrieben - nicht manuell anpassen!
extern uint8_t BigFont[];
int intAusgabe;
unsigned long lastSyncMillis = 0;

//bool Debug = false;
bool startWifiManager = false;
bool startCalibration = false;
bool wm_shouldSaveConfig = false;
bool isRGBavailable = true;
bool isDIMavailable = true;
String lastLEVELValue = "";
String lastCOLORValue = "";


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(D0, INPUT);
  SPI.setFrequency(ESP_SPI_FREQ);
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  myGLCD.setColor(255, 255, 255);
  Serial.begin(115200);

  myTouch.begin((uint16_t)myGLCD.getDisplayYSize(), (uint16_t)myGLCD.getDisplayXSize());

  //startCalibration = (digitalRead(D0) == HIGH) ;

  if (startCalibration) {
    calibrate();
    ESP.restart();
  }


  myTouch.setRotation(myTouch.ROT270);
  myTouch.setCalibration(244, 1735, 1728, 264);


  if (!loadSystemConfig()) startWifiManager = true;
  if (myTouch.isTouching()) startWifiManager = true;

  isRGBavailable = (String(HomeMaticConfig.RGB_Dimmer_Serial) != "");
  isDIMavailable = (String(HomeMaticConfig.UP_Dimmer_Serial) != "");


  if (doWifiConnect() == false) {
    myGLCD.clrScr();
    myGLCD.setFont(BigFont);
    myGLCD.setColor(255, 255, 255);
    myGLCD.print("  Fehler bei ", 175, 00, 90);
    myGLCD.print("WLAN-Verbindung", 155, 00, 90);
    delay(2000);
    ESP.restart();
  }
  else {
    startOTAhandling();
    myGLCD.clrScr();
    slider_init();
    if (isDIMavailable) on_off_button_init();
  }
}

void loop() {
  ArduinoOTA.handle();
  if (lastSyncMillis > millis())
    lastSyncMillis = millis();

  if (WiFi.status() == WL_CONNECTED) {
    slider_read();
    if (millis() - lastSyncMillis > SYNCINTERVAL * 1000) {
      Serial.println("SYNC HM");
      lastSyncMillis = millis();
      slider_init();
    }

    yield();

  } else ESP.restart();
}

void slider_init() {
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRect (posX, posY, posX + posEndX, posY + posEndY);
  bool CCUerror = false;
  if (!isRGBavailable) {
    memcpy(HomeMaticConfig.RGB_Dimmer_Serial, HomeMaticConfig.UP_Dimmer_Serial, VARIABLESIZE);
    Serial.println("slider_init(): RGB Dimmer NOT available");
  }
  //aktuellen LEVEL holen und anzeigen
  String currentLEVELValue = getStateFromCCU(HomeMaticConfig.RGB_Dimmer_Serial, "1.LEVEL");
  if (currentLEVELValue != lastLEVELValue) {
    lastLEVELValue = currentLEVELValue;
    CCUerror = !drawCurrentDimLevel(currentLEVELValue);
  }

  if (isRGBavailable) {
    String currentCOLORValue = getStateFromCCU(HomeMaticConfig.RGB_Dimmer_Serial, "2.COLOR");
    if (currentCOLORValue != "error") {
      if (currentCOLORValue != lastCOLORValue) {
        lastCOLORValue = currentCOLORValue;
        myGLCD.setColor(255, 255, 255);

        int knobPosX = (((float)currentCOLORValue.toInt() / 200.0) + ((float)RGBstartX / (float)RGBheight)) * (float)RGBheight;
        draw_knob(knobPosX);
        yield();
      }
    } else {
      CCUerror = true;
    }
  }

  if (CCUerror) {
    myGLCD.clrScr();
    myGLCD.setFont(BigFont);
    myGLCD.setColor(255, 255, 255);
    myGLCD.print("  Fehler bei ", 175, 00, 90);
    myGLCD.print("CCU2-Verbindung", 155, 00, 90);
    myGLCD.setColor(255, 0, 0);
    myGLCD.drawLine(319, 0, 319, 240);
    myTouch.powerDown();
  } else {
    myGLCD.setColor(0, 255, 0);
    myGLCD.drawLine(319, 0, 319, 240);
  }
}

void on_off_button_init() {
  //AN / AUS BUTTONS
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (buttonOffX, buttonOffY, buttonOffX + buttonWidth, buttonOffY + buttonHeight);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (buttonOffX, buttonOffY, buttonOffX + buttonWidth, buttonOffY + buttonHeight);

  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (buttonOnX, buttonOnY, buttonOnX + buttonWidth, buttonOnY + buttonHeight);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (buttonOnX, buttonOnY, buttonOnX + buttonWidth, buttonOnY + buttonHeight);

  myGLCD.setColor(0, 255, 0);
  myGLCD.print("AUS",   52, 177, 90);
  myGLCD.print("AN",   275, 185, 90);
  //
}

// Slider auslesen und Werte zurückgeben
int slider_read() {
  float tPosX;
  float tPosY;
  float tAusgabe;
  if (myTouch.isTouching()) {
    //Serial.println("Touch detected.");
    uint16_t x, y;
    int xCumul = 0;
    int yCumul = 0;
    for (int touchCount = 0; touchCount < 100; touchCount++) {
      myTouch.getPosition(x, y);
      xCumul += x;
      yCumul += y;
    }
    tPosX = xCumul / 100;
    tPosY = yCumul / 100;
    //Serial.println("tPosX = " + String(tPosX) + "; tPosY = " + String(tPosY));

    if (isDIMavailable) {
      if ((tPosY > buttonOffY) && (tPosY < buttonOffY + buttonHeight)) {
        if ((tPosX > buttonOffX) && (tPosX < buttonOffX + buttonWidth)) {
          waitForIt(buttonOffX, buttonOffY, buttonOffX + buttonWidth, buttonOffY + buttonHeight);
          Serial.println("OFF BUTTON pressed.");
          setStateCCU(HomeMaticConfig.UP_Dimmer_Serial, "1.LEVEL", "0.0");
          if (!isRGBavailable) drawCurrentDimLevel("0");
        }
      }

      if ((tPosY > buttonOnY) && (tPosY < buttonOnY + buttonHeight)) {
        if ((tPosX > buttonOnX) && (tPosX < buttonOnX + buttonWidth)) {
          waitForIt(buttonOnX, buttonOnY, buttonOnX + buttonWidth, buttonOnY + buttonHeight);
          Serial.println("ON BUTTON pressed. ON_LEVEL = " + String(HomeMaticConfig.OnLevel));
          setStateCCU(HomeMaticConfig.UP_Dimmer_Serial, "1.LEVEL", String((float)(HomeMaticConfig.OnLevel / 100.0)));
          if (!isRGBavailable) drawCurrentDimLevel(String((float)(HomeMaticConfig.OnLevel / 100.0)));
        }
      }
    }

    if (tPosX >= posX && tPosX <= posX + posEndX && tPosY >= posY && tPosY <= posY + posEndY) {
      isDimTouching = true;
      draw_used_unused(tPosX, tPosY);
      tAusgabe = ((tPosX - posY) / posEndX) * 100;
      intAusgabe = (int)tAusgabe - 3;
      String displayPercent = "   " + String(intAusgabe);
      displayPercent = displayPercent.substring(displayPercent.length() - 3) + "%";
      myGLCD.setColor(255, 255, 255);
      myGLCD.print(displayPercent, percentX, percentY, 90);
    }
    //RGB
    if (isRGBavailable && tPosX >= RGBstartX && tPosX <= RGBstartX + RGBheight && tPosY >= RGBstartY && tPosY <= RGBstartY +  (RGBwidth * RGBbarCount)) {
      draw_knob(tPosX);
      isRGBTouching = true;
      tAusgabe = ((tPosX - RGBstartX) / RGBheight) * 200;
      intAusgabe = (int) tAusgabe;
    }
  }
  else {
    if (isDimTouching) {
      if (intAusgabe < 4) intAusgabe = 0;
      String val = "00" + String(intAusgabe);
      val = "0." + val.substring(val.length() - 2);
      Serial.println("Setze CCU Wert " + String(HomeMaticConfig.RGB_Dimmer_Serial) + "1.LEVEL = " + val);
      setStateCCU(HomeMaticConfig.RGB_Dimmer_Serial, "1.LEVEL", val);
      isDimTouching = false;
    }
    //RGB
    if (isRGBTouching) {
      Serial.println("Setze CCU Wert " + String(HomeMaticConfig.RGB_Dimmer_Serial) + "2.COLOR = " + String(intAusgabe));
      setStateCCU(HomeMaticConfig.RGB_Dimmer_Serial, "2.COLOR", String(intAusgabe));
      isRGBTouching = false;
    }
  }
}

// Draw a red frame while a button is touched
void waitForIt(int x1, int y1, int x2, int y2) {
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
  while (myTouch.isTouching()) yield(); //loop
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
}

// Schieberegler
void draw_knob(int tPosX) {
  word tColor;
  for (int barCount = 0; barCount < RGBbarCount; barCount++)
    myGLCD.drawBitmap (RGBstartX, RGBstartY + (RGBwidth * barCount), RGBheight, RGBwidth, RGBhorz_halb, 1);
  tColor = myGLCD.getColor();
  myGLCD.setColor(255, 255, 255);

  myGLCD.fillRect(tPosX - 1, RGBstartY + ( (RGBwidth * RGBbarCount) / 2) - ( (RGBwidth * RGBbarCount) / 4), tPosX, RGBstartY + ( (RGBwidth * RGBbarCount) / 2) + ( (RGBwidth * RGBbarCount) / 4));
  // myGLCD.drawLine(posX, RGBstartY + ( (RGBwidth * RGBbarCount) / 2), tPosX, RGBstartY + ( (RGBwidth * RGBbarCount) / 2));

  myGLCD.setColor(tColor);
}

void draw_used_unused(int tPosX, int tPosY) {
  //Serial.println("draw_used_unused("+String(tPosX)+","+String(tPosY)+")");
  const word usedColor = VGA_WHITE; // Farbe f. genutzen Anteil
  const word unusedColor = VGA_BLACK; // Farbe f. ungenutzen Anteil
  word tColor;
  tColor = myGLCD.getColor();
  myGLCD.setColor(usedColor);
  for (int tPosY = posY; tPosY <= posY + posEndY; tPosY++) myGLCD.drawLine(posX, tPosY, tPosX, tPosY);
  myGLCD.setColor(tColor);
  myGLCD.setColor(unusedColor);
  for (int tPosY = posY; tPosY <= posY + posEndY; tPosY++) myGLCD.drawLine(tPosX, tPosY, posX + posEndX, tPosY);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRect (posX, posY, posX + posEndX, posY + posEndY);
  myGLCD.setColor(tColor);
}

String IpAddress2String(const IPAddress& ipAddress) {
  return String(ipAddress[0]) + String(".") + \
         String(ipAddress[1]) + String(".") + \
         String(ipAddress[2]) + String(".") + \
         String(ipAddress[3]);
}
