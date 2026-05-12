"""
Motor Monitor — Real-time COM port visualizer
  python motor_monitor.py
  python motor_monitor.py --port COM3 --baud 115200
  python motor_monitor.py --port /dev/ttyUSB0 --baud 9600
  python motor_monitor.py --demo
"""

import argparse
import threading
import queue
import re
import time
import math
import random
import sys
from collections import deque
from datetime import datetime

import serial
import serial.tools.list_ports
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
import matplotlib.animation as animation
from matplotlib.patches import FancyBboxPatch

# ── Settings ──────────────────────────────────────────────────────────────

WINDOW_SIZE   = 200
UPDATE_MS     = 50
DEFAULT_BAUD  = 115200

COLORS = {
    'bg':           '#0d1117',
    'panel':        '#161b22',
    'border':       '#30363d',
    'text':         '#e6edf3',
    'text_dim':     '#8b949e',
    'accent':       '#58a6ff',
    'green':        '#3fb950',
    'yellow':       '#d29922',
    'red':          '#f85149',
    'purple':       '#bc8cff',
    'orange':       '#ffa657',
    'speed_target': '#58a6ff',
    'speed_actual': '#3fb950',
    'current':      '#ffa657',
    'power':        '#bc8cff',
    'voltage':      '#d29922',
    'temp':         '#f85149',
}

# ── Regular expression ─────────────────────────────────────────────────────────────

PATTERN = re.compile(
    r'time:\s*(?P<date>[\d\.]+)\s+(?P<clock>[\d:]+)\s*-\s*(?P<ms>\d+)\s*ms'
    r'.*?temp:\s*(?P<temp>[\d\.\-]+)'
    r'.*?current\s*A:\s*(?P<current>[\d\.\-]+)'
    r'.*?power\s*W:\s*(?P<power>[\d\.\-]+)'
    r'.*?voltage\s*V:\s*(?P<voltage>[\d\.\-]+)'
    r'.*?target_speed:\s*(?P<target>[\d\.\-]+)'
    r'.*?motor_speed:\s*(?P<speed>[\d\.\-]+)',
    re.IGNORECASE
)

def parse_line(line: str):
    m = PATTERN.search(line)
    if not m:
        return None
    return {
        'ts':      time.time(),
        'temp':    float(m.group('temp')),
        'current': float(m.group('current')),
        'power':   float(m.group('power')),
        'voltage': float(m.group('voltage')),
        'target':  float(m.group('target')),
        'speed':   float(m.group('speed')),
    }

# ── Demo generator  ─────────────────────────────────────

def demo_generator(q: queue.Queue):
    t = 0.0
    speed = 0.0
    target = 50.0
    voltage = 7.4
    temp = 23.0

    while True:
        t += 0.05

        error = target - speed
        speed += error * 0.08 + random.gauss(0, 0.3)
        speed = max(0.0, min(100.0, speed))


        if int(t) % 10 == 0 and int(t) != int(t - 0.05):
            target = random.choice([10, 30, 50, 70, 90])

        current = speed * 0.015 + random.gauss(0, 0.005)
        power   = current * voltage
        voltage += random.gauss(0, 0.01)
        voltage  = max(7.0, min(8.4, voltage))
        temp    += random.gauss(0, 0.02)
        temp     = max(20.0, min(80.0, temp))

        now = datetime.now()
        line = (
            f"time: {now.strftime('%d.%m.%Y %H:%M:%S')} - {now.microsecond//1000} ms,"
            f"temp: {temp:.2f},current A: {current:.3f},"
            f"power W: {power:.2f}, voltage V: {voltage:.2f},"
            f"target_speed: {target:.0f}, motor_speed: {speed:.1f}"
        )
        parsed = parse_line(line)
        if parsed:
            q.put(parsed)
        time.sleep(0.05)

# ── COM reader ─────────────────────────────────────────────────────────────

