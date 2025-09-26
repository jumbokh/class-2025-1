好的！下面以 **ESP32-CAM**（您圖示的版型）為平台，在 **2 小時內**完成 5 個 Arduino 類比實驗。每個實驗都含：**實驗目標、所需器材、腳位與接線、需安裝的程式庫、步驟、程式、預期結果與注意事項**。
（說明：ESP32-CAM 全部 I/O 皆為 **3.3V** 邏輯；若您同時啟用相機/SD，請避用相機/SD 既用腳位。本單元不使用相機；**Neopixel 依您指定接在 GPIO32**。個別模組若與您板型腳位衝突，僅需把 `#define` 內的腳位換成您可用腳位即可。）

---

# 時間規劃（建議）

* 安裝庫與燒錄測試：10 分
* 實驗1（LDR 讀值）：15 分
* 實驗2（夜燈：LDR→板載白光 Flash/LED）：20 分
* 實驗3（Neopixel 顏色/關閉）：20 分
* 實驗4（MQ-2 類比讀值）：20 分
* 實驗5（MQ-2 + Neopixel 告警顏色）：25 分
* 總結與問答：10 分
  （合計約 2 小時）

---

## 共同環境準備

* **開發環境**：Arduino IDE 2.x（已安裝 ESP32 開發板套件 *esp32 by Espressif Systems*）
* **板型**：Tools → Board → **ESP32 Arduino → AI Thinker ESP32-CAM**（或相容板）
* **上傳**：GPIO0 拉到 GND 進入下載模式；燒錄後拔除再上電。
* **電源**：建議 5V/1A 以上（Micro-USB 口或 5V pin）。
* **重要提示**：GPIO34/35/36/39 為**輸入專用**，不可驅動負載。

---

## 實驗 1：光敏電阻（LDR）類比讀值

**目標**
讀取環境亮度並在序列監視器輸出數值。

**器材**

* LDR×1、10 kΩ 電阻×1（與 LDR 組成分壓）
* 杜邦線若干

**腳位與接線**

* **ADC 腳**：GPIO34（輸入專用，穩定）
* 接法（分壓）：`3.3V ─ LDR ──┬──> GPIO34`；`GND ─ 10kΩ ──┘`
  （亮度變化造成分壓點電壓改變）

**需安裝程式庫**：無

**步驟**
接線 → 開啟序列監視器 115200 → 上傳程式 → 遮擋/照亮 LDR 觀察數值。

**程式（可直接燒錄）**

```cpp
// EXP1_LDR_Read.ino
const int PIN_LDR = 34; // ADC1_CH6，輸入專用
void setup() {
  Serial.begin(115200);
  analogReadResolution(12);   // 0~4095
  analogSetAttenuation(ADC_11db); // 擴大量測範圍
}
void loop() {
  int raw = analogRead(PIN_LDR);
  float volt = raw * (3.3 / 4095.0);
  Serial.printf("LDR raw=%d  V=%.3f\n", raw, volt);
  delay(300);
}
```

**預期結果**
亮度升高→`raw` 增/減（依接法而定，本接法通常**越亮電壓越高**）。

**注意**

* 量測易受雜訊干擾，可多次取平均平滑。

---

## 實驗 2：夜燈（LDR 觸發板載白光 LED）

**目標**
暗到一定程度自動點亮板載白光（ESP32-CAM 多為 **GPIO4** 的白光/閃光 LED）。

**器材**

* 沿用實驗1 LDR 分壓
* 板載白光 LED（GPIO4）

**腳位與接線**

* LDR：同實驗1（GPIO34）
* **白光 LED**：GPIO4（輸出）

**程式庫**：無

**步驟**
設定亮度閾值，低於（或高於）即開燈。

**程式**

