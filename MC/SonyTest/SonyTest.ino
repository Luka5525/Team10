#include <SoftwareSerial.h>
#include <ros.h>
#include <std_msgs/String.h>

#include <SPI.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789


#define PBR 7
#define PBC 6
#define PBL 5

#define TFT_CS        10
#define TFT_RST        9
#define TFT_DC         8

#define LENGTH(X) (sizeof X/sizeof X[0])

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
//Adafruit_ST7789 tft = Adafruit_ST7789(&SPI4, TFT_CS, TFT_DC, TFT_RST);
//Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
char guipos = 0;
char* guitext[4] = {"Move", "Right", "Left", "Stop"};
char commandtxt[4] = {'M', 'R', 'L', 'S'};

bool transmit = false;
bool buttonpress[3] = {false, false, false};
bool DF = 1;

unsigned long responseT;

void guidown() {
  if (guipos > 0) {
    tft.setCursor(10, guipos * 34);
    tft.setTextColor(ST77XX_BLACK);
    tft.print(">");
    guipos--;
    tft.setCursor(10, guipos * 34);
    tft.setTextColor(ST77XX_WHITE);
    tft.print(">");
  }
}

void guiup() {
  if (guipos < LENGTH(guitext) - 2) {
    tft.setCursor(10, guipos * 34);
    tft.setTextColor(ST77XX_BLACK);
    tft.print(">");
    guipos++;
    tft.setCursor(10, guipos * 34);
    tft.setTextColor(ST77XX_WHITE);
    tft.print(">");
  }
}


void left() {
  //Serial.print("Left");
  static unsigned long OldIntTimeL;
  unsigned long IntTimeL = millis();
  if ((IntTimeL - OldIntTimeL) > 100) {
    guidown();
  }
  OldIntTimeL = IntTimeL;
}
void right() {
  //Serial.print("Right");
  static unsigned long OldIntTimeR;
  unsigned long IntTimeR = millis();
  if ((IntTimeR - OldIntTimeR) > 100) {
    guiup();
  }
  OldIntTimeR = IntTimeR;
}
void centre() {
  static unsigned long OldIntTimeC;
  unsigned long IntTimeC = millis();
  if ((IntTimeC - OldIntTimeC) > 100) {
    Serial.print(guitext[guipos]);
    Serial2.write(commandtxt[guipos]);
    responseT = millis();
    screenstate(commandtxt[guipos]);
  }
  OldIntTimeC = IntTimeC;
}
void setup() {
  Serial.begin(9600);//Serial for debugging
  //TRN.begin(115200); //Serial To ESP32
  Serial2.begin(115200);
  tftsetup();

  pinMode(PBL, INPUT);
  pinMode(PBC, INPUT);
  pinMode(PBR, INPUT);
  attachInterrupt(digitalPinToInterrupt(PBL), left, FALLING);
  attachInterrupt(digitalPinToInterrupt(PBR), right, FALLING);
  attachInterrupt(digitalPinToInterrupt(PBC), centre, FALLING);

  delay(1000);
}

void loop() {
  if (Serial2.available()) {
    char cmd = Serial2.read();
    if (cmd == "C"){
      screenstate("C");  
    }
    if (cmd == "F"){
      screenstate("F");  
    }
    float T = millis() - responseT;
    Serial.print("\n");
    Serial.print(T);
  }
}
void screenstate(char ScrSt) {
  switch (ScrSt) {
    case 'F':
      detachInterrupt(digitalPinToInterrupt(PBL));
      detachInterrupt(digitalPinToInterrupt(PBR));
      tft.setCursor(10, guipos * 34);
      tft.setTextColor(ST77XX_BLACK);
      tft.print(">");
      guipos = 3;
      tft.fillScreen(ST77XX_BLACK);
      tft.setTextColor(ST77XX_WHITE);
      tft.setCursor(0, 0);
      tft.print("Movement in\nprogress, \npress fire to stop");
      DF = 0;
      break;
    default:
      if (DF == 0) {
        attachInterrupt(digitalPinToInterrupt(PBL), left, FALLING);
        attachInterrupt(digitalPinToInterrupt(PBR), right, FALLING);
        tft.fillScreen(ST77XX_BLACK);
        tft.setTextColor(ST77XX_WHITE);
        guipos = 0;
        tft.setCursor(10, 0);
        tft.setTextColor(ST77XX_WHITE);
        tft.print(">");
        tft.setTextSize(3);
        for (int i = 0; (i < (LENGTH(guitext) - 1)); i++) {
          tft.setCursor(35, i * 32);
          tft.print(guitext[i]);
        }
      }
      DF = 1;
      break;
  }
}
void tftsetup() {
  tft.init(240, 240);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);
  for (int i = 0; (i < (LENGTH(guitext) - 1)); i++) {
    tft.setCursor(35, i * 32);
    tft.print(guitext[i]);
  }
  guipos = 0;
  tft.setCursor(10, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.print(">");
}
