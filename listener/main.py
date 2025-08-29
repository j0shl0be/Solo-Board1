import serial
import serial.tools.list_ports
import time
import atexit
import signal
import sys
import platform
import subprocess

TARGET_VID = 0x239a
TARGET_PID = 0xcafe


def find_target_port():
	ports = serial.tools.list_ports.comports()
	for p in ports:
		if p.vid == TARGET_VID and p.pid == TARGET_PID:
			return p.device
	return None


def open_serial_blocking(baudrate=115200, timeout_s=0.1, retry_delay_s=2.0):
	while True:
		device = find_target_port()
		if device is not None:
			try:
				# Create closed Serial, set params, then open once
				s = serial.Serial()
				s.baudrate = baudrate
				s.timeout = timeout_s
				s.port = device
				s.open()
				print(f"Connected to {device}")
				return s
			except Exception as e:
				try:
					if 's' in locals() and getattr(s, 'is_open', False):
						s.close()
				except Exception:
					pass
				print(f"Failed to open {device}: {e}")
		else:
			print("Waiting for device ...")
		time.sleep(retry_delay_s)


stack = []
timeout = 0.8
last_time = time.time()
ser = None


def _set_volume_windows(percent: int) -> None:
	try:
		from ctypes import POINTER, cast
		from comtypes import CLSCTX_ALL  # type: ignore
		from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume  # type: ignore
		clamped = max(0, min(100, int(percent)))
		devices = AudioUtilities.GetSpeakers()
		interface = devices.Activate(IAudioEndpointVolume._iid_, CLSCTX_ALL, None)
		volume = cast(interface, POINTER(IAudioEndpointVolume))
		volume.SetMasterVolumeLevelScalar(clamped / 100.0, None)
	except Exception as e:
		print(f"Failed to set Windows volume: {e}")


def _set_volume_macos(percent: int) -> None:
	try:
		clamped = max(0, min(100, int(percent)))
		subprocess.run([
			"osascript",
			"-e",
			f"set volume output volume {clamped}",
		], check=True)
	except Exception as e:
		print(f"Failed to set macOS volume: {e}")


def _set_volume_linux(percent: int) -> None:
	# Best-effort using amixer if available
	try:
		clamped = max(0, min(100, int(percent)))
		subprocess.run(["amixer", "-D", "pulse", "sset", "Master", f"{clamped}%"], check=True)
	except Exception as e:
		print(f"Failed to set Linux volume: {e}")


def set_volume(volume):
	os_name = platform.system()
	if os_name == "Windows":
		_set_volume_windows(volume)
	elif os_name == "Darwin":
		_set_volume_macos(volume)
	else:
		_set_volume_linux(volume)


def cleanup():
	global ser
	try:
		if ser is not None and ser.is_open:
			ser.close()
			print("Serial port closed")
	except Exception:
		pass


def handle_signal(signum, frame):
	cleanup()
	sys.exit(0)


# Register cleanup for normal exits and signals
atexit.register(cleanup)
try:
	signal.signal(signal.SIGINT, handle_signal)
	signal.signal(signal.SIGTERM, handle_signal)
except Exception:
	pass


try:
	while True:
		if ser is None or not ser.is_open:
			ser = open_serial_blocking()
			last_time = time.time()

		try:
			if ser.in_waiting > 0:
				raw = ser.readline()
				try:
					data_str = raw.decode('utf-8', errors='ignore').rstrip()
				except Exception:
					data_str = ""
				if len(data_str) >= 4:
					payload = data_str[4:]
					try:
						print(payload)
						stack.append(int(payload))
						last_time = time.time()
					except ValueError:
						pass

			if time.time() - last_time > timeout:
				if len(stack) > 0:
					set_volume(stack.pop())
					stack.clear()

			# lil sleep to avoid busy spinning when idle
			time.sleep(0.02)

		except (serial.SerialException, OSError) as e:
			# Handle disconnects gracefully close and retry
			try:
				if ser is not None and ser.is_open:
					ser.close()
			except Exception:
				pass
			print(f"Serial error: {e}. Will retry when device is available.")
			ser = None
			stack.clear()
			time.sleep(0.5)
except KeyboardInterrupt:
	print("Interrupted by user. Exiting...")
finally:
	cleanup()

