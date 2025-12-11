

## 一、学生版试卷（Word/PDF 文案）

> 建议：这一整段直接贴进一个新的 Word 文件，调整学校抬头、页眉页脚、字号即可。

---

### 福州外语外贸学院 大数据学院

《物联网技术与应用》期末实验测验（学生版）

课程名称：_________________________
授课教师：_________________________
考试日期：________年____月____日
考试时间：****：**** ～ ****：****（共 ______ 分钟）

姓名：_________________
学号：_________________
班级：_________________

总分：________ / 100（加分题最多 +20）

---

### 一、考试说明

1. 本次为**上机实验期末测验**，每位同学配发一张 **MIFARE Classic 1K 卡**。
2. 读卡设备：**ACR122U** 或等效 NFC 读卡器。
3. 使用语言：建议 **Python + ACR122U 函式库**，也可使用 C / C++ / 其他老师允许的语言，但须现场演示程式执行。
4. 不得擅自修改：

   * 卡片的 UID；
   * Sector 0（绝对 Block 0–3）；
   * 非题目要求的区块内容。
5. 考试中不得互相交换卡片、程式码，不得上网抄代码，一经发现按作弊处理。
6. 完成各小题后，请保持程式窗口与读卡器连接状态，等待老师逐一验收并记录成绩。

---

### 二、卡片区块规划（统一规定）

本实验以 **MIFARE Classic 1K** 为基础，绝对区块编号为 **Block 0–63**。统一规定如下：

| 功能说明                  | Sector / Block       | 绝对 Block | 说明                  |
| --------------------- | -------------------- | -------- | ------------------- |
| UID / 厂商资料（不得修改）      | Sector 0 / Block 0–3 | 0–3      | 不动                  |
| **位置资讯 + 卡片编号区块**     | Sector 1 / Block 0   | **4**    | 加分题位置资讯 + 卡片编号 C    |
| **学号存放区块**            | Sector 1 / Block 1   | **5**    | 学号（ASCII）           |
| **运算结果记录区块**          | Sector 1 / Block 2   | **6**    | C、S、R 等（ASCII）      |
| **钱包区块（Value Block）** | Sector 1 / Block 3   | **7**    | MIFARE Value 格式余额 R |
| 长文字区块（学生自选）           | 任意连续若干数据区块           | 例如 10–12 | 用于加分题               |

**重要约定：**

* 本题统一使用 **Key A = FF FF FF FF FF FF**。
* 老师已经在 **Block 4 的 Byte 5–6** 写入本卡 **两位数字卡片编号 C**（例如 `"37"`），使用 ASCII 编码。
* 若你完成加分题，会在 Block 4 的 Byte 0–4 写入位置资讯（见加分题说明）。

---

### 三、题目与要求

**记号说明：**

* `C`：老师预先写在卡上的 **两位数卡片编号**（00–99）
* `S`：你自己学号的**末两位数字**（00–99）
* `R`：计算结果，定义为

[
R = C \times 100 + S
]

---

#### 【第 1 题】写入学号（20 分）

1. 写一个程式，连接读卡器并读取本卡的 UID（仅显示即可）。
2. 将你的「完整学号」以 **ASCII 字串**形式写入：

   * Sector 1 / Block 1（绝对 Block 5）

**格式要求：**

* 学号使用 ASCII 编码逐字写入；
* 若长度不足 16 字节，后面以 `0x00` 填满；
* 读出时能正确显示完整学号。

> 老师验收时会要求你运行程式，读出 Block 5 并显示学号。

---

#### 【第 2 题】運算並記錄結果（30 分）

1. 从 **Sector 1 / Block 0（绝对 Block 4）** 读取卡片编号 `C`：

   * 卡号以 **两位十进制 ASCII** 存在 **Byte 5–6**（例如 `"37"`）。
2. 从你的学号中取**最后两位数字**，记为 `S`（例如学号末两位为 `"45"`，则 S=45）。
3. 根据公式计算：

[
R = C \times 100 + S
]

4. 将 `C`、`S`、`R` 的结果以 ASCII 字串形式写入：

   * Sector 1 / Block 2（绝对 Block 6）

**规定格式：**

