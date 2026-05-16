"""
Phase 5 bridge: Nano 33 BLE Sense Rev2 -> BLE -> Flask dashboard

What it does:
- scans for the Nano's BLE device name
- connects with Bleak
- reads two characteristics:
    * SpO2
    * signal-valid flag
- serves a browser dashboard with live values

Install:
    pip install flask bleak

Run:
    python pulse_ox_bridge.py

Then open:
    http://127.0.0.1:5000
"""

import asyncio
import threading
import time
from dataclasses import dataclass

from bleak import BleakClient, BleakScanner
from flask import Flask, jsonify, render_template_string

# Match the UUIDs in your Arduino BLE sketch
SERVICE_UUID = "19B10000-E8F2-537E-4F6C-D104768A1214"
SPO2_UUID = "19B10001-E8F2-537E-4F6C-D104768A1214"
SIGNAL_UUID = "19B10002-E8F2-537E-4F6C-D104768A1214"

DEVICE_NAME = "PulseOxNano"

app = Flask(__name__)


@dataclass
class SharedState:
    connected: bool = False
    device_name: str = ""
    spo2: int = 0
    signal_ok: int = 0
    last_update: float = 0.0
    status: str = "Searching for device..."


state = SharedState()
state_lock = threading.Lock()


HTML = """
<!doctype html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Pulse Ox Dashboard</title>
  <style>
    body { font-family: Arial, sans-serif; background:#0b1220; color:#e8eef9; margin:0; padding:20px; }
    .wrap { max-width:900px; margin:0 auto; }
    .grid { display:grid; grid-template-columns: repeat(3, 1fr); gap:14px; margin-top:16px; }
    .card { background:#121a2b; border:1px solid #22304a; border-radius:18px; padding:16px; }
    .label { opacity:.75; font-size:14px; }
    .value { font-size:42px; font-weight:700; margin-top:8px; }
    .small { opacity:.75; font-size:13px; margin-top:6px; }
    .status { margin-top:16px; padding:14px 16px; background:#101827; border:1px solid #22304a; border-radius:18px; }
    .ok { color:#8ef0a0; }
    .bad { color:#ff9aa2; }
    @media (max-width: 800px) { .grid { grid-template-columns:1fr; } .value { font-size:34px; } }
  </style>
</head>
<body>
  <div class="wrap">
    <h1>Pulse Oximeter Dashboard</h1>
    <div class="grid">
      <div class="card">
        <div class="label">SpO2</div>
        <div id="spo2" class="value">--</div>
        <div class="small">oxygen saturation</div>
      </div>
      <div class="card">
        <div class="label">Signal</div>
        <div id="signal" class="value">--</div>
        <div class="small">valid reading</div>
      </div>
      <div class="card">
        <div class="label">Connection</div>
        <div id="conn" class="value" style="font-size:22px;">--</div>
        <div class="small">BLE link status</div>
      </div>
    </div>

    <div class="status">
      <div class="label">Status</div>
      <div id="status" style="font-size:18px; margin-top:6px;">Loading...</div>
      <div id="updated" class="small"></div>
    </div>
  </div>

<script>
async function refresh() {
  const res = await fetch('/api/state', { cache: 'no-store' });
  const d = await res.json();

  document.getElementById('spo2').textContent = d.spo2 ? (d.spo2 + '%') : '--';
  document.getElementById('signal').textContent = d.signal_ok ? 'OK' : '--';
  document.getElementById('signal').className = 'value ' + (d.signal_ok ? 'ok' : 'bad');
  document.getElementById('conn').textContent = d.connected ? 'Connected' : 'Offline';
  document.getElementById('conn').className = 'value ' + (d.connected ? 'ok' : 'bad');
  document.getElementById('status').textContent = d.status;
  document.getElementById('updated').textContent = d.last_update ? ('Last update: ' + new Date(d.last_update * 1000).toLocaleTimeString()) : '';
}
refresh();
setInterval(refresh, 1000);
</script>
</body>
</html>
"""


@app.route("/")
def index():
    return render_template_string(HTML)


@app.route("/api/state")
def api_state():
    with state_lock:
        return jsonify(
            connected=state.connected,
            device_name=state.device_name,
            spo2=state.spo2,
            signal_ok=state.signal_ok,
            last_update=state.last_update,
            status=state.status,
        )


def update_state(**kwargs):
    with state_lock:
        for key, value in kwargs.items():
            setattr(state, key, value)


async def ble_worker():
    while True:
        update_state(connected=False, status="Scanning for Nano BLE device...")

        device = await BleakScanner.find_device_by_filter(
            lambda d, ad: (d.name == DEVICE_NAME) or (DEVICE_NAME in (d.name or "")),
            timeout=10.0,
        )

        if device is None:
            update_state(status="Device not found. Make sure the Nano is powered and advertising.")
            await asyncio.sleep(2)
            continue

        update_state(device_name=device.name or DEVICE_NAME, status="Connecting to Nano BLE device...")

        try:
            async with BleakClient(device) as client:
                if not client.is_connected:
                    update_state(status="BLE connect failed. Retrying...")
                    await asyncio.sleep(2)
                    continue

                update_state(connected=True, status="Connected. Reading live values...")

                while client.is_connected:
                    try:
                        spo2_raw = await client.read_gatt_char(SPO2_UUID)
                        signal_raw = await client.read_gatt_char(SIGNAL_UUID)

                        spo2_value = int(spo2_raw[0]) if spo2_raw else 0
                        signal_value = int(signal_raw[0]) if signal_raw else 0

                        update_state(
                            spo2=spo2_value,
                            signal_ok=signal_value,
                            last_update=time.time(),
                            status="Live data received from Nano.",
                        )
                    except Exception as read_error:
                        update_state(status=f"Read error: {read_error}")
                        break

                    await asyncio.sleep(0.25)

        except Exception as conn_error:
            update_state(status=f"Connection error: {conn_error}")

        update_state(connected=False, status="Disconnected. Reconnecting...")
        await asyncio.sleep(2)


def start_ble_thread():
    asyncio.run(ble_worker())


if __name__ == "__main__":
    ble_thread = threading.Thread(target=start_ble_thread, daemon=True)
    ble_thread.start()

    print("Starting dashboard at http://127.0.0.1:5000")
    app.run(host="127.0.0.1", port=5000, debug=False)
