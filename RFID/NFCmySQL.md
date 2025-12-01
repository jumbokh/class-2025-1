下面给你一份**可直接运行的 Python 示例脚本**：读取 ACR122U 上的 **MIFARE 卡 UID**（APDU：`FF CA 00 00 00`），并写入 **MySQL**。

---

## 1) MySQL 表结构（先执行一次）

```sql
CREATE DATABASE IF NOT EXISTS nfc CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
USE nfc;

CREATE TABLE IF NOT EXISTS mifare_uid_log (
  id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
  uid_hex VARCHAR(32) NOT NULL,
  first_seen TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  last_seen  TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  seen_count BIGINT UNSIGNED NOT NULL DEFAULT 1,
  PRIMARY KEY (id),
  UNIQUE KEY uk_uid (uid_hex)
);
```

> 这个设计会对同一张卡：第一次刷卡插入；后续刷卡只更新 `last_seen/seen_count`（不重复插入多行）。

---

## 2) 安装依赖

```bash
pip install pyscard mysql-connector-python
```

> Windows 上 `pyscard` 若安装失败，通常是缺少编译环境/PCSC 组件；确保已装好 ACR122U 驱动并能在“智能卡读卡器”里看到设备。

---

## 3) Python 脚本：读 UID 并存 MySQL（保存为 `acr122u_uid_to_mysql.py`）

```python
import time
from datetime import datetime
from typing import Optional

import mysql.connector
from mysql.connector import Error as MySQLError

from smartcard.System import readers
from smartcard.Exceptions import NoCardException, CardConnectionException


# ========= 配置区：改成你的 MySQL 连接信息 =========
DB_CONFIG = {
    "host": "127.0.0.1",
    "user": "root",
    "password": "YOUR_PASSWORD",
    "database": "nfc",
    "port": 3306,
}
POLL_INTERVAL_SEC = 0.3          # 轮询间隔
DEBOUNCE_SEC = 1.5               # 同一张卡放在读卡器上时，去抖（避免狂写数据库）


APDU_GET_UID = [0xFF, 0xCA, 0x00, 0x00, 0x00]  # ACR122U 常用指令：获取 UID


def ensure_table(conn) -> None:
    ddl = """
    CREATE TABLE IF NOT EXISTS mifare_uid_log (
      id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
      uid_hex VARCHAR(32) NOT NULL,
      first_seen TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
      last_seen  TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
      seen_count BIGINT UNSIGNED NOT NULL DEFAULT 1,
      PRIMARY KEY (id),
      UNIQUE KEY uk_uid (uid_hex)
    );
    """
    with conn.cursor() as cur:
        cur.execute(ddl)
    conn.commit()


def pick_reader(prefer_keyword: str = "ACR122") -> str:
    rlist = readers()
    if not rlist:
        raise RuntimeError("未检测到任何 PC/SC 读卡器。请确认 ACR122U 驱动/服务正常。")

    # 优先选名字含 ACR122 的
    for r in rlist:
        if prefer_keyword.lower() in str(r).lower():
            return str(r)

    # 否则选第一个
    return str(rlist[0])


def connect_reader(reader_name: str):
    rlist = readers()
    reader_obj = None
    for r in rlist:
        if str(r) == reader_name:
            reader_obj = r
            break
    if reader_obj is None:
        raise RuntimeError(f"找不到读卡器：{reader_name}")

    connection = reader_obj.createConnection()
    return connection


def read_uid_hex(card_connection) -> Optional[str]:
    """
    成功返回 UID 的十六进制字符串（大写，无空格），如 '04A1B2C3D4'；
    如果当前无卡，返回 None。
    """
    try:
        card_connection.connect()
    except NoCardException:
        return None
    except CardConnectionException:
        return None

    data, sw1, sw2 = card_connection.transmit(APDU_GET_UID)
    if (sw1, sw2) != (0x90, 0x00) or not data:
        # 不是成功状态或没返回数据
        return None

    return "".join(f"{b:02X}" for b in data)


def upsert_uid(conn, uid_hex: str) -> None:
    """
    若 uid 不存在：插入；
    若已存在：seen_count+1，并更新 last_seen。
    """
    sql = """
    INSERT INTO mifare_uid_log (uid_hex, first_seen, last_seen, seen_count)
    VALUES (%s, NOW(), NOW(), 1)
    ON DUPLICATE KEY UPDATE
      last_seen = NOW(),
      seen_count = seen_count + 1;
    """
    with conn.cursor() as cur:
        cur.execute(sql, (uid_hex,))
    conn.commit()


def main():
    # 连接 MySQL
    try:
        db = mysql.connector.connect(**DB_CONFIG)
    except MySQLError as e:
        raise SystemExit(f"MySQL 连接失败：{e}")

    ensure_table(db)

    # 选择读卡器
    reader_name = pick_reader()
    print(f"[{datetime.now().isoformat(sep=' ', timespec='seconds')}] 使用读卡器：{reader_name}")

    card_conn = connect_reader(reader_name)

    last_uid = None
    last_time = 0.0

    print("开始监听刷卡（Ctrl+C 退出）...")
    try:
        while True:
            uid = read_uid_hex(card_conn)
            now = time.time()

            if uid:
                # 去抖：同一张卡保持在感应区时不重复写
                if uid != last_uid or (now - last_time) > DEBOUNCE_SEC:
                    upsert_uid(db, uid)
                    print(f"[{datetime.now().isoformat(sep=' ', timespec='seconds')}] UID={uid} 已写入/更新数据库")
                    last_uid = uid
                    last_time = now

            time.sleep(POLL_INTERVAL_SEC)

    except KeyboardInterrupt:
        print("\n退出。")
    finally:
        try:
            db.close()
        except Exception:
            pass


if __name__ == "__main__":
    main()
```

---

## 4) 验证：看数据库有没有记录

```sql
USE nfc;
SELECT * FROM mifare_uid_log ORDER BY last_seen DESC LIMIT 20;
```

---

如果你希望改成“**每次刷卡都插入一条日志**（不做去重/累计）”，我也可以给你另一版表结构与脚本（带 `event_time`、`reader_name`、`ip` 等字段）。
