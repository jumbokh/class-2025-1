---
noteId: "6e7606a0c36411f087cea9a14f0afef8"
tags: []

---

這張圖片是 **NFC 工具（TagInfo）** 讀取一張 **MIFARE Classic 1K** 空白卡後顯示的資訊。  
以下逐項解釋：

---

### ✅ **通用資訊（Generic Info）**
| 欄位 | 值 | 說明 |
|------|----|------|
| **UID** | `31 22 E5 3C` | 4-byte 唯一序號（UID），這張卡的「身份證號碼」。 |
| **RF Technology** | `ISO/IEC 14443 Type A` | 表示這是一張 **13.56 MHz** 的 **Type A** 非接觸式卡片。 |
| **ATQA** | `0004` | Answer To Request (Type A) － 告訴讀卡機「我是 Classic 1K」。 |
| **SAK** | `08` | Select Acknowledge － 二進制 `0000 1000`，bit3=1 代表 **MIFARE Classic**。 |
| **ATS** | （空白） | Classic 1K **沒有 ATS**；ATS 只有 CPU 卡或 DESFire 這類「正式」ISO14443-4 卡片才有。 |

---

### ✅ **Tag Type & Manufacturer**
| 欄位 | 值 | 說明 |
|------|----|------|
| **Tag Type** | **MIFARE Classic 1K – NXP** | 99 % 正確；NXP 原廠晶片。 |
| **Memory Size** | **1024 byte** | 1 KB = 16 個扇區 × 4 個區塊 × 16 byte。 |
| **Block Size** | **16 byte** | 每個區塊固定 16 byte。 |
| **Number of Sectors** | **16** | 扇區 0–15；每個扇區有 4 個區塊。 |
| **Number of Blocks** | **64** | 區塊 0–63；區塊 0 是製造商碼，區塊 3/7/11… 是控制塊（Key A + 存取位元 + Key B）。 |

---

### ✅ 一句話總結
這是一張 **NXP 原廠 MIFARE Classic 1K 空白卡**，UID=`31 22 E5 3C`，記憶體 1 KB，16 個扇區，64 個區塊，還沒寫入任何應用資料，所有金鑰都還是出廠預設值（通常 Key A = `FF FF FF FF FF FF`）。