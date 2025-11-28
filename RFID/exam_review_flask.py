# exam_review_flask.py
# Pre-exam review: 3 experiments for ACR122U + MIFARE Classic via pyscard, served by Flask.

import sys, time, argparse, logging
from flask import Flask, request, render_template_string
from smartcard.System import readers
from smartcard.Exceptions import CardConnectionException
from smartcard.util import toHexString

GET_UID     = [0xFF, 0xCA, 0x00, 0x00, 0x00]
LOAD_KEY    = lambda slot, key: [0xFF,0x82,0x00,slot,0x06] + key
AUTH_BLOCK  = lambda blk, t, slot: [0xFF,0x86,0x00,0x00,0x05, 0x01,0x00,blk,t,slot]
READ_BLOCK  = lambda blk: [0xFF,0xB0,0x00,blk,0x10]
WRITE_BLOCK = lambda blk, d16: [0xFF,0xD6,0x00,blk,0x10] + d16

SLOT_A    = 0x00
SLOT_B    = 0x01
KEYTYPE_A = 0x60
KEYTYPE_B = 0x61

def parse_hex6(s):
    s = (s or "").replace(":", "").replace(" ", "").upper()
    if len(s) != 12 or any(c not in "0123456789ABCDEF" for c in s):
        raise ValueError("Key must be 12 hex chars, e.g. FFFFFFFFFFFF")
    return [int(s[i:i+2], 16) for i in range(0, 12, 2)]

def ascii_safe(bs):
    return ''.join(chr(c) if 32 <= c < 127 else '.' for c in bs)

def hex16_grouped(bs):
    parts = [f"{b:02X}" for b in bs[:16]]
    for i in (12,8,4): parts.insert(i, '')
    return ' '.join(parts)

def connect_first_reader():
    rlist = readers()
    if not rlist:
        return None, "No reader found (enable Smart Card service)."
    conn = rlist[0].createConnection()
    try:
        conn.connect()
        return conn, None
    except Exception as e:
        return None, f"Connect error: {type(e).__name__}: {e}"

def load_key(conn, slot, key_bytes):
    _, sw1, sw2 = conn.transmit(LOAD_KEY(slot, key_bytes))
    return (sw1, sw2) == (0x90, 0x00)

def auth_with(conn, block, keytype, slot):
    _, sw1, sw2 = conn.transmit(AUTH_BLOCK(block, keytype, slot))
    return (sw1, sw2) == (0x90, 0x00)

def read_block16(conn, block):
    data, sw1, sw2 = conn.transmit(READ_BLOCK(block))
    return (sw1, sw2), bytes(data)

def write_block16(conn, block, payload16):
    _, sw1, sw2 = conn.transmit(WRITE_BLOCK(block, list(payload16)))
    return (sw1, sw2)

def sector_base_block(sector):
    return sector * 4

def is_trailer_block(block):
    return (block % 4) == 3

def is_manufacturer_block(block):
    return block == 0

def detect_1k_or_4k(conn, key_a):
    try:
        if load_key(conn, SLOT_A, key_a) and auth_with(conn, 64, KEYTYPE_A, SLOT_A):
            sw, _ = read_block16(conn, 64)
            if sw == (0x90,0x00):
                return "4K"
    except Exception:
        pass
    return "1K"

def ensure_auth(conn, block, key_a, key_b=None):
    if load_key(conn, SLOT_A, key_a) and auth_with(conn, block, KEYTYPE_A, SLOT_A):
        return True, "A"
    if key_b:
        if load_key(conn, SLOT_B, key_b) and auth_with(conn, block, KEYTYPE_B, SLOT_B):
            return True, "B"
    return False, None

def make_value_block(value, block_addr):
    v = int(value) & 0xFFFFFFFF
    v_bytes = bytes([v & 0xFF, (v>>8)&0xFF, (v>>16)&0xFF, (v>>24)&0xFF])
    nv_bytes = bytes([b ^ 0xFF for b in v_bytes])
    a = block_addr & 0xFF
    return v_bytes + nv_bytes + v_bytes + bytes([a, a ^ 0xFF, a, a ^ 0xFF])