```text
CC-SS-RRRR
```

* `CC`：卡片编号，两位十进制，前导 0；
* `SS`：学号末两位，两位十进制，前导 0；
* `RRRR`：结果 R，不足 4 位左侧补 0；
* 中间使用英文连字号 `-` 分隔；
* 例：C=37，S=45，R=3745，则写入 `"37-45-3745"`；
* 不足 16 字节时，后面用 `0x00` 填满。

---

#### 【第 3 题】初始化钱包 Value Block（30 分）

1. 使用你在第 2 题算出的 `R` 作为钱包初始余额。
2. 在：

   * Sector 1 / Block 3（绝对 Block 7）

以 **MIFARE Value Block 格式** 写入一个 Value Block，初始化「余额 = R」。

**要求：**

* 使用 Value Block 写入相关指令，或在程式中正确组装 Value Block 结构；
* 初始化后，应能再次读取该 Value Block，并从中解析出数值 R；
* **加 5 分（选做）：** 在程式中示范一次「充值 +10」或「扣款 -10」操作，并显示余额变化。

---

#### 【加分题】跨区块长文字存取与位置标记（最多 20 分）

老师提供一段**超过 16 字节**的文字（英文或中文），例如：

> This is a long message for MIFARE final exam.

你的任务：

1. 自行选择 **连续的若干个数据区块**（例如绝对 Block 10–12），将这段文字完整写入这些区块（可以跨多个区块）。

2. 在 **Sector 1 / Block 0（绝对 Block 4）** 写入「位置资讯」，包含：

   * 起始绝对区块编号；
   * 使用区块数量 N；
   * 文字总长度（字节数）。

3. 程式须能：

   * 从 Block 4 读出位置资讯；
   * 根据位置资讯，从对应区块连续读出内容；
   * 截取前 `长度` 个字节，重新组合为原始字串并显示。

**建议但非强制的编码格式：**

Block 4 的前 5 个 Byte：

* Byte 0：固定标记值 `0xA5`（表示有合法位置资讯）
* Byte 1：起始绝对 Block 编号（0–63）
* Byte 2：使用 Block 数量 N
* Byte 3：文字总长度低 8 位
* Byte 4：文字总长度高 8 位

卡片编号 `C` 保存在：

* Byte 5–6：卡号 ASCII（例如 `"37"`）
* Byte 7–15：预留（可填 0x00）

---

> 试卷到此结束，请确认程式可重复执行，并等待老师验收。

---

## 二、教师版讲义（含参考架构）

> 建议：这一段放到另一份 Word 里，当成「教师用资料」。

---

### 《MIFARE 智慧卡期末实验测验 – 教师版讲义》

#### 1. 考试目标

考察学生是否掌握：

1. 使用 ACR122U / PC/SC 读取 MIFARE 卡 UID 与数据区块；
2. 了解 MIFARE Classic 1K 的区块结构、扇区与权限；
3. 能在指定区块写入 ASCII 数据，并进行简单数值运算；
4. 能初始化并读取 MIFARE Value Block（钱包）；
5. 能设计简单的「跨区块长消息 + 位置标记」机制（加分题）。

---

#### 2. 预设约定与区块内容

* 使用卡片：MIFARE Classic 1K，绝对 Block 0–63。
* 统一使用 **Key A = FF FF FF FF FF FF**，Key B 不用。
* 区块分配：

  * Block 0–3：保留（UID / 厂商数据），不改。
  * Block 4：位置资讯与卡片编号区块。
  * Block 5：学号（ASCII + 0x00 填充）。
  * Block 6：C / S / R 运算结果（ASCII）
  * Block 7：钱包 Value Block（余额 = R）
  * 其余 Block（如 10–12）可开放给学生加分题使用。

**Block 4 格式建议（考试用）：**

| Byte | 内容                   |
| ---- | -------------------- |
| 0    | 位置资讯标记 `0xA5` 或 0x00 |
| 1    | 起始绝对 Block（加分题）      |
| 2    | 使用 Block 数量 N        |
| 3    | 长度低位                 |
| 4    | 长度高位                 |
| 5–6  | 卡片编号 C 的 ASCII（两位）   |
| 7–15 | 预留 = 0x00            |

