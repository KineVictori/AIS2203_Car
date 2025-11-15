
"""
Receiver that automatically picks a free TCP port and prints it.
Usage: python receiver_autoport.py
"""
import socket
import threading
import json
from jetracer.nvidia_racecar import NvidiaRacecar

car = NvidiaRacecar()

HOST = "0.0.0.0"
PORT = 43457

state = {"steering": 0, "throttle": 0, "steering_gain": 0.65, "steering_offset": 0, "throttle_gain": 0.8}
state_lock = threading.Lock()

def handle_json_payload(payload):
    with state_lock:
        for k in state.keys():
            if k in payload:
                state[k] = payload[k]
        car.steering = float(state["steering"])
        car.throttle = float(state["throttle"])

        car.steering_gain = float(state["steering_gain"])
        car.steering_gain = float(state["steering_offset"])
        car.steering_gain = float(state["throttle_gain"])

def handle_client(conn, addr):
    with conn:
        buffer = b""
        while True:
            data = conn.recv(4096)
            if not data:
                break
            buffer += data
            while b"\n" in buffer:
                line, buffer = buffer.split(b"\n", 1)
                if not line.strip():
                    continue
                try:
                    payload = json.loads(line.decode("utf-8"))
                    handle_json_payload(payload)
                    
                except json.JSONDecodeError:
                    print(f"[!] Invalid JSON: {line!r}")

def accept_loop():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((HOST, PORT))
        port = s.getsockname()[1]
        print(f"[i] Listening on {HOST}:{port}")
        s.listen()
        while True:
            conn, addr = s.accept()
            threading.Thread(target=handle_client, args=(conn, addr), daemon=True).start()

if __name__ == "__main__":
    try:
        accept_loop()
    except KeyboardInterrupt:
        print("\n[i] Shutting down.")