HTML = """
<!doctype html>
<html>
<head>
<meta charset="utf-8">
<title>Pre-Exam Review: MIFARE Classic (ACR122U)</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
  body { font-family: -apple-system, Segoe UI, Roboto, Helvetica, Arial, sans-serif; margin: 20px; }
  h1 { font-size: 22px; margin: 0 0 12px; }
  h2 { font-size: 18px; margin: 20px 0 8px; }
  form { margin: 10px 0; padding: 10px; border: 1px solid #ddd; border-radius: 8px; }
  label { display: inline-block; min-width: 130px; }
  input[type="text"], input[type="number"] { padding: 6px 8px; width: 280px; max-width: 90%; }
  .row { margin: 6px 0; }
  .warn { color: #b35900; }
  .ok { color: #0a7d00; }
  .err { color: #b00020; }
  table { border-collapse: collapse; width: 100%; max-width: 900px; margin-top: 10px; }
  th, td { border: 1px solid #ddd; padding: 6px 8px; text-align: left; font-family: ui-monospace, Menlo, Consolas, monospace; }
  th { background: #f5f5f5; }
  .mono { font-family: ui-monospace, Menlo, Consolas, monospace; }
</style>
</head>
<body>
<h1>Pre-Exam Review: ACR122U + MIFARE Classic</h1>

<div class="row">
  <form method="post" action="{{ url_for('refresh') }}">
    <div class="row"><strong>Reader/Card:</strong> {{ info }}</div>
    <div class="row">
      <label>Key A (hex 12):</label>
      <input name="keya" value="{{ keya or 'FFFFFFFFFFFF' }}" class="mono">
      <label style="margin-left:10px;">Key B (hex 12):</label>
      <input name="keyb" value="{{ keyb or '' }}" class="mono" placeholder="optional">
      <button type="submit">Refresh ATR+UID</button>
    </div>
  </form>
</div>

{% if atr_uid %}
  <div class="row"><strong>ATR:</strong> <span class="mono">{{ atr_uid.atr }}</span></div>
  <div class="row"><strong>UID:</strong> <span class="mono">{{ atr_uid.uid }}</span></div>
  <div class="row"><strong>Type (best-effort):</strong> {{ atr_uid.ctype }}</div>
{% endif %}

<h2>Experiment 1: View Sector (0..15) in HEX + ASCII</h2>
<form method="post" action="{{ url_for('view_sector') }}">
  <div class="row">
    <label>Sector (0..15):</label>
    <input name="sector" type="number" min="0" max="15" value="{{ sector if sector is not none else 0 }}">
  </div>
  <div class="row">
    <label>Key A (hex 12):</label>
    <input name="keya" value="{{ keya or 'FFFFFFFFFFFF' }}" class="mono">
    <label style="margin-left:10px;">Key B (hex 12):</label>
    <input name="keyb" value="{{ keyb or '' }}" class="mono" placeholder="optional">
  </div>
  <button type="submit">Read Sector</button>
</form>

{% if sector_blocks %}
  <table>
    <thead><tr><th>Block</th><th>HEX (grouped)</th><th>ASCII</th><th>Note</th></tr></thead>
    <tbody>
      {% for row in sector_blocks %}
        <tr>
          <td>{{ row.block }}</td>
          <td class="mono">{{ row.hex }}</td>
          <td class="mono">{{ row.ascii }}</td>
          <td>{{ row.note }}</td>
        </tr>
      {% endfor %}
    </tbody>
  </table>
{% endif %}

<h2>Experiment 2: Write Message to Block (16 bytes)</h2>
<form method="post" action="{{ url_for('write_message') }}">
  <div class="row warn">Do NOT write: block 0 or any sector trailer (3,7,...,63). Data will be padded/truncated to 16 bytes.</div>
  <div class="row">
    <label>Block (0..63):</label>
    <input name="block" type="number" min="0" max="63" value="{{ block2 if block2 is not none else 4 }}">
  </div>
  <div class="row">
    <label>Message (max 16B):</label>
    <input name="message" type="text" placeholder="Hello, RFID!">
  </div>
  <div class="row">
    <label>Key A (hex 12):</label>
    <input name="keya" value="{{ keya or 'FFFFFFFFFFFF' }}" class="mono">
    <label style="margin-left:10px;">Key B (hex 12):</label>
    <input name="keyb" value="{{ keyb or '' }}" class="mono" placeholder="optional">
  </div>
  <button type="submit">Write Message</button>
</form>
{% if msg_status %}
  <div class="{{ 'ok' if msg_status.ok else 'err' }}">{{ msg_status.text }}</div>
{% endif %}

<h2>Experiment 3: Write Value Block (wallet)</h2>
<form method="post" action="{{ url_for('write_value') }}">
  <div class="row warn">Value block layout will be used (val,~val,val,addr,~addr,addr,~addr). Choose a data block.</div>
  <div class="row">
    <label>Block (0..63):</label>
    <input name="block" type="number" min="0" max="63" value="{{ block3 if block3 is not none else 8 }}">
  </div>
  <div class="row">
    <label>Value (int):</label>
    <input name="value" type="number" value="100">
  </div>
  <div class="row">
    <label>Key A (hex 12):</label>
    <input name="keya" value="{{ keya or 'FFFFFFFFFFFF' }}" class="mono">
    <label style="margin-left:10px;">Key B (hex 12):</label>
    <input name="keyb" value="{{ keyb or '' }}" class="mono" placeholder="optional">
  </div>
  <button type="submit">Write Value Block</button>
</form>
{% if val_status %}
  <div class="{{ 'ok' if val_status.ok else 'err' }}">{{ val_status.text }}</div>
{% endif %}

</body>
</html>
"""

