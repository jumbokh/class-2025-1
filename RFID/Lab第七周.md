# 第 7 週｜資產出借＋盤點（借還流程＋缺漏統計）

> 專案位址：`week7_lab_pack/`（已提供 zip）。本手冊為學生版一步一步操作指引。

## 0) 本週成果（驗收標準）

* 借出 `/loan/checkout`、歸還 `/loan/return` 可用，`op_no` 去重（重送不重複入帳）。
* 資產狀態（IN/OUT）正確切換；逾期清單 `/report/overdue` 可查。
* 盤點全流程可跑通：`/inventory/start → /inventory/scan → /inventory/summary → /inventory/end`。
* 盤點缺漏計算：**missing = 資產狀態為 IN 但本次未掃到**；外借中的資產列於 **out**。

---

## 1) 安裝與準備

1. 建議使用虛擬環境並安裝依賴：

```bash
pip install -r requirements.txt
```

2. 建資料庫表與測試資料：

```bash
mysql -u root -p < db/schema.sql
mysql -u root -p < db/seed.sql
```

> 說明：本週使用 `rfid_iot` 資料庫。借用人以第 5 週建立的 `users` 表為準（`users.uid` 作為 borrower_uid）。

3. 複製環境變數檔：

```bash
cp .env.sample .env
```

確認 `PORT=8002`、`SERVER_URL=http://127.0.0.1:8002`。

---

## 2) 啟動後端服務

```bash
python server/app_assets.py
# 服務預設 http://127.0.0.1:8002
```

---

## 3) 借出／歸還（Console 範例）

### 3.1 借出（Checkout）

```bash
python client/checkout_console.py
# 依提示輸入：
# borrower_uid（例如 users.uid: 04aa11bb）
# tag_uid（例如 seed 中的 A100aa01）
# 借用天數（預設 7）
```

成功後 API 會回傳 due_ts、op_no 等資訊，資料表 `loans` 會新增一筆，`assets.status` 會轉為 `OUT`。

### 3.2 歸還（Return）

```bash
python client/return_console.py
# 輸入：
# tag_uid（同上）
# condition_in（可空白，例如：OK/輕微刮傷）
```

成功後 `loans.return_ts` 會補上時間戳，`assets.status` 會回到 `IN`。

### 3.3 狀態查詢與逾期清單

```bash
# 查資產當前狀態
curl "http://127.0.0.1:8002/loan/status?tag_uid=A100aa01"

# 查逾期（return_ts 為 NULL 且 due_ts < now）
curl "http://127.0.0.1:8002/report/overdue"
```

---

## 4) 盤點流程（Inventory）

### 4.1 開始盤點（建立 Session）

```bash
python client/inv_start.py
# 輸入：盤點地點（e.g., Lab-A）
# 取得回傳的 session_id
```

### 4.2 掃描資產（模擬 或 ACR122U 讀卡）

* **模擬掃描（建議先用）**

  ```bash
  python client/inv_scan_sim.py
  # 輸入上一步取得的 session_id
  # 程式會每 2 秒隨機送一筆 tag_uid
  ```
* **實機讀卡（ACR122U）**

  ```bash
  python reader/inv_scan_reader.py
  # 輸入 session_id，持續感應資產標籤
  ```

### 4.3 查盤點摘要

```bash
curl "http://127.0.0.1:8002/inventory/summary?session_id=<你的SID>"
# 回傳 present/missing/out 清單與計數
```

### 4.4 結束盤點

```bash
curl -X POST "http://127.0.0.1:8002/inventory/end" -H "Content-Type: application/json" \
  -d '{"session_id": <你的SID> }'
```

---

## 5) 標準測試流程（建議）

1. 先借出一件資產（例如 `A100aa01`），使其 `status=OUT`。
2. 啟動盤點 Session，掃描其餘 **IN** 資產直到 `missing=0`（外借中的應出現在 `out`）。
3. 把外借資產歸還，再次查 `inventory/summary`（或開新一輪 Session）確認狀態回到 **IN**。
4. 透過將借用天數設為 0（或手動改 `due_ts`）測試 `/report/overdue` 是否列出逾期。
5. 重送同一 `op_no`（借出或歸還）應回 `duplicate-op`，驗證「去重」生效。

---

## 6) API 範例（直接用 curl 測）

```bash
# 借出
curl -X POST http://127.0.0.1:8002/loan/checkout -H "Content-Type: application/json" \
  -d '{"tag_uid":"A100aa01","borrower_uid":"04aa11bb","due_days":7,"op_no":1730612345}'

# 歸還
curl -X POST http://127.0.0.1:8002/loan/return -H "Content-Type: application/json" \
  -d '{"tag_uid":"A100aa01","condition_in":"OK","op_no":1730619999}'

# 盤點：start → scan → summary → end
curl -X POST http://127.0.0.1:8002/inventory/start -H "Content-Type: application/json" -d '{"location":"Lab-A"}'
# 假設回 {"session_id": 12}
curl -X POST http://127.0.0.1:8002/inventory/scan -H "Content-Type: application/json" \
  -d '{"session_id":12,"tag_uid":"A100aa02","reader_id":"simR"}'
curl "http://127.0.0.1:8002/inventory/summary?session_id=12"
curl -X POST http://127.0.0.1:8002/inventory/end -H "Content-Type: application/json" -d '{"session_id":12}'
```

---

## 7) 交付清單（作業繳交）

* 借出／歸還流程截圖（含資料庫或 API 回應）。
* 一次完整盤點（`present/missing/out`）摘要截圖。
* 逾期清單截圖與說明（如何觸發、如何清理）。
* README（啟動步驟）＋ 30 秒 Demo 影片（可選）。

---

## 8) 常見問題（快速排查）

* **`duplicate-op`**：同一 `op_no` 已被用過（去重保護），改用新 `op_no` 再送。
* **`already-out`**：資產尚未歸還就嘗試再次借出。
* **`no-open-loan`**：沒有開放中的借出紀錄卻送了歸還。
* **查不到 borrower**：請先確定第 5 週的 `users` 表已存在且 `uid` 正確。
* **時區／時間戳**：所有時間以 Unix timestamp（秒）計，請注意 due_ts 與當前時間的比較。

---