> 考试前你可以只写 Byte 5–6 为卡号，Byte 0–4 和 7–15 全部设为 0x00。
> 学生若完成加分题，会把 Byte 0–4 写成位置资讯；Byte 5–6 的卡号不变。

---

#### 3. 评分建议（可按需要微调）

| 项目                               | 分数 |
| -------------------------------- | -- |
| 连接读卡器并正确读取 UID                   | 10 |
| Block 5 写入完整学号（ASCII + 填充正确）     | 20 |
| 读取 C、取学号末两位 S，正确计算 R             | 10 |
| Block 6 按格式写入 `CC-SS-RRRR`       | 20 |
| Block 7 初始化 Value Block = R，并可读回 | 20 |
| 实现一次充值/扣款并显示余额变化（可并入上题）          | 10 |
| 加分：跨区块存放长文字，并能读回重组               | 10 |
| 加分：Block 4 位置资讯编码/解析完全正确         | 10 |

---

#### 4. 参考程式架构要点（Python + pyscard）

* 使用 `smartcard.System.readers()` 找到 ACR122U 读卡器。

* UID 读取 APDU：`FF CA 00 00 00`。

* 载入密钥（Key A）到槽 0：

  ```text
  FF 82 00 00 06 FF FF FF FF FF FF
  ```

* 扇区认证（Key A）：

  ```text
  FF 86 00 00 05  01 00 <BLOCK> 60 00
  ```

* 读区块 APDU：`FF B0 00 <BLOCK> 10`。

* 写区块 APDU：`FF D6 00 <BLOCK> 10 <16 bytes>`。

Value Block：

* Value = 4 Bytes（小端），反码、重复值、Block 地址与反码，典型布局：

  ```
  value(4) | ~value(4) | value(4) | addr | ~addr | addr | ~addr
  ```

长文字加分题：

* Block 4 的位置资讯给出：起始 Block、Block 数、总长度；
* 从起始 Block 起连续读取 N 个 Block，拼接后截取前 Length 个 byte，按 UTF-8 解码。

---

## 三、「批改小工具」Python 脚本

> 说明：
>
> * 使用 `pyscard` 与 ACR122U；
> * 每次放一张卡，按 Enter 即可自动检查：
>
>   * UID
>   * 学号 Block 5
>   * C / S / R Block 6 与公式是否正确
>   * 钱包 Value Block 是否 = R
>   * 如有位置资讯 (Byte0 = 0xA5)，会尝试重组长文字与预设答案比对。
> * 你可以把「预设长文字」改成你考试时公布的那一句。

安装依赖（如未安装）：

```bash
pip install pyscard
```

保存下列脚本为：`grade_mifare_exam.py`：

