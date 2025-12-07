[![ESP32-CAM AI-Thinker Pinout Guide: GPIOs Usage Explained | Random Nerd ...](https://tse2.mm.bing.net/th/id/OIP.By7Vtzw9IXqaZKxa6MP0VgHaDS?cb=ucfimg2\&pid=Api\&ucfimg=1)](https://randomnerdtutorials.com/esp32-cam-ai-thinker-pinout/?utm_source=chatgpt.com)

以下是 **INMP441 ↔ ESP32-CAM（常見 AI Thinker 版）** 的腳位對應表（Arduino/I2S 讀音常用接法）：

### 方案 A（最常用：BCLK=14、LRCLK=15、DOUT=13）

| INMP441 模組腳位 | 功能                          | ESP32-CAM 腳位                           |
| ------------ | --------------------------- | -------------------------------------- |
| VDD / 3V3    | 供電（1.8–3.3V）                | 3.3V ([DigiKey][1])                    |
| GND          | 地                           | GND ([DigiKey][1])                     |
| SCK / BCLK   | I2S 位元時鐘                    | GPIO14 ([docs.espressif.com][2])       |
| WS / LRCLK   | I2S 左右聲道切換                  | GPIO15 ([docs.espressif.com][2])       |
| SD (DOUT)    | I2S 數據輸出（麥克風→ESP32）         | GPIO13 ([docs.espressif.com][2])       |
| L/R          | 左/右聲道選擇（GND=Left、VDD=Right） | 接 GND（建議先用 Left） ([TDK InvenSense][3]) |

### 方案 B（替代：把 WS 改接 GPIO12）

| INMP441    | ESP32-CAM |                      |
| ---------- | --------- | -------------------- |
| SCK/BCLK → | GPIO14    |                      |
| WS/LRCLK → | GPIO12    |                      |
| SD(DOUT) → | GPIO13    | ([Instructables][4]) |

**重要提醒：**ESP32-CAM 的 microSD 會占用 **GPIO14/15/2/4/12/13**，所以你用上面任一方案接 INMP441 後，基本上就等於「不要用 SD 卡」；同理你把 LED 放在 GPIO2 也會跟 SD DATA0 衝突。([Random Nerd Tutorials][5])

[1]: https://www.digikey.com/htmldatasheets/production/1431884/0/0/1/inmp441-datasheet.html?utm_source=chatgpt.com "INMP441 Datasheet by TDK InvenSense"
[2]: https://docs.espressif.com/projects/esp-idf/en/stable/esp32h2/api-reference/peripherals/i2s.html?utm_source=chatgpt.com "Inter-IC Sound (I2S) - ESP32-H2"
[3]: https://invensense.tdk.com/wp-content/uploads/2015/02/INMP441.pdf?utm_source=chatgpt.com "INMP441"
[4]: https://www.instructables.com/Camera-and-Mic-Arduino-Experiment-With-TTGO-T-Came/?utm_source=chatgpt.com "Camera and Mic Arduino Experiment With TTGO T ..."
[5]: https://randomnerdtutorials.com/esp32-cam-ai-thinker-pinout/?utm_source=chatgpt.com "ESP32-CAM AI-Thinker Pinout Guide: GPIOs Usage ..."
