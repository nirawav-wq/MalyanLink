import socket
import psutil
import time
import comtypes
from datetime import datetime
from ctypes import cast, POINTER
from comtypes import CLSCTX_ALL
from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume

# --- CONFIGURATION ---
ESP_IP = "192.168.188.XX" # Replace with your ESP IP
SEND_PORT = 4210  
RECV_PORT = 4211  

def get_system_temp():
    try:
        temps = psutil.sensors_temperatures()
        for key in ['coretemp', 'amdtemp', 'package', 'cpu_thermal']:
            if key in temps:
                return int(temps[key][0].current)
    except:
        pass
    return int(35 + (psutil.cpu_percent() * 0.4))

# --- AUDIO SETUP ---
comtypes.CoInitialize()
try:
    from pycaw.pycaw import IMMDeviceEnumerator
    from comtypes import GUID
    CLSID_MMDeviceEnumerator = GUID('{BCDE0395-E52F-467C-8E3D-C4579291692E}')
    IID_IMMDeviceEnumerator = GUID('{A95664D2-9614-4135-A930-DE54069692E7}')
    enumerator = comtypes.CoCreateInstance(CLSID_MMDeviceEnumerator, IMMDeviceEnumerator, CLSCTX_ALL)
    endpoint = enumerator.GetDefaultAudioEndpoint(0, 1)
    interface = endpoint.Activate(IAudioEndpointVolume._iid_, CLSCTX_ALL, None)
    volume = cast(interface, POINTER(IAudioEndpointVolume))
except Exception as e:
    print(f"Audio-Error: {e}")
    exit()

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(("0.0.0.0", RECV_PORT))
sock.setblocking(False) 

def set_volume(delta):
    try:
        current_v = volume.GetMasterVolumeLevelScalar()
        new_v = max(0.0, min(1.0, current_v + delta))
        volume.SetMasterVolumeLevelScalar(new_v, None)
        return int(new_v * 100)
    except: return 0

last_sync = 0
print("MalyanLink Host active...")

try:
    while True:
        try:
            data, addr = sock.recvfrom(1024)
            msg = data.decode().strip()
            if "V_UP" in msg:
                v = set_volume(0.02)
                sock.sendto(f"VOL:{v}".encode(), (ESP_IP, SEND_PORT))
            elif "V_DOWN" in msg:
                v = set_volume(-0.02)
                sock.sendto(f"VOL:{v}".encode(), (ESP_IP, SEND_PORT))
        except BlockingIOError: pass

        if time.time() - last_sync > 1.0:
            cpu = int(psutil.cpu_percent())
            ram = int(psutil.virtual_memory().percent)
            temp = get_system_temp()
            vol = int(volume.GetMasterVolumeLevelScalar() * 100)
            now = datetime.now().strftime('%H:%M:%S')

            sock.sendto(f"CLK:{now}".encode(), (ESP_IP, SEND_PORT))
            sock.sendto(f"CPU:{cpu}".encode(), (ESP_IP, SEND_PORT))
            sock.sendto(f"RAM:{ram}".encode(), (ESP_IP, SEND_PORT))
            sock.sendto(f"TMP:{temp}".encode(), (ESP_IP, SEND_PORT))
            sock.sendto(f"VOL:{vol}".encode(), (ESP_IP, SEND_PORT))
            last_sync = time.time()
        time.sleep(0.01)
except KeyboardInterrupt: pass
finally: sock.close(); comtypes.CoUninitialize()