```python
from smartcard.System import readers
from smartcard.Exceptions import NoCardException, CardConnectionException

# ---------- 配置区：依需求修改 ----------

KEY_A_HEX = "FFFFFFFFFFFF"          # Key A
BLOCK_CARD_INFO = 4
BLOCK_STUDENT_ID = 5
BLOCK_RESULT = 6
BLOCK_WALLET = 7

# 如果有加分题，设定你给学生的那串长文字：
EXPECTED_LONG_MESSAGE = "This is a long message for MIFARE final exam."

# --------------------------------------


def pick_reader():
    rlist = readers()
    if not rlist:
        raise RuntimeError("未找到任何 PC/SC 读卡器，请检查 ACR122U 是否连接与驱动是否安装。")
    for r in rlist:
        if "ACR122" in str(r):
            return r
    return rlist[0]


def connect_card(reader):
    conn = reader.createConnection()
    conn.connect()
    return conn


def transmit(conn, apdu):
    data, sw1, sw2 = conn.transmit(apdu)
    return bytes(data), sw1, sw2


def load_key(conn, key_hex=KEY_A_HEX, key_no=0x00):
    key_bytes = bytes.fromhex(key_hex)
    apdu = [0xFF, 0x82, 0x00, key_no, 0x06] + list(key_bytes)
    data, sw1, sw2 = transmit(conn, apdu)
    if (sw1, sw2) != (0x90, 0x00):
        raise RuntimeError(f"載入 Key 失敗: SW={sw1:02X}{sw2:02X}")


def mifare_auth(conn, block_no: int, key_no=0x00, key_type="A"):
    key_mode = 0x60 if key_type.upper() == "A" else 0x61
    apdu = [
        0xFF, 0x86, 0x00, 0x00, 0x05,
        0x01, 0x00, block_no, key_mode, key_no
    ]
    data, sw1, sw2 = transmit(conn, apdu)
    if (sw1, sw2) != (0x90, 0x00):
        raise RuntimeError(f"區塊 {block_no} 認證失敗: SW={sw1:02X}{sw2:02X}")


def read_block(conn, block_no: int) -> bytes:
    mifare_auth(conn, block_no)
    data, sw1, sw2 = transmit(conn, [0xFF, 0xB0, 0x00, block_no, 16])
    if (sw1, sw2) != (0x90, 0x00):
        raise RuntimeError(f"讀取區塊 {block_no} 失敗: SW={sw1:02X}{sw2:02X}")
    return data


def read_uid(conn) -> str:
    data, sw1, sw2 = transmit(conn, [0xFF, 0xCA, 0x00, 0x00, 0x00])
    if (sw1, sw2) != (0x90, 0x00):
        raise RuntimeError("讀取 UID 失敗")
    return "".join(f"{b:02X}" for b in data)


def parse_card_number_from_block4(b4: bytes) -> int:
    """
    Block 4:
    Byte 5-6 = ASCII 卡號，例如 b'37'
    """
    try:
        c_str = b4[5:7].decode("ascii")
        c = int(c_str)
        if not (0 <= c <= 99):
            raise ValueError
        return c
    except Exception:
        raise RuntimeError(f"Block4 中卡片編號格式錯誤，byte[5:7]={b4[5:7]!r}")


def parse_student_id(b5: bytes) -> str:
    # 擷取直到第一個 0x00
    if 0x00 in b5:
        idx = b5.index(0x00)
        sid_bytes = b5[:idx]
    else:
        sid_bytes = b5
    try:
        sid = sid_bytes.decode("ascii")
    except UnicodeDecodeError:
        raise RuntimeError(f"學號不是 ASCII：{sid_bytes!r}")
    return sid.strip()


def parse_result_block(b6: bytes):
    """
    格式: b"CC-SS-RRRR" + 0x00 填充
    """
    if 0x00 in b6:
        idx = b6.index(0x00)
        text = b6[:idx].decode("ascii", errors="replace")
    else:
        text = b6.decode("ascii", errors="replace")
    parts = text.split("-")
    if len(parts) != 3:
        raise RuntimeError(f"結果區塊格式錯誤: {text!r}")
    try:
        C = int(parts[0])
        S = int(parts[1])
        R = int(parts[2])
    except ValueError:
        raise RuntimeError(f"結果區塊數字解析失敗: {text!r}")
    return C, S, R, text


def decode_value_block(b7: bytes, expected_block_addr: int = BLOCK_WALLET) -> int:
    if len(b7) != 16:
        raise RuntimeError("Value Block 長度不是 16 bytes")

    v1 = int.from_bytes(b7[0:4], "little")
    v1n = b7[4:8]
    v2 = int.from_bytes(b7[8:12], "little")
    addr_bytes = b7[12:16]

    # 檢查 value / ~value
    if any((b ^ n) != 0xFF for b, n in zip(b7[0:4], v1n)):
        raise RuntimeError("Value Block 反碼檢查失敗")
    if v1 != v2:
        raise RuntimeError("Value Block 前後 value 不一致")

    # 檢查地址字節
    addr = addr_bytes[0]
    if addr != expected_block_addr:
        # 不一定要嚴格，這裡僅提示
        print(f"警告：Value Block 地址={addr}，與預期 {expected_block_addr} 不同")

    return v1


def parse_long_message_meta(b4: bytes):
    """
    加分題位置資訊:
    Byte0: 0xA5 => 有資訊
    Byte1: start block
    Byte2: block count
    Byte3: length low
    Byte4: length high
    """
    if b4[0] != 0xA5:
        return None  # 無加分題
    start_block = b4[1]
    block_count = b4[2]
    length = b4[3] + (b4[4] << 8)
    return start_block, block_count, length


def read_long_message(conn, start_block: int, block_count: int, length: int) -> str:
    bufs = []
    for i in range(block_count):
        blk = start_block + i
        bufs.append(read_block(conn, blk))
    data = b"".join(bufs)
    data = data[:length]
    try:
        text = data.decode("utf-8")
    except UnicodeDecodeError:
        text = data.decode("utf-8", errors="replace")
    return text


def grade_card(conn):
    uid = read_uid(conn)
    print(f"UID: {uid}")

    # 讀主要區塊
    b4 = read_block(conn, BLOCK_CARD_INFO)
    b5 = read_block(conn, BLOCK_STUDENT_ID)
    b6 = read_block(conn, BLOCK_RESULT)
    b7 = read_block(conn, BLOCK_WALLET)

    # 卡號 C
    C_from_b4 = parse_card_number_from_block4(b4)

    # 學號 / S
    sid = parse_student_id(b5)
    if len(sid) < 2 or not sid[-2:].isdigit():
        raise RuntimeError(f"學號末兩位不是數字: {sid}")
    S_expected = int(sid[-2:])

    # Block 6 的 C,S,R
    C_res, S_res, R_res, text_res = parse_result_block(b6)

    # 理論 R
    R_expected = C_from_b4 * 100 + S_expected

    # Value Block
    R_wallet = decode_value_block(b7, expected_block_addr=BLOCK_WALLET)

    print("---- 解析結果 ----")
    print(f"學號: {sid}")
    print(f"卡號 C (Block4 Byte5-6): {C_from_b4}")
    print(f"S (學號末兩位): {S_expected}")
    print(f"Block6 內容: {text_res!r} -> C={C_res}, S={S_res}, R={R_res}")
    print(f"理論 R = C*100+S = {R_expected}")
    print(f"錢包 Value Block 值: {R_wallet}")

    ok_C = (C_res == C_from_b4)
    ok_S = (S_res == S_expected)
    ok_R = (R_res == R_expected)
    ok_wallet = (R_wallet == R_expected)

    print("\n--- 判定 ---")
    print(f"學號寫入 Block5：{'OK' if sid else '??'}")
    print(f"卡號 C 一致：{'OK' if ok_C else '錯誤'}")
    print(f"S (末兩位) 一致：{'OK' if ok_S else '錯誤'}")
    print(f"R 計算正確：{'OK' if ok_R else '錯誤'}")
    print(f"錢包值 = R：{'OK' if ok_wallet else '錯誤'}")

    # 加分題檢查
    meta = parse_long_message_meta(b4)
    if meta is None:
        print("\n加分題：未偵測到位置資訊 (Block4 Byte0 != 0xA5)")
    else:
        start_block, block_count, length = meta
        print(f"\n加分題位置資訊：start={start_block}, count={block_count}, length={length}")
        text = read_long_message(conn, start_block, block_count, length)
        print(f"重組長文字: {text!r}")
        if EXPECTED_LONG_MESSAGE and text == EXPECTED_LONG_MESSAGE:
            print("長文字與預期完全相符：加分 OK")
        else:
            print("長文字與預期不符，請人工確認內容。")


def main():
    try:
        r = pick_reader()
    except RuntimeError as e:
        print(e)
        return

    print(f"使用讀卡器：{r}")
    print("提示：每次把卡放上讀卡器，按 Enter 開始批改；輸入 q + Enter 離開。")

    while True:
        cmd = input("\n按 Enter 讀取當前卡片，或輸入 q 離開：").strip().lower()
        if cmd == "q":
            print("結束批改程式。")
            break

        try:
            conn = connect_card(r)
        except NoCardException:
            print("未偵測到卡片，請先放上卡再試。")
            continue
        except CardConnectionException as e:
            print(f"連線卡片失敗：{e}")
            continue

        try:
            load_key(conn)
            grade_card(conn)
        except Exception as e:
            print(f"\n[錯誤] {e}")
        finally:
            try:
                conn.disconnect()
            except Exception:
                pass


if __name__ == "__main__":
    main()
```

