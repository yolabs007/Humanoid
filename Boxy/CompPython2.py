"""
This code is to send  a and b to ESP 32 
Note: you must install  the library Bleak 
as well as run BleakScanner Code to get the esp32 BLE MAC address 
based on this a and b command you can control light ot servo @ ESP side 
 
"""



import asyncio
from bleak import BleakClient

ADDRESS = "D41C0CF6-3DFD-5844-4DA8-7746828451C3"  # ESP32 BLE Address (via BLE Scanner Code )
CHAR_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"

async def run(address):
    async with BleakClient(address) as client:
        print("Connected to ESP32 BLE!")
        while True:
            cmd = input("Enter 'a' or 'b' (q to quit): ").lower()
            if cmd in ['a', 'b']:
                await client.write_gatt_char(CHAR_UUID, cmd.encode())
                print(f"Sent: {cmd}")
            elif cmd == 'q':
                break
            else:
                print("Invalid input.")

asyncio.run(run(ADDRESS))
