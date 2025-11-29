编译器提示  
```
BluetoothA2DPSink.h: No such file or directory
```  
说明 **IDE 根本没有找到 ESP32-A2DP 库**，而不是代码写错。  
把库正确装进去即可解决，步骤如下（任选一种，推荐 ZIP 法最省事）：

------------------------------------------------
一、1 分钟安装法（ZIP，最简）
1. 打开库发布页  
   https://github.com/pschatzmann/ESP32-A2DP  
   → 点击绿色「**Code ▼**」→「**Download ZIP**」
2. 回到 Arduino-IDE  
   菜单 **Sketch → Include Library → Add .ZIP Library…**  
   选中刚才下载的 `ESP32-A2DP-main.zip`，点“打开”
3. 安装完以后，  
   **File → Examples** 里会多出一组 **ESP32-A2DP** 示例，  
   再编译你的代码，上述 `No such file` 错误消失 

------------------------------------------------
二、Git 克隆法（适合以后随时更新）
```bash
cd %USERPROFILE%\Documents\Arduino\libraries
git clone https://github.com/pschatzmann/ESP32-A2DP.git
git clone https://github.com/pschatzmann/arduino-audio-tools.git   ; 多数示例同时需要 AudioTools
```
重启 IDE 即可 

------------------------------------------------
三、装完后最简可用代码（内部 DAC 输出）
```cpp
#include "BluetoothA2DPSink.h"

BluetoothA2DPSink a2dp_sink;

void setup() {
    static const i2s_config_t i2s = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
        .sample_rate = 44100,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S_MSB,
        .intr_alloc_flags = 0,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false
    };
    a2dp_sink.set_i2s_config(i2s);
    a2dp_sink.start("MyMusic");   // 手机搜这个名配对
}
void loop() {}
```
音频默认从 **GPIO25（左）/GPIO26（右）** 输出，接功放就能放音 

------------------------------------------------
四、如果以后碰到 “multiple libraries” 警告
把 `C:\Program Files\WindowsApps\ArduinoLLC.*\libraries\` 里的旧 SD/WiFi 库文件夹改名成 `xxx.old` 即可，让 IDE 只使用 esp32 核心自带的那份。

装完库再编译，问题即可消失；若仍有报错，把编译窗口第一条红色 error 贴出来继续排查。
