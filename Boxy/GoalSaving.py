import cv2
import numpy as np
import math
import asyncio
from bleak import BleakClient

# BLE Configuration
BLE_ADDRESS = "D41C0CF6-3DFD-5844-4DA8-7746828451C3"  # replace with your device address
CHAR_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"

# HSV tolerances and selection
tolerance_hue = 10
tolerance_sat = 40
tolerance_val = 40
clicked_hsv = None

# Mouse callback to select HSV
def click_to_mask(event, x, y, flags, param):
    global clicked_hsv
    if event == cv2.EVENT_LBUTTONDOWN:
        clicked_hsv = hsv_frame[y, x]
        print(f"New Clicked HSV: {clicked_hsv}")

async def ble_connect_and_send():
    async with BleakClient(BLE_ADDRESS) as client:
        print(f"✔ Connected to {BLE_ADDRESS}")

        cap = cv2.VideoCapture(0)
        cv2.namedWindow("Original")
        cv2.setMouseCallback("Original", click_to_mask)

        last_angle = None
        while True:
            ret, frame = cap.read()
            if not ret:
                break

            height, width = frame.shape[:2]
            cx0, cy0 = width // 2, height  # bottom-center origin

            # Convert to HSV and create mask
            global hsv_frame
            hsv_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
            mask = np.zeros((height, width), dtype="uint8")

            if clicked_hsv is not None:
                lower = np.array([
                    max(int(clicked_hsv[0]) - tolerance_hue, 0),
                    max(int(clicked_hsv[1]) - tolerance_sat, 0),
                    max(int(clicked_hsv[2]) - tolerance_val, 0)
                ])
                upper = np.array([
                    min(int(clicked_hsv[0]) + tolerance_hue, 179),
                    min(int(clicked_hsv[1]) + tolerance_sat, 255),
                    min(int(clicked_hsv[2]) + tolerance_val, 255)
                ])
                mask = cv2.inRange(hsv_frame, lower, upper)

                # Find largest contour and centroid
                contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
                if contours:
                    c = max(contours, key=cv2.contourArea)
                    M = cv2.moments(c)
                    if M["m00"] != 0:
                        cx = int(M["m10"] / M["m00"])
                        cy = int(M["m01"] / M["m00"])

                        # Draw visuals
                        cv2.circle(frame, (cx, cy), 5, (0, 255, 0), -1)
                        cv2.circle(frame, (cx0, cy0), 5, (0, 0, 255), -1)
                        cv2.line(frame, (cx0, cy0), (cx, cy), (255, 255, 0), 2)

                        # Compute angle theta and map to servo
                        dx = cx - cx0
                        dy = cy - cy0
                        theta = math.degrees(math.atan2(dy, dx))  # -180..+180
                        servo_angle = int((theta + 180) / 2)
                        servo_angle = max(0, min(servo_angle, 180))
                        servo_angle = -servo_angle  # invert

                        # Only send when change > 5 degrees
                        if last_angle is None or abs(servo_angle - last_angle) > 2:
                            await client.write_gatt_char(CHAR_UUID, str(abs(2*servo_angle)).encode())
                            print(f"→ Sent servo angle: {servo_angle}°")
                            last_angle = servo_angle

                        # Annotate
                        cv2.putText(frame,
                                    f"Centroid: ({cx},{cy})",
                                    (10, height - 60), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)
                        cv2.putText(frame,
                                    f"θ={theta:.1f}° -> Servo={servo_angle}°",
                                    (10, height - 30), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 255), 2)
                        print(f"Centroid x={cx}, y={cy}, θ={theta:.1f}°, mapped servo={servo_angle}°")

            # Display
            masked = cv2.bitwise_and(frame, frame, mask=mask)
            cv2.imshow("Original", frame)
            cv2.imshow("Masked Color", masked)

            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

        cap.release()
        cv2.destroyAllWindows()
        print("✔ Disconnected.")

if __name__ == "__main__":
    asyncio.run(ble_connect_and_send())