```cpp
// EXP2_NightLight.ino
const int PIN_LDR = 34;
const int PIN_FLASH = 4;   // 板載白光LED
const int THRESH = 1800;   // 視場域可調整

void setup(){
  Serial.begin(115200);
  pinMode(PIN_FLASH, OUTPUT);
  digitalWrite(PIN_FLASH, LOW); // 多數板上 LOW=關
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
}

void loop(){
  int raw = analogRead(PIN_LDR);
  bool dark = (raw < THRESH); // 依您接法選擇 < 或 >
  digitalWrite(PIN_FLASH, dark ? HIGH : LOW); // 多數板 HIGH=亮
  Serial.printf("raw=%d  dark=%d\n", raw, dark);
  delay(200);
}
```

**預期結果**
遮住 LDR → 白光亮；照明 → 白光熄。

**注意**

* 若您的板上白光為反相，交換 `HIGH/LOW` 即可。

---

## 實驗 3：板上 Neopixel 顏色與關閉（Pin **32**）

**目標**
控制單顆 Neopixel 顯示紅/綠/藍與關閉。

**器材**

* Neopixel（WS2812/WS2812B/同類型）×1
* 330–470 Ω 串聯電阻（資料線保護，建議）
* 1000 µF/6.3V 電解電容（VCC-GND 間抑制突波，建議）

**腳位與接線**

* **資料線**：GPIO32 →（串 330Ω）→ DIN
* 供電：Neopixel 取 **5V**（或 3.3V，小顆可行）與 GND；**GND 必須與 ESP32 共地**
* **注意**：某些 ESP32-CAM 方案把 GPIO32 給了 Camera PWDN。您本課程 **不啟用相機**，故可用。若現場衝突，改用 **GPIO15** 或 **GPIO2**（相機未用時）並在程式中改腳位。

**需安裝程式庫**

* **Adafruit_NeoPixel**（Library Manager 直接安裝）

**程式**

```cpp
// EXP3_Neopixel_Basic.ino
#include <Adafruit_NeoPixel.h>
#define PIN_NEOPIXEL 32
#define NUM_PIXELS   1
Adafruit_NeoPixel pixels(NUM_PIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

void setup() {
  pixels.begin();
  pixels.clear();
  pixels.show();
}

void loop() {
  // 紅
  pixels.setPixelColor(0, pixels.Color(255, 0, 0)); pixels.show(); delay(800);
  // 綠
  pixels.setPixelColor(0, pixels.Color(0, 255, 0)); pixels.show(); delay(800);
  // 藍
  pixels.setPixelColor(0, pixels.Color(0, 0, 255)); pixels.show(); delay(800);
  // 關閉
  pixels.clear(); pixels.show(); delay(800);
}
```

**預期結果**
Neopixel 依序顯示紅→綠→藍→熄滅循環。

**注意**

* **務必共地**；若用 5V 供電，ESP32 的 3.3V 資料通常仍可驅動 1 顆 Neopixel，但線要短，且建議加串阻與電解電容。

---

## 實驗 4：MQ-2 可燃氣體感測（類比輸出）

**目標**
讀取 MQ-2 模組 AO 的類比電壓，觀察濃度趨勢。

**器材**

* MQ-2 模組×1（帶 AO）
* **電阻分壓器**：100 kΩ ×2（把 AO 的 0–5V 降到 0–2.5V，保護 ADC）
* 杜邦線

> MQ-2 加熱絲建議 **5V 供電**，暖機數分鐘數值才穩定。直接把 AO 接 ESP32 會超壓，**必須分壓**或改用 3.3V 供電之模組。

**腳位與接線**

* MQ-2：`VCC→5V`、`GND→GND`
* AO → **分壓**上端 →（下端接 GND，中點→GPIO33）

  * 分壓：AO ─ 100kΩ ─┬─> **GPIO33**（ADC）
    　　　　　　　　GND ─ 100kΩ ─┘

**程式庫**：無

**程式**

