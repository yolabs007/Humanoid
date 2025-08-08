"""
Run this code to scan all avaialbe BLE  devices. 
Copy the MAC address and paste in your pythoncode- BLE_senderAnB.py to connected to this device 

"""


import asyncio
from bleak import BleakScanner

async def scan_ble():
    print("Scanning for BLE devices for 5 seconds...")
    devices = await BleakScanner.discover(timeout=5.0)
    print("\nFound devices:")
    for d in devices:
        print(f"{d.address}    {d.name}")

if __name__ == "__main__":
    asyncio.run(scan_ble())