app = Flask(__name__)

def render_index(extra_ctx=None):
    ctx = dict(info="(press Refresh to read ATR/UID)", atr_uid=None,
               keya=request.form.get('keya') if request.method=='POST' else request.args.get('keya','FFFFFFFFFFFF'),
               keyb=request.form.get('keyb') if request.method=='POST' else request.args.get('keyb',''),
               sector=None, sector_blocks=None, block2=None, msg_status=None, block3=None, val_status=None)
    if extra_ctx: ctx.update(extra_ctx)
    return render_template_string(HTML, **ctx)

def detect_type_and_uid(conn, keya_hex):
    data, sw1, sw2 = conn.transmit(GET_UID)
    uid = " ".join(f"{b:02X}" for b in data) if (sw1, sw2) == (0x90,0x00) else "N/A"
    atr = " ".join(f"{b:02X}" for b in conn.getATR())
    try:
        keya = parse_hex6(keya_hex)
    except Exception:
        keya = [0xFF]*6
    ctype = "1K"
    try:
        if load_key(conn, SLOT_A, keya) and auth_with(conn, 64, KEYTYPE_A, SLOT_A):
            sw, _ = read_block16(conn, 64)
            if sw == (0x90,0x00): ctype = "4K"
    except Exception:
        pass
    return dict(atr=atr, uid=uid, ctype=ctype)

@app.route("/", methods=["GET"])
def index():
    return render_index()

@app.route("/refresh", methods=["POST"])
def refresh():
    keya_hex = request.form.get("keya","FFFFFFFFFFFF")
    keyb_hex = request.form.get("keyb","")
    info = ""
    atr_uid = None
    conn, err = connect_first_reader()
    if err:
        info = err
    else:
        try:
            atr_uid = detect_type_and_uid(conn, keya_hex)
            info = "OK"
        except Exception as e:
            info = f"Read error: {type(e).__name__}: {e}"
        finally:
            try: conn.disconnect()
            except Exception: pass
    return render_template_string(HTML, info=info, atr_uid=atr_uid, keya=keya_hex, keyb=keyb_hex,
                                  sector=None, sector_blocks=None, block2=None, msg_status=None, block3=None, val_status=None)

