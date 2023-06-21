/**
* ThermoLogger
**/

// for OLED
#include <Wire.h> 
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
SSD1306AsciiWire OLED;

// for Thermo-Couple
#include <SPI.h>
#include "Adafruit_MAX31855.h"
#define MAXCS     10
Adafruit_MAX31855 thermocouple(MAXCS);

// for Switch
#define PIN_SW 7 // for Reset Switch
unsigned long prev_Trg;
unsigned long volatile curr_Trg;

// for Logger
unsigned long time_log, time_stamp;

void setup() {  
  // ピンのアサイン
  pinMode(PIN_SW, INPUT_PULLUP);
  prev_Trg = millis(); // 前回トリガー時刻をセット
  attachInterrupt(4, catchSW, FALLING); // 割り込みInt4(pin7)をスイッチに割り付け

  // OLEDの初期化
  Wire.begin();
  Wire.setClock(400000L);
  OLED.begin(&Adafruit128x32, 0x3C);

  OLED.setFont(Callibri15);
  OLED.clear();
  OLED.setCursor(15, 1); OLED.print("THERMO-LOGGER");

  // シリアルの初期化
  Serial.begin(9600);
  //while (!Serial) delay(1); // wait for Serial on Leonardo

  // MAX31855の初期化
  delay(1000); // wait for MAX chip to stabilize
  if (!thermocouple.begin()) {
    OLED.clear(); OLED.setCursor(15, 1); OLED.print("INIT ERROR!");
    while (1) delay(10); // 無限ループに落とす
  }
  delay(1000);
  OLED.clear();

  time_log = -1; // 起動時間をセット
  time_stamp = 0; // タイムスタンプをセット
}

void loop() {
  // スイッチの割り込みを検出した場合の処理
  if (curr_Trg - prev_Trg >= 200) { // 割り込みを検出し、前回トリガー時刻からの経過時間が200m秒以上だった
    prev_Trg = curr_Trg; // トリガー時刻の更新
    time_stamp = 0; // タイムスタンプのリセット
  }

  // ロガー機能の実装
  if (millis() - time_log > 1000) { // 前回ロギング時より1000ms以上経過していた
    time_stamp += millis() - time_log;
    time_log = millis(); // ロギング時刻を更新
    double c = thermocouple.readCelsius(); // 熱電対温度を摂氏で取得
    if (isnan(c)) { // 取得できてなかった場合
      uint8_t e = thermocouple.readError(); // エラー情報の取得
      OLED.setFont(Callibri15);
      if (e & MAX31855_FAULT_OPEN) { // 回路オープン
        Serial.println("THERMOCOUPLE IS OPEN!");
        OLED.clear(); OLED.setCursor(15, 1); OLED.print("TC OPEN !");
      }
      if (e & MAX31855_FAULT_SHORT_GND) { // GNDショート
        Serial.println("THERMOCOUPLE IS SHORT-GND!");
        OLED.clear(); OLED.setCursor(15, 1); OLED.print("SHORT GND !");
      }
      if (e & MAX31855_FAULT_SHORT_VCC) { // VCCショート
        Serial.println("THERMOCOUPLE IS SHORT-VCC!");
        OLED.clear(); OLED.setCursor(15, 1); OLED.print("SHORT VCC !");
      }
    } else { // 取得できてた場合
      Serial.print(c); Serial.print(", "); Serial.println(time_stamp); // シリアル出力      
      OLED.setFont(fixed_bold10x15);
      OLED.clear(); OLED.setCursor(12, 1);
      if (c > 0) OLED.print('+');
      OLED.print(c);
      OLED.setFont(Callibri10); OLED.setCursor(110, 0); OLED.print("o");
      OLED.setFont(fixed_bold10x15); OLED.setCursor(114, 1); OLED.print("C");
    }
  }
}

void catchSW() { curr_Trg = millis(); }