```cpp
// EXP4_MQ2_Read.ino
const int PIN_MQ2 = 33; // ADC1_CH5
const int WARMUP_MS = 5000;

void setup(){
  Serial.begin(115200);
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  Serial.println("MQ-2 warming up...");
  delay(WARMUP_MS);
}

int readAvg(int pin, int n=10){
  long s=0; for(int i=0;i<n;i++){ s += analogRead(pin); delay(5); }
  return s / n;
}

void loop(){
  int raw = readAvg(PIN_MQ2);
  float v = raw * (3.3 / 4095.0);   // 分壓後實際量到的是 0~2.5V，換算不用特別處理
  Serial.printf("MQ-2 raw=%d  V=%.3f\n", raw, v);
  delay(300);
}
```

**預期結果**
靠近酒精/打火機瓦斯（請勿點火）數值上升，遠離或通風數值下降。

**注意**

* 安全第一，僅用極少量揮發源測試；避免火源。
* MQ-2 需「暖機」；不同模組靈敏度差異大，讀值僅作趨勢判斷。

---

## 實驗 5：MQ-2 + Neopixel 告警夜燈

**目標**
以 MQ-2 讀值決定 Neopixel 顏色：安全（綠）、注意（黃）、危險（紅），並可在暗環境自動開啟白光。

**器材**

* 綜合使用：LDR 分壓（GPIO34）、MQ-2 分壓（GPIO33）、Neopixel（GPIO32）、板載白光（GPIO4）

**程式庫**

* **Adafruit_NeoPixel**

**程式**

```cpp
// EXP5_MQ2_Neopixel_Alarm.ino
#include <Adafruit_NeoPixel.h>
#define PIN_NEOPIXEL 32
#define NUM_PIXELS   1
#define PIN_LDR      34
#define PIN_MQ2      33
#define PIN_FLASH    4

// 依現場調整：請先觀察 EXP4 的 raw 值分佈後設定
int TH_SAFE  = 1200;  // 低於 → 綠
int TH_WARN  = 2000;  // 介於 SAFE~WARN → 黃；高於 WARN → 紅

Adafruit_NeoPixel pix(NUM_PIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

int readAvg(int pin, int n=8){ long s=0; for(int i=0;i<n;i++){ s+=analogRead(pin); delay(3);} return s/n; }

void setup(){
  Serial.begin(115200);
  pinMode(PIN_FLASH, OUTPUT); digitalWrite(PIN_FLASH, LOW);
  pix.begin(); pix.clear(); pix.show();
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
}

void setPixel(uint8_t r,uint8_t g,uint8_t b){ pix.setPixelColor(0, pix.Color(r,g,b)); pix.show(); }

void loop(){
  int mq = readAvg(PIN_MQ2);
  int ldr = readAvg(PIN_LDR);

  // Neopixel 顏色邏輯
  if(mq < TH_SAFE)          setPixel(0, 180, 0);     // 綠
  else if(mq < TH_WARN)     setPixel(180, 120, 0);   // 黃
  else                      setPixel(200, 0, 0);     // 紅

  // 簡易夜燈（可選）：太暗就開白光
  bool dark = (ldr < 1800);
  digitalWrite(PIN_FLASH, dark ? HIGH : LOW);

  Serial.printf("MQ2=%d  LDR=%d  dark=%d\n", mq, ldr, dark);
  delay(250);
}
```

**預期結果**

* 空氣清新→Neopixel 綠；揮發略高→黃；高濃度→紅。
* 室內變暗→板載白光自動開；亮→關。

**注意**

* 閾值須依場地校準。
* 若 GPIO32 與相機硬體衝突，請把 `PIN_NEOPIXEL` 改成您可用腳位（如 15/2），並重新接線。

---

# 總結與教學備註

* **學習重點**：ADC 量測、分壓原理、Neopixel 時序控制、閾值判斷、I/O 腳位特性（輸入專用/可輸出）。
* **安全**：MQ-2 測試僅做趨勢演示，保持通風，嚴禁明火。
* **擴充**：可加入移動平均或中位數濾波、把讀值透過序列繪圖或藍牙/網頁顯示。
* **常見錯誤**：Neopixel 不亮多半是**未共地**、腳位設錯或忘記 `pixels.show()`；MQ-2 未分壓會燒 ADC；LDR 接錯導致閾值方向反了。