@app.route("/view_sector", methods=["POST"])
def view_sector():
    sector = int(request.form.get("sector","0"))
    keya_hex = request.form.get("keya","FFFFFFFFFFFF")
    keyb_hex = request.form.get("keyb","")
    rows = []
    info = ""
    atr_uid = None

    try:
        key_a = parse_hex6(keya_hex)
        key_b = parse_hex6(keyb_hex) if keyb_hex.strip() else None
    except Exception as e:
        return render_template_string(HTML, info=f"Key parse error: {e}", atr_uid=None, keya=keya_hex, keyb=keyb_hex,
                                      sector=sector, sector_blocks=None, block2=None, msg_status=None, block3=None, val_status=None)

    conn, err = connect_first_reader()
    if err:
        info = err
        return render_template_string(HTML, info=info, atr_uid=None, keya=keya_hex, keyb=keyb_hex,
                                      sector=sector, sector_blocks=None, block2=None, msg_status=None, block3=None, val_status=None)
    try:
        atr_uid = detect_type_and_uid(conn, keya_hex)
        base = sector_base_block(sector)
        for i in range(4):
            blk = base + i
            ok, which = ensure_auth(conn, blk, key_a, key_b)
            if not ok:
                rows.append(dict(block=blk, hex="<AUTH FAIL>", ascii="", note="auth failed"))
                continue
            sw, raw = read_block16(conn, blk)
            if sw != (0x90,0x00):
                rows.append(dict(block=blk, hex=f"<READ FAIL SW={sw[0]:02X}{sw[1]:02X}>", ascii="", note=""))
            else:
                note = "TRAILER" if is_trailer_block(blk) else ("MFG" if blk==0 else "")
                rows.append(dict(block=blk, hex=hex16_grouped(raw), ascii=ascii_safe(raw), note=note))
        info = "OK"
    except Exception as e:
        info = f"Error: {type(e).__name__}: {e}"
    finally:
        try: conn.disconnect()
        except Exception: pass

    return render_template_string(HTML, info=info, atr_uid=atr_uid, keya=keya_hex, keyb=keyb_hex,
                                  sector=sector, sector_blocks=rows, block2=None, msg_status=None, block3=None, val_status=None)

@app.route("/write_message", methods=["POST"])
def write_message():
    keya_hex = request.form.get("keya","FFFFFFFFFFFF")
    keyb_hex = request.form.get("keyb","")
    block = int(request.form.get("block","4"))
    message = request.form.get("message","").encode("utf-8", errors="ignore")[:16]
    if len(message) < 16:
        message = message + b' '*(16-len(message))

    try:
        key_a = parse_hex6(keya_hex)
        key_b = parse_hex6(keyb_hex) if keyb_hex.strip() else None
    except Exception as e:
        return render_template_string(HTML, info="", atr_uid=None, keya=keya_hex, keyb=keyb_hex,
                                      sector=None, sector_blocks=None, block2=block,
                                      msg_status=dict(ok=False, text=f"Key parse error: {e}"),
                                      block3=None, val_status=None)

    if is_manufacturer_block(block) or is_trailer_block(block):
        return render_template_string(HTML, info="", atr_uid=None, keya=keya_hex, keyb=keyb_hex,
                                      sector=None, sector_blocks=None, block2=block,
                                      msg_status=dict(ok=False, text="Forbidden block: manufacturer (0) or trailer (3,7,...)"),
                                      block3=None, val_status=None)

    conn, err = connect_first_reader()
    if err:
        return render_template_string(HTML, info=err, atr_uid=None, keya=keya_hex, keyb=keyb_hex,
                                      sector=None, sector_blocks=None, block2=block,
                                      msg_status=dict(ok=False, text=err), block3=None, val_status=None)
    try:
        ok, which = ensure_auth(conn, block, key_a, key_b)
        if not ok:
            status = dict(ok=False, text="AUTH FAIL (check keys/access bits)")
        else:
            sw1, sw2 = write_block16(conn, block, message)
            if (sw1, sw2) == (0x90,0x00):
                status = dict(ok=True, text=f"Write OK at block {block}.")
            else:
                status = dict(ok=False, text=f"WRITE FAIL SW={sw1:02X}{sw2:02X}")
    except Exception as e:
        status = dict(ok=False, text=f"Error: {type(e).__name__}: {e}")
    finally:
        try: conn.disconnect()
        except Exception: pass

    return render_template_string(HTML, info="", atr_uid=None, keya=keya_hex, keyb=keyb_hex,
                                  sector=None, sector_blocks=None, block2=block,
                                  msg_status=status, block3=None, val_status=None)