def serial_reader(port: str, baud: int, q: queue.Queue):
    while True:
        try:
            print(f"[COM] Connection to {port} @ {baud}...")
            with serial.Serial(port, baud, timeout=1) as ser:
                print(f"[COM] Connected with {port}")
                while True:
                    try:
                        raw = ser.readline()
                        line = raw.decode('utf-8', errors='replace').strip()
                        if not line:
                            continue
                        parsed = parse_line(line)
                        if parsed:
                            q.put(parsed)
                        else:
                            print(f"[SKIP] {line[:80]}")
                    except Exception as e:
                        print(f"[READ ERR] {e}")
                        break
        except serial.SerialException as e:
            print(f"[COM ERR] {e} — Restart")
            time.sleep(3)

class MotorMonitor:
    def __init__(self, q: queue.Queue):
        self.q = q
        self.n = WINDOW_SIZE

        self.ts      = deque(maxlen=self.n)
        self.speed   = deque(maxlen=self.n)
        self.target  = deque(maxlen=self.n)
        self.current = deque(maxlen=self.n)
        self.power   = deque(maxlen=self.n)
        self.voltage = deque(maxlen=self.n)
        self.temp    = deque(maxlen=self.n)

        self.last = {}
        self.pkt_count = 0
        self.start_time = time.time()

        self._build_ui()

    def _style_ax(self, ax, title=''):
        ax.set_facecolor(COLORS['panel'])
        ax.tick_params(colors=COLORS['text_dim'], labelsize=8)
        ax.spines['bottom'].set_color(COLORS['border'])
        ax.spines['top'].set_color(COLORS['border'])
        ax.spines['left'].set_color(COLORS['border'])
        ax.spines['right'].set_color(COLORS['border'])
        if title:
            ax.set_title(title, color=COLORS['text_dim'], fontsize=9,
                         fontfamily='monospace', pad=4)
        ax.grid(True, color=COLORS['border'], linewidth=0.5, alpha=0.7)
        ax.set_xlim(0, self.n)

    def _build_ui(self):
        matplotlib.rcParams['toolbar'] = 'None'
        self.fig = plt.figure(figsize=(14, 8), facecolor=COLORS['bg'])
        self.fig.canvas.manager.set_window_title('Motor Monitor — COM Visualizer')

        gs = gridspec.GridSpec(
            3, 3,
            figure=self.fig,
            hspace=0.45, wspace=0.35,
            left=0.06, right=0.97,
            top=0.88, bottom=0.07
        )

        self.fig.text(
            0.5, 0.955, '⚙  MOTOR MONITOR',
            ha='center', va='center',
            color=COLORS['accent'], fontsize=16,
            fontfamily='monospace', fontweight='bold'
        )
        self.status_text = self.fig.text(
            0.5, 0.925, 'Waiting data...',
            ha='center', va='center',
            color=COLORS['text_dim'], fontsize=9,
            fontfamily='monospace'
        )

        self.ax_speed   = self.fig.add_subplot(gs[0, :2])
        self.ax_current = self.fig.add_subplot(gs[1, 0])
        self.ax_power   = self.fig.add_subplot(gs[1, 1])
        self.ax_voltage = self.fig.add_subplot(gs[2, 0])
        self.ax_temp    = self.fig.add_subplot(gs[2, 1])
        self.ax_info    = self.fig.add_subplot(gs[:, 2])

        self._style_ax(self.ax_speed,   'Speed  [%]')
        self._style_ax(self.ax_current, 'Current  [A]')
        self._style_ax(self.ax_power,   'Power  [W]')
        self._style_ax(self.ax_voltage, 'Voltage  [V]')
        self._style_ax(self.ax_temp,    'Temperature  [°C]')

        x = list(range(self.n))
        zeros = [0] * self.n

        self.ln_target, = self.ax_speed.plot(
            x, zeros, color=COLORS['speed_target'],
            linewidth=1.2, linestyle='--', label='Target', alpha=0.8)
        self.ln_speed, = self.ax_speed.plot(
            x, zeros, color=COLORS['speed_actual'],
            linewidth=1.5, label='Actual')
        self.ax_speed.legend(
            loc='upper left', fontsize=8,
            facecolor=COLORS['panel'], edgecolor=COLORS['border'],
            labelcolor=COLORS['text'])
        self.ax_speed.set_ylim(-5, 110)

        self.ln_current, = self.ax_current.plot(
            x, zeros, color=COLORS['current'], linewidth=1.2)
        self.ln_power, = self.ax_power.plot(
            x, zeros, color=COLORS['power'], linewidth=1.2)
        self.ln_voltage, = self.ax_voltage.plot(
            x, zeros, color=COLORS['voltage'], linewidth=1.2)
        self.ln_temp, = self.ax_temp.plot(
            x, zeros, color=COLORS['temp'], linewidth=1.2)

        self.ax_info.set_facecolor(COLORS['panel'])
        self.ax_info.set_xlim(0, 1)
        self.ax_info.set_ylim(0, 1)
        self.ax_info.axis('off')
        for spine in self.ax_info.spines.values():
            spine.set_color(COLORS['border'])

        self.ax_info.set_title(
            'LIVE VALUES', color=COLORS['text_dim'],
            fontsize=9, fontfamily='monospace', pad=6)

        labels = [
            ('target_lbl',  0.08, 0.88, 'TARGET SPEED', COLORS['text_dim'], 8),
            ('target_val',  0.08, 0.80, '---',           COLORS['speed_target'], 18),
            ('speed_lbl',   0.08, 0.70, 'ACTUAL SPEED',  COLORS['text_dim'], 8),
            ('speed_val',   0.08, 0.62, '---',           COLORS['speed_actual'], 18),
            ('sep1',        0.08, 0.55, '─' * 22,        COLORS['border'], 8),
            ('cur_lbl',     0.08, 0.50, 'CURRENT',       COLORS['text_dim'], 8),
            ('cur_val',     0.08, 0.43, '--- A',         COLORS['current'], 13),
            ('pwr_lbl',     0.08, 0.36, 'POWER',         COLORS['text_dim'], 8),
            ('pwr_val',     0.08, 0.29, '--- W',         COLORS['power'], 13),
            ('volt_lbl',    0.08, 0.22, 'VOLTAGE',       COLORS['text_dim'], 8),
            ('volt_val',    0.08, 0.15, '--- V',         COLORS['voltage'], 13),
            ('temp_lbl',    0.08, 0.08, 'TEMPERATURE',   COLORS['text_dim'], 8),
            ('temp_val',    0.08, 0.01, '--- °C',        COLORS['temp'], 13),
        ]

        self.info_texts = {}
        for key, x, y, txt, clr, sz in labels:
            self.info_texts[key] = self.ax_info.text(
                x, y, txt, color=clr, fontsize=sz,
                fontfamily='monospace', fontweight='bold',
                transform=self.ax_info.transAxes, va='bottom')

        self.pkt_text = self.fig.text(
            0.06, 0.955, 'PKT: 0 | 0.0 Hz',
            color=COLORS['text_dim'], fontsize=8,
            fontfamily='monospace')

    def _get_plot_data(self, buf):
        data = list(buf)
        n = len(data)
        x = list(range(self.n - n, self.n))
        return x, data

    def update(self, frame):
        new_data = False
        while not self.q.empty():
            try:
                d = self.q.get_nowait()
                self.ts.append(d['ts'])
                self.speed.append(d['speed'])
                self.target.append(d['target'])
                self.current.append(d['current'])
                self.power.append(d['power'])
                self.voltage.append(d['voltage'])
                self.temp.append(d['temp'])
                self.last = d
                self.pkt_count += 1
                new_data = True
            except queue.Empty:
                break

        if not new_data or not self.last:
            return

        d = self.last

        xs, ys = self._get_plot_data(self.speed)
        self.ln_speed.set_data(xs, ys)

        xt, yt = self._get_plot_data(self.target)
        self.ln_target.set_data(xt, yt)

        xc, yc = self._get_plot_data(self.current)
        self.ln_current.set_data(xc, yc)
        if yc:
            margin = max(abs(max(yc)), abs(min(yc))) * 0.2 + 0.01
            self.ax_current.set_ylim(min(yc) - margin, max(yc) + margin)

        xp, yp = self._get_plot_data(self.power)
        self.ln_power.set_data(xp, yp)
        if yp:
            margin = max(abs(max(yp)), abs(min(yp))) * 0.2 + 0.01
            self.ax_power.set_ylim(min(yp) - margin, max(yp) + margin)

        xv, yv = self._get_plot_data(self.voltage)
        self.ln_voltage.set_data(xv, yv)
        if yv:
            margin = (max(yv) - min(yv)) * 0.3 + 0.1
            self.ax_voltage.set_ylim(min(yv) - margin, max(yv) + margin)

        xte, yte = self._get_plot_data(self.temp)
        self.ln_temp.set_data(xte, yte)
        if yte:
            margin = (max(yte) - min(yte)) * 0.3 + 0.5
            self.ax_temp.set_ylim(min(yte) - margin, max(yte) + margin)

        self.info_texts['target_val'].set_text(f"{d['target']:.0f} %")
        self.info_texts['speed_val'].set_text(f"{d['speed']:.1f} %")
        self.info_texts['cur_val'].set_text(f"{d['current']:.3f} A")
        self.info_texts['pwr_val'].set_text(f"{d['power']:.2f} W")
        self.info_texts['volt_val'].set_text(f"{d['voltage']:.2f} V")
        self.info_texts['temp_val'].set_text(f"{d['temp']:.1f} °C")

        if d['temp'] > 60:
            self.info_texts['temp_val'].set_color(COLORS['red'])
        elif d['temp'] > 40:
            self.info_texts['temp_val'].set_color(COLORS['yellow'])
        else:
            self.info_texts['temp_val'].set_color(COLORS['temp'])

        elapsed = time.time() - self.start_time
        hz = self.pkt_count / elapsed if elapsed > 0 else 0
        self.pkt_text.set_text(f"PKT: {self.pkt_count} | {hz:.1f} Hz")

        error = d['target'] - d['speed']
        self.status_text.set_text(
            f"speed: {d['speed']:.1f}%  target: {d['target']:.0f}%  "
            f"err: {error:+.1f}%  |  "
            f"I={d['current']:.3f}A  P={d['power']:.2f}W  "
            f"U={d['voltage']:.2f}V  T={d['temp']:.1f}°C"
        )

        return (self.ln_speed, self.ln_target, self.ln_current,
                self.ln_power, self.ln_voltage, self.ln_temp)

