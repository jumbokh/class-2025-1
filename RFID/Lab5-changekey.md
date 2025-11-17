這裡是「實驗 5：金鑰基礎（僅限自有空白卡）」的**安全示範程式**：更改某扇區的 **Key A**，並立即用新 Key A 驗證；同時提供一鍵恢復為預設 `FFFFFFFFFFFF` 的選項。程式會保留原本的 **Access Bits（4 bytes）** 與 **Key B**，避免誤鎖卡。

[下載 exp5_change_key.py](sandbox:/mnt/data/exp5_change_key.py)

### 你會學到

* 如何定位 **Sector Trailer**（每扇區最後一塊）
* 如何**讀出並解析** Trailer（KeyA | Access | KeyB）
* 如何**只改 Key A** 並寫回，**不動** Access 與 Key B
* 如何用新 Key A **重新認證資料塊**驗證成功

### 使用說明

1. 安裝依賴（若未安裝）
   `py -m pip install pyscard`
2. 接好 ACR122U，執行並把卡放上讀卡機。

#### 更改扇區 3 的 Key A 為 `A0A1A2A3A4A5`

```bash
python -u exp5_change_key.py --sector 3 --new-keya A0A1A2A3A4A5
```

#### 還原扇區 3 的 Key A 為預設 `FFFFFFFFFFFF`

```bash
python -u exp5_change_key.py --sector 3 --restore
```

#### 如果該扇區目前必須用 Key B 才能存取（你知道 Key B）

```bash
python -u exp5_change_key.py --sector 3 --new-keya 112233445566 --keyb FFFFFFFFFFFF
```

### 安全提醒（務必遵守）

* 僅在**自有的空白測試卡**上進行此實驗；請勿用於門禁/支付等卡片。
* 本程式**只改 Key A**，保留 Access/Key B；但若卡原本就改過存取位，可能仍需要正確 Key B 才能改寫。
* 若出現 `WRITE FAIL SW=6982/6300` 或 `AUTH FAIL`，代表**金鑰不對或存取位限制**；請以正確 Key B 重試或先恢復存取位（需要更進階的實驗，若你要我可以補上「安全重置 Trailer（含備份）」版本）。

需要把這支程式包成 **zip + README + 教師版講義**嗎？我可以直接幫你生出來。