@app.route("/write_value", methods=["POST"])
def write_value():
    keya_hex = request.form.get("keya","FFFFFFFFFFFF")
    keyb_hex = request.form.get("keyb","")
    block = int(request.form.get("block","8"))
    try:
        value = int(request.form.get("value","0"))
    except:
        value = 0

    try:
        key_a = parse_hex6(keya_hex)
        key_b = parse_hex6(keyb_hex) if keyb_hex.strip() else None
    except Exception as e:
        return render_template_string(HTML, info="", atr_uid=None, keya=keya_hex, keyb=keyb_hex,
                                      sector=None, sector_blocks=None, block2=None, msg_status=None,
                                      block3=block, val_status=dict(ok=False, text=f"Key parse error: {e}"))

    if is_manufacturer_block(block) or is_trailer_block(block):
        return render_template_string(HTML, info="", atr_uid=None, keya=keya_hex, keyb=keyb_hex,
                                      sector=None, sector_blocks=None, block2=None, msg_status=None,
                                      block3=block, val_status=dict(ok=False, text="Forbidden block: manufacturer (0) or trailer (3,7,...)"))

    payload = make_value_block(value, block)

    conn, err = connect_first_reader()
    if err:
        return render_template_string(HTML, info=err, atr_uid=None, keya=keya_hex, keyb=keyb_hex,
                                      sector=None, sector_blocks=None, block2=None, msg_status=None,
                                      block3=block, val_status=dict(ok=False, text=err))
    try:
        ok, which = ensure_auth(conn, block, key_a, key_b)
        if not ok:
            status = dict(ok=False, text="AUTH FAIL (check keys/access bits)")
        else:
            sw1, sw2 = write_block16(conn, block, payload)
            if (sw1, sw2) == (0x90,0x00):
                status = dict(ok=True, text=f"Value block written at block {block} (value={value}).")
            else:
                status = dict(ok=False, text=f"WRITE FAIL SW={sw1:02X}{sw2:02X}")
    except Exception as e:
        status = dict(ok=False, text=f"Error: {type(e).__name__}: {e}")
    finally:
        try: conn.disconnect()
        except Exception: pass

    return render_template_string(HTML, info="", atr_uid=None, keya=keya_hex, keyb=keyb_hex,
                                  sector=None, sector_blocks=None, block2=None, msg_status=None,
                                  block3=block, val_status=status)

def main():
    ap = argparse.ArgumentParser(description="Pre-exam review Flask app for ACR122U + MIFARE Classic")
    ap.add_argument("--host", type=str, default="127.0.0.1")
    ap.add_argument("--port", type=int, default=5000)
    ap.add_argument("--quiet", action="store_true")
    args = ap.parse_args()
    if args.quiet:
        logging.getLogger("werkzeug").setLevel(logging.ERROR)
    app.run(host=args.host, port=args.port, debug=False, use_reloader=False, threaded=True)

if __name__ == "__main__":
    main()
