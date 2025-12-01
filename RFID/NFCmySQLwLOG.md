下面把方案改成：**每次刷卡都插入一条日志**（不去重、不累计）。

---

## 1) MySQL 表结构（事件日志表）

```sql
CREATE DATABASE IF NOT EXISTS nfc CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
USE nfc;

CREATE TABLE IF NOT EXISTS mifare_uid_events (
  id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
  uid_hex VARCHAR(32) NOT NULL,
  reader_name VARCHAR(128) NULL,
  event_time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (id),
  KEY idx_event_time (event_time),
  KEY idx_uid_hex (uid_hex)
);
```

---

## 2) Python 脚本（每次刷卡插入一行）

保存为 `acr122u_uid_eventlog.py`：

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

POLL_INTERVAL_SEC = 0.15   # 轮询间隔（越小越灵敏）
DEBOUNCE_SEC = 0.8         # 防止同一张卡停在感应区导致多次插入（可设为 0 彻底不防抖）

APDU_GET_UID = [0xFF, 0xCA, 0x00, 0x00, 0x00]  # 获取 UID


def ensure_table(conn) -> None:
    ddl = """
    CREATE TABLE IF NOT EXISTS mifare_uid_events (
      id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
      uid_hex VARCHAR(32) NOT NULL,
      reader_name VARCHAR(128) NULL,
      event_time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
      PRIMARY KEY (id),
      KEY idx_event_time (event_time),
      KEY idx_uid_hex (uid_hex)
    );
    """
    with conn.cursor() as cur:
        cur.execute(ddl)
    conn.commit()


def pick_reader(prefer_keyword: str = "ACR122") -> str:
    rlist = readers()
    if not rlist:
        raise RuntimeError("未检测到任何 PC/SC 读卡器。请确认 ACR122U 驱动/服务正常。")
    for r in rlist:
        if prefer_keyword.lower() in str(r).lower():
            return str(r)
    return str(rlist[0])


def connect_reader(reader_name: str):
    for r in readers():
        if str(r) == reader_name:
            return r.createConnection()
    raise RuntimeError(f"找不到读卡器：{reader_name}")


def read_uid_hex(card_connection) -> Optional[str]:
    try:
        card_connection.connect()
    except (NoCardException, CardConnectionException):
        return None

    data, sw1, sw2 = card_connection.transmit(APDU_GET_UID)
    if (sw1, sw2) != (0x90, 0x00) or not data:
        return None

    return "".join(f"{b:02X}" for b in data)


def insert_event(conn, uid_hex: str, reader_name: str) -> None:
    sql = """
    INSERT INTO mifare_uid_events (uid_hex, reader_name, event_time)
    VALUES (%s, %s, NOW());
    """
    with conn.cursor() as cur:
        cur.execute(sql, (uid_hex, reader_name))
    conn.commit()


def main():
    # MySQL
    try:
        db = mysql.connector.connect(**DB_CONFIG)
    except MySQLError as e:
        raise SystemExit(f"MySQL 连接失败：{e}")

    ensure_table(db)

    # Reader
    reader_name = pick_reader()
    print(f"[{datetime.now().isoformat(sep=' ', timespec='seconds')}] 使用读卡器：{reader_name}")
    card_conn = connect_reader(reader_name)

    last_uid = None
    last_insert_ts = 0.0

    print("开始监听刷卡（Ctrl+C 退出）...")
    try:
        while True:
            uid = read_uid_hex(card_conn)
            now = time.time()

            if uid:
                # 说明：你要求“不做去重/累计”，但实际场景里卡停在读卡器上会被轮询到很多次。
                # 这里提供一个「可选防抖」：默认 0.8 秒内同 UID 只记一次，避免刷屏与狂写库。
                # 若你真的希望停留也疯狂插入，把 DEBOUNCE_SEC 设为 0。
                if DEBOUNCE_SEC <= 0 or uid != last_uid or (now - last_insert_ts) > DEBOUNCE_SEC:
                    insert_event(db, uid, reader_name)
                    print(f"[{datetime.now().isoformat(sep=' ', timespec='seconds')}] INSERT UID={uid}")
                    last_uid = uid
                    last_insert_ts = now

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

## 3) 查询最近 20 条刷卡记录

```sql
USE nfc;
SELECT id, uid_hex, reader_name, event_time
FROM mifare_uid_events
ORDER BY event_time DESC
LIMIT 20;
```

> 小提示：你说“不做去重”，我仍留了一个 **可关掉的防抖**（默认 0.8s），因为 ACR122U 轮询会导致卡放着不动也不断触发读取；如果你确实想“放着也一直插入”，把 `DEBOUNCE_SEC = 0` 即可。
