# 5x5 シリアルLED ビット時計 (MH-ET LIVE / ESP32)

## 構成
- 5x5 シリアルLED(WS2812系)マトリクス
- MH-ET LIVE MiniKit (ESP32)
- PlatformIO + Arduinoフレームワーク
- ライブラリ: FastLED

## 配線
| LED側 | MH-ET LIVE側 |
|---|---|
| GND | GND |
| VCC | VCC (5V) |
| IN  | GPIO16 |

25個のLEDを全点灯・フル輝度にすると電流が大きくなる(理論値で約1.5A)ため、
USBバスパワーのみで動かす場合は `BRIGHTNESS`(src/main.cpp内)を低めに
抑えてください。安定動作させたい場合は外部5V電源の使用を推奨します。

## 表示ルール
5列×5行のLEDを、列ごとに5ビットの2進数として使います。
各列の**最下段がビット0(LSB)**、上に行くほど上位ビットです。

| 列(左から) | 内容 | 値の範囲 |
|---|---|---|
| 1列目 | 時刻(24時間表記) | 0〜23 |
| 2列目 | 分の十の位 | 0〜5 |
| 3列目 | 分の一の位 | 0〜9 |
| 4列目 | 秒の十の位 | 0〜5 |
| 5列目 | 秒の一の位 | 0〜9 |

各列は見分けやすいように色分けしています(時=赤、分十=橙、分一=黄、
秒十=緑、秒一=水色)。色は `src/main.cpp` の `COLOR_*` 定数で変更できます。

## 時刻の取得方法
このコードはWi-Fi経由でNTPサーバーに接続し、時刻を取得します(タイムゾーンは
JST=UTC+9に設定済み)。`src/main.cpp` 冒頭の下記部分を、ご自宅のWi-Fi情報に
書き換えてください。

```cpp
const char* WIFI_SSID     = "YOUR_SSID";
const char* WIFI_PASSWORD = "YOUR_PASSWORD";
```

RTCモジュール(DS3231など)を使いたい場合は、`connectWiFi()` と
`configTime()` の呼び出し部分を、RTCライブラリからの時刻取得処理に
置き換えてください。

## 配線パターン(サーペンタイン)について
安価な5x5マトリクスは、配線が「奇数行だけ逆向き」になっている
(ジグザグ配線/サーペンタイン)ことが多いです。コード内の

```cpp
#define MATRIX_SERPENTINE 1
```

がこれに対応する設定です。実際に書き込んでみて表示が列ズレ・鏡写しに
なっている場合は、この値を `0` に変更して再度書き込んでみてください。

## ビルド・書き込み手順 (VS Code + PlatformIO)
1. VS Codeに拡張機能「PlatformIO IDE」をインストール
2. このフォルダ(`bit-clock`)を「Open Folder」で開く
   (自動的にPlatformIOプロジェクトとして認識されます)
3. `src/main.cpp` のWi-Fi設定を書き換える
4. PlatformIOのサイドバーから `mhetesp32minikit` 環境を選択
5. 「Upload」(→アイコン)でビルド・書き込み
6. 「Monitor」でシリアルモニタを開くと、接続状況や現在時刻がログ表示されます

## トラブルシューティング
- **LEDが全く光らない**: 配線(GND/VCC/IN)、電源容量、`LED_PIN`の設定(GPIO16)を確認
- **色や配置がおかしい**: `MATRIX_SERPENTINE` の値を0/1で切り替えて確認
- **Wi-Fiに繋がらない**: SSID/パスワードの誤字、2.4GHz帯のWi-Fiであるか確認(ESP32は5GHz非対応)
- **時刻がズレる**: `GMT_OFFSET_SEC` の値、NTPサーバーへの到達性を確認
