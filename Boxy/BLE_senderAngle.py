"""
Please not this code rather sending a chracter sending numbers and 
placing constraint  0- 180 degree 
as this number will go as a string you will need a diffrent code at ESP side to convert it back to an integer 
so use ESP_ServoAngle.ino in ESP to make it work 

Written by Rahul Sharma for YoLabs on - 7th Aug' 25

"""


import asyncio
from bleak import BleakClient

# Configure your ESP32 BLE address and characteristic UUID
ADDRESS = "D41C0CF6-3DFD-5844-4DA8-7746828451C3"  # replace with your device address
CHAR_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"

async def run(address):
    async with BleakClient(address) as client:
        print(f"✔ Connected to {{address}} via BLE")
        while True:
            cmd = input("Enter angle (0-180) or 'q' to quit: ").strip()
            if cmd.lower() == 'q':
                print("Exiting...")
                break

            if cmd.isdigit():
                angle = int(cmd)
                if 0 <= angle <= 180:
                    # send ASCII-encoded number to ESP32
                    await client.write_gatt_char(CHAR_UUID, cmd.encode())
                    print(f"→ Sent angle: {angle}")
                else:
                    print("⚠️  Angle out of range. Enter 0–180.")
            else:
                print("⚠️  Invalid input. Please enter a number or 'q'.")

asyncio.run(run(ADDRESS))
