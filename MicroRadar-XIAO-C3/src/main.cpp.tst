// ============================================================
// ディスプレイ単体テスト用スケッチ
//
// 使い方:
//   1. 今の src/main.cpp を src/main.cpp.bak などにリネームして退避
//   2. このファイルを src/main.cpp としてコピー(拡張子を .cpp に)
//   3. ビルド・書き込み
//   4. 赤 → 緑 → 青 → 白地に黒文字 が1秒ごとに切り替われば
//      ディスプレイの配線・LGFX設定は正しいということです
//   5. 確認できたら元の main.cpp.bak を main.cpp に戻してください
// ============================================================

#include <Arduino.h>
#include "LGFX.h"

LGFX tft;

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println("Display test start");

    tft.init();
    tft.setRotation(0);

    Serial.print("Panel width x height: ");
    Serial.print(tft.width());
    Serial.print(" x ");
    Serial.println(tft.height());
}

void loop()
{
    Serial.println("RED");
    tft.fillScreen(TFT_RED);
    delay(1000);

    Serial.println("GREEN");
    tft.fillScreen(TFT_GREEN);
    delay(1000);

    Serial.println("BLUE");
    tft.fillScreen(TFT_BLUE);
    delay(1000);

    Serial.println("WHITE + TEXT");
    tft.fillScreen(TFT_WHITE);
    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextDatum(lgfx::middle_center);
    tft.drawString("TEST OK", tft.width() / 2, tft.height() / 2);
    delay(1000);
}
