/*
 * 5x5 シリアルLED ビット時計 (行バージョン)
 * ---------------------------------------------------
 * 配線: LED側 GND, VCC, IN -> MH-ET LIVE(ESP32) の GND, VCC, GPIO16
 *
 * 表示レイアウト (行 = 5個、各行5ビット、右端がビット1=LSB / 左端がMSB)
 *   1行目(最上段) : 時   (0-23)  … 5ビットすべて使用
 *   2行目         : 分の十の位 (0-5)
 *   3行目         : 分の一の位 (0-9)
 *   4行目         : 秒の十の位 (0-5)
 *   5行目(最下段) : 秒の一の位 (0-9)
 *
 * 列バージョン(main.cpp)から行と列を入れ替えたレイアウトです。
 * 時刻はWi-Fi経由でNTPサーバーから取得します。
 * RTCモジュールを使う場合はconnectWiFi()/configTime()の部分を
 * RTCライブラリでの時刻取得に置き換えてください。
 */

#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <FastLED.h>

// ==================== ユーザー設定 ====================
// ご自宅のWi-Fi情報に書き換えてください
const char* WIFI_SSID     = "YOUR_SSID";
const char* WIFI_PASSWORD = "YOUR_PASSWORD";

const char* NTP_SERVER1 = "ntp.nict.jp";      // 日本の公的NTPサーバー
const char* NTP_SERVER2 = "pool.ntp.org";
const long  GMT_OFFSET_SEC      = 9 * 3600;   // JST (UTC+9)
const int   DAYLIGHT_OFFSET_SEC = 0;          // 日本はサマータイムなし

#define LED_PIN        16   // GPIO16
#define MATRIX_WIDTH   5
#define MATRIX_HEIGHT  5
#define NUM_LEDS       (MATRIX_WIDTH * MATRIX_HEIGHT)
#define BRIGHTNESS     40   // 0-255。全灯だと電流が大きいので控えめに

// 配線がジグザグ(サーペンタイン)方式なら 1、
// 各行とも同じ方向(左→右)に配線されているなら 0 にしてください。
// 表示が列ズレ/鏡写しになる場合はここを反転して試してください。
#define MATRIX_SERPENTINE 1
// ======================================================

CRGB leds[NUM_LEDS];

// 行(物理位置)ごとの表示色（見分けやすいように色分け）
const CRGB COLOR_HOUR     = CRGB(255,   0,   0); // 時       : 赤
const CRGB COLOR_MIN_TENS = CRGB(255, 120,   0); // 分の十の位: オレンジ
const CRGB COLOR_MIN_ONES = CRGB(255, 255,   0); // 分の一の位: 黄
const CRGB COLOR_SEC_TENS = CRGB(  0, 255,   0); // 秒の十の位: 緑
const CRGB COLOR_SEC_ONES = CRGB(  0, 128, 255); // 秒の一の位: 水色

// 列(col:0-4)・物理行(physRow:0=最上段)からLEDのシリアル番号を求める
int ledIndex(uint8_t col, uint8_t physRow) {
#if MATRIX_SERPENTINE
  if (physRow % 2 == 0) {
    return physRow * MATRIX_WIDTH + col;
  } else {
    return physRow * MATRIX_WIDTH + (MATRIX_WIDTH - 1 - col);
  }
#else
  return physRow * MATRIX_WIDTH + col;
#endif
}

// 1行分をビット表示する
// bit0(LSB)を右端に、左に行くほど上位ビットになるように表示
void showRow(uint8_t physRow, uint8_t value, CRGB color) {
  for (uint8_t bit = 0; bit < MATRIX_WIDTH; bit++) {
    uint8_t col = (MATRIX_WIDTH - 1) - bit; // bit0 -> 右端の列
    int idx = ledIndex(col, physRow);
    if ((value >> bit) & 0x01) {
      leds[idx] = color;
    } else {
      leds[idx] = CRGB::Black;
    }
  }
}

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Wi-Fiに接続中");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("接続完了 IP: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  delay(200);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  FastLED.show();

  connectWiFi();

  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER1, NTP_SERVER2);

  Serial.println("NTP時刻同期を待っています...");
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("時刻同期完了");
}

int lastSecondShown = -1;

void loop() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    delay(200);
    return;
  }

  // 秒が変わった時だけ描画（無駄な再描画を防ぐ）
  if (timeinfo.tm_sec == lastSecondShown) {
    delay(50);
    return;
  }
  lastSecondShown = timeinfo.tm_sec;

  uint8_t hour   = timeinfo.tm_hour; // 0-23
  uint8_t minute = timeinfo.tm_min;  // 0-59
  uint8_t second = timeinfo.tm_sec;  // 0-59

  uint8_t minTens = minute / 10;
  uint8_t minOnes = minute % 10;
  uint8_t secTens = second / 10;
  uint8_t secOnes = second % 10;

  showRow(0, hour,    COLOR_HOUR);     // 1行目(最上段)
  showRow(1, minTens, COLOR_MIN_TENS); // 2行目
  showRow(2, minOnes, COLOR_MIN_ONES); // 3行目
  showRow(3, secTens, COLOR_SEC_TENS); // 4行目
  showRow(4, secOnes, COLOR_SEC_ONES); // 5行目(最下段)

  FastLED.show();

  Serial.printf("%02d:%02d:%02d\n", hour, minute, second);
}