# ── Ports list ─────────────────────────────────────────────────────────────

def list_ports():
    ports = serial.tools.list_ports.comports()
    if not ports:
        print("No ports")
        return
    print("Available ports:")
    for p in ports:
        print(f"  {p.device:12s} — {p.description}")

# ── Main ──────────────────────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(
        description='Motor Monitor — real-time COM port visualizer')
    parser.add_argument('--port',  default=None,        help='COM port or /dev/ttyUSB0)')
    parser.add_argument('--baud',  default=DEFAULT_BAUD, type=int, help='Baudrate')
    parser.add_argument('--demo',  action='store_true', help='Demo mode without hardware')
    parser.add_argument('--list',  action='store_true', help='Show available COM ports')
    args = parser.parse_args()

    if args.list:
        list_ports()
        return

    data_q = queue.Queue(maxsize=1000)

    if args.demo:
        print("[DEMO] Running in demo mode (without COM port)")
        t = threading.Thread(target=demo_generator, args=(data_q,), daemon=True)
        t.start()
    else:
        if args.port is None:
            ports = serial.tools.list_ports.comports()
            if not ports:
                print("No COM ports found. Use --demo or --port COMx")
                print("Available options:")
                print("  python motor_monitor.py --demo")
                print("  python motor_monitor.py --port COM3 --baud 115200")
                print("  python motor_monitor.py --list")
                sys.exit(1)
            args.port = ports[0].device
            print(f"[AUTO] Port selected: {args.port}")

        t = threading.Thread(
            target=serial_reader,
            args=(args.port, args.baud, data_q),
            daemon=True)
        t.start()

    monitor = MotorMonitor(data_q)

    ani = animation.FuncAnimation(
        monitor.fig,
        monitor.update,
        interval=UPDATE_MS,
        blit=False,
        cache_frame_data=False
    )

    plt.show()

if __name__ == '__main__':
    main()
