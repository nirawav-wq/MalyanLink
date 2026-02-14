import socket, psutil, time, comtypes
from datetime import datetime
from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume

ESP_IP = "192.168.188.XX"
SEND_PORT = 4210
RECV_PORT = 4211

def get_system_temp():
    try:
        temps = psutil.sensors_temperatures()
        for key in ['coretemp', 'amdtemp', 'package']:
            if key in temps: return int(temps[key][0].current)
    except: pass
    return int(35 + (psutil.cpu_percent() * 0.4))

print("MalyanLink Host active...")
# (Rest of the logic from our previous stable version)